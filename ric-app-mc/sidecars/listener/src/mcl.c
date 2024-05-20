// vim: ts=4 sw=4 noet:
/*
--------------------------------------------------------------------------------
	Copyright (c) 2018-2019 AT&T Intellectual Property.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

	   http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
--------------------------------------------------------------------------------
*/

/*
	Mnemonic:	mcl.c.
	Abstract:	The mc listener library content. All external functions
				should start with mcl_ and all stderr messages should have
				(mcl) as the first token following the severity indicator.

	Date:		22 August 2019
	Author:		E. Scott Daniels
*/

#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>


#include <rmr/rmr.h>
#include <rmr/rmr_symtab.h>
#include <rmr/RIC_message_types.h>

#include "mcl.h"

#ifndef FOREVER
#define FOREVER 1
#endif

#define READER 0
#define WRITER 1

#define TRUE	1
#define FALSE	0

/*
	Information about one file descriptor. This is pointed to by the hash
	such that the message type can be used as a key to look up the fifo's
	file descriptor.
*/
typedef struct {
	int	fd;					// open fdes
	int key;				// symtab key
	long long wcount;		// number of writes
	long long drops;		// number dropped

	long long wcount_rp;	// number of writes during last reporting period
	long long drops_rp;		// number dropped during last reporting period
} fifo_t;

/*
	Our conext.  Pointers to the read and write hash tables (both keyed on the message
	type), the message router (RMR) context, and other goodies.
*/
typedef struct {
	void*	mrc;				// the message router's context
	void*	wr_hash;			// symtable to look up pipe info based on mt for writing
	void*	rd_hash;			// we support reading from pipes, but need a different FD for that
	char*	fifo_dir;			// directory where we open fifos

} mcl_ctx_t;

// -------- private -------------------------------------------------------


/*
	Set up for raw data capture. We look for directory overriedes from
	environment variables, and then invoke the rdc_init() to actually
	set things up.
*/
static void* setup_rdc() {
	void*	ctx;
	int		value;							// value computed for something
	char*	ep;								// pointer to environment var
	char*	sdir = "/tmp/rdc/stage";		// default directory names
	char*	fdir = "/tmp/rdc/final";
	char*	suffix = ".rdc";
	char*	done = NULL;

	if( (ep = getenv( "MCL_RDC_ENABLE" )) != NULL && atoi( ep ) == 0 ) {					// exists and is 0
		logit( LOG_INFO, "(mcl) raw data capture disabled by environment var setting (MCL_RDCENABLE=0)" );
		return NULL;
	}

	if( (ep = getenv( "MCL_RDC_STAGE" )) != NULL ) {
		sdir = ep;
	} else {
		mkdir( "/tmp/rdc", 0755 );			// we ignore failures here as it could likely exist
		mkdir( sdir, 0755 );
	}

	if( (ep = getenv( "MCL_RDC_FINAL" )) != NULL ) {
		fdir = ep;
	} else {
		mkdir( "/tmp/rdc", 0755 );			// we ignore failures again -- very likely it's there
		mkdir( fdir, 0755 );
	}

	if( (ep = getenv( "MCL_RDC_SUFFIX" )) != NULL ) {
		suffix = ep;
	}

	if( (ep = getenv( "MCL_RDC_DONE" )) != NULL ) {
		done = ep;
	}

	ctx = rdc_init( sdir, fdir, suffix, done );
	if( ctx == NULL ) {
		logit( LOG_ERR, "rdc_init did not generate a context" );
	} else {
		logit( LOG_INFO, "raw data capture files will be staged in: %s", sdir );
		logit( LOG_INFO, "raw data capture files will be moved for copy to: %s", fdir );
	}

	if( (ep = getenv( "MCL_RDC_FREQ" )) != NULL ) {
		value = atoi( ep );
		logit( LOG_INFO, "setting frequency: %d", value );
		rdc_set_freq( ctx, value );
	}
	return ctx;
}

/*
	Builds an extended header in the buffer provided, or allocates a new buffer if
	dest is nil. The header is of the form:
		<delim><len><timestamp>

	Field lengths (bytes) are:
		delim		4    
		len			8	(7 digits + 0)
		timestamp	16  (15 digits + 0)


	Timestamp is a single unsigned long long in ASCII; ms since epoch.
	If the current time is 2019/10/03 10:39:51.103 which is 1570113591.103
	the timestamp generated will be 1570113591103.

	The lenght and timestamp fields in the header are zero terminated so
	they can be parsed as a string (atoi etc).
*/
static char* build_hdr( int len, char* dest, int dest_len ) {
	struct timespec ts;         // time just before call executed

	if( dest == NULL ) {
		dest_len = MCL_EXHDR_SIZE + 2;			// more than enough room
		dest = (char *) malloc( sizeof( char ) * dest_len );
	} else {
		if( dest_len < MCL_EXHDR_SIZE ) {		// shouldn't happen, but take no chances
			memset( dest, 0, dest_len );
			return NULL;
		}
	}

	memset( dest, 0, dest_len );

	clock_gettime( CLOCK_REALTIME, &ts );
	snprintf( dest, dest_len, "%s%07d", MCL_DELIM, len );
	snprintf( dest+12, dest_len-13, "%ld%03ld", ts.tv_sec, ts.tv_nsec/1000000 );

	return dest;
}

/*
	Build a file name and open. The io_direction is either READER or
	WRITER.  For a writer we must 'trick' the system into allowing us
	to open a pipe for writing in non-blocking mode so that we can
	report on drops (messages we couldn't write because there was no
	reader).  The trick is to open a reader on the pipe so that when
	we open the writer there's a reader and the open won't fail. As
	soon as we have the writer open, we can close the junk reader.

	If the desired fifo does not exist, it is created.
*/
static int open_fifo( mcl_ctx_t* ctx, int mtype, int io_dir ) {
	char	wbuf[1024];
	int		fd;					// real file des
	int		jfd = -1;			// junk file des
	int		state;

	if( ctx == NULL || mtype < 0 ) {
		return -1;
	}

	snprintf( wbuf, sizeof( wbuf ), "%s/MT_%09d", ctx->fifo_dir, mtype );

	state = mkfifo( wbuf, 0660 );		// make the fifo; this will fail if it exists and that's ok
	if( state != 0 && errno != EEXIST ) {
		logit(  LOG_ERR, "(mcl) unable to create fifo: %s: %s", wbuf, strerror( errno ) );
		return -1;
	}

	if( io_dir == READER ) {
		fd = open( wbuf, O_RDONLY  );			// just open the reader
		if( fd < 0 ) {
			logit(  LOG_ERR, "(mcl) fifo open failed (ro): %s: %s", wbuf, strerror( errno ) );
		}
	} else {
		jfd = open( wbuf, O_RDWR  | O_NONBLOCK );			// must have a reader before we can open a non-blocking writer
		if( jfd < 0 ) {
			logit(  LOG_ERR, "(mcl) fifo open failed (rw): %s: %s", wbuf, strerror( errno ) );
			return -1;
		}

		fd = open( wbuf, O_WRONLY  | O_NONBLOCK );			// this will be our actual writer, in non-blocking mode
		if( fd < 0 ) {
			logit(  LOG_ERR, "(mcl) fifo open failed (wo): %s: %s", wbuf, strerror( errno ) );
		}

		close( jfd );			// should be safe to close this
	}


	return fd;
}

/*
	Given a message type, return the file des of the fifo that
	the payload should be written to.	 Returns the file des, or -1
	on error. When sussing out a read file descriptor this will
	block until there is a fifo for the message type which is
	open for reading.

	If fref is not nil, then a pointer to the fifo info block is returned
	allowing for direct update of counts after the write.
*/
static int suss_fifo( mcl_ctx_t* ctx, int mtype, int io_dir, fifo_t** fref ) {
	fifo_t* fifo = NULL;
	void*	hash;

	if( ctx == NULL ) {
		if( fref != NULL ) {
			*fref = NULL;
		}
		return -1;
	}

	if( io_dir == READER ) {		// with an integer key, we need two hash tables
		hash = ctx->rd_hash;
	} else {
		hash = ctx->wr_hash;
	}

	if( (fifo = (fifo_t *) rmr_sym_pull( hash, mtype )) == NULL ) {
		fifo = (fifo_t *) malloc( sizeof( *fifo ) );
		if( fifo != NULL ) {
			memset( fifo, 0, sizeof( *fifo ) );
			fifo->key = mtype;
			fifo->fd = open_fifo( ctx, mtype, io_dir );
			if( fifo->fd >= 0 ) {					// save only on good open
				rmr_sym_map( hash, mtype, fifo );
			} else {
				free( fifo );
				fifo = NULL;
			}
		}
	} else {
		if( fifo->fd < 0 ) {				// it existed, but was closed; reopen
			fifo->fd = open_fifo( ctx, mtype, io_dir );
		}
	}

	if( fref != NULL ) {
		*fref = fifo;
	}

	return fifo == NULL ? -1 : fifo->fd;
}

/*
	Should we need to close a FIFO we do so and leave the block in the hash
	with a bad FD so that we'll attempt to reopen on next use.
*/
static void close_fifo( mcl_ctx_t* ctx, int mtype, int io_dir ) {
	fifo_t* fifo;
	void*	hash;

	if( ctx == NULL ) {
		return;
	}

	if( io_dir == READER ) {		// with an integer key, we need two hash tables
		hash = ctx->rd_hash;
	} else {
		hash = ctx->wr_hash;
	}

	if( (fifo = (fifo_t *) rmr_sym_pull( hash, mtype )) != NULL ) {
		if( fifo->fd >= 0 ) {
			close( fifo->fd );
			fifo->fd = -1;
		}
	}
}

/*
	Make marking counts easier in code
*/
static inline void chalk_error( fifo_t* fifo ) {
	if( fifo != NULL ) {
		fifo->drops++;
		fifo->drops_rp++;
	}
}

static inline void chalk_ok( fifo_t* fifo ) {
	if( fifo != NULL ) {
		fifo->wcount++;
		fifo->wcount_rp++;
	}
}

/*
	Callback function driven to traverse the symtab and generate the
	counts for each fifo.  Sonar will complain about unused parameters which
	are normal for callbacks. Further, sonar will grumble about st, and entry
	not being const; we can't unless RMR proto for the callback changes.
*/
static void wr_stats( void* st, void* entry, char const* name, void* thing, void* data ) {
	fifo_t*	fifo;
	int		report_period = 60;

	if( data ) {
		report_period = *((int *) data);
	}

	if( (fifo = (fifo_t *) thing) != NULL ) {
		logit( LOG_STAT, "(mcl) mtype=%d total writes=%lld total drops=%lld; during last %ds writes=%lld drops=%lld",
			fifo->key, fifo->wcount, fifo->drops, report_period, fifo->wcount_rp, fifo->drops_rp );

		fifo->wcount_rp = 0;		// reset the report counts
		fifo->drops_rp = 0;
		return;						// return here to avoid sonar required hack below
	}

	/*
		Sonar doesn't grok the fact that for callback functions some parms are naturally
		ignored. So, to eliminate the 5 code smells because we only care about thing, we
		have this hack....
	*/
	if( st == NULL && entry == NULL && name == NULL && data == NULL ) {
		fprintf( stderr, "mdcl: all parms to callback stats were nil\n" );
	}
}

/*
	Writes the indicated bytes (n2write) from buf onto the fd. Returns only after
	full buffer is written, or there is a hard error (not eagain or eintr).
	Returns the number written; if less than n2write the caller may assume
	that there was a hard error and errno should reflect.
*/
static inline int write_all( int fd, char const* buf, int n2write ) {
	ssize_t remain = 0;			// number of bytes remaining to write
	ssize_t wrote = 0;			// number of bytes written thus far
	ssize_t state = 0;

	if( fd < 0 ) {
		errno = EBADFD;
		return 0;
	}

	errno = 0;
	remain = n2write;
	do {
		if( (state = write( fd, buf + wrote, remain )) > 0 ) {
			wrote += state;
			remain = n2write - wrote;
		}
	} while( remain > 0 && (errno == EINTR || errno == EAGAIN) ) ;

	return wrote;
}

/*
	Similar to write_all, this will write all bytes in the buffer, but
	will return failure if the first write attempt fails with 0 written
	(assuming that the pipe has no reader). We use this when writing the
	header bytes; we want to drop the message if we can't even write one
	byte, but if we write one, we must loop until all are written.

	Returns the number written. If that value is less than n2write, then
	the caller may assume a hard error occurred and errno should reflect.
	If 0 is returned it can be assumed that the FIFO would block/has no
	reader.
*/
static inline int write_all_nb( int fd, char const* buf, int n2write ) {
	ssize_t remain = 0;			// number of bytes remaining to write
	ssize_t wrote = 0;			// number of bytes written

	if( fd < 0 ) {
		errno = EBADFD;
		return 0;
	}

	errno = 0;
	remain = n2write;
	wrote = write( fd, buf, remain );
	if( wrote < 0 ) {								// report error with exception for broken pipe
		return errno == EPIPE ? 0 : -1;				// broken pipe we assume no reader and return 0 since nothing written
	}

	if( wrote < n2write  &&  wrote > 0 ) {			// if we wrote anything, we must tough it out and write all if it was short
		wrote +=  write_all( fd, buf + wrote, n2write - wrote );
	}

	return wrote;
}

// ---------- public ------------------------------------------------------
/*
	Sets a signal handler for sigpipe so we don't crash if a reader closes the
	last reading fd on a pipe. We could do this automatically, but if the user
	programme needs to trap sigpipe too, this gives them the option not to have
	us interfere.
*/
extern int mcl_set_sigh( ) {
	signal( SIGPIPE, SIG_IGN );
}

/*
	"Opens" the interface to RMR such that messages sent to the application will
	be available via the rmr receive funcitons. This is NOT automatically called
	by the mk_context function as some applications will be using the mc library
	for non-RMR, fifo, chores.
*/
extern int mcl_start_listening( void* vctx,  char* port, int wait4ready ) {
	mcl_ctx_t*	ctx;
	int		announce = 0;

	if( (ctx = (mcl_ctx_t*) vctx) == NULL ) {
		return 0;
	}

	ctx->mrc = rmr_init( port, RMR_MAX_RCV_BYTES, RMRFL_NONE );	// start your engines!
	if( ctx->mrc == NULL ) {
		logit(  LOG_CRIT, "start listening: unable to initialise RMr" );
		return 0;
	}

	while( wait4ready && ! rmr_ready( ctx->mrc ) ) {				// only senders need to wait
		if( announce <= 0 ) {
			logit(  LOG_INFO, "waiting for RMR to show ready" );
			announce = 10;
		} else {
			announce--;
		}

		sleep( 1 );
	}

	return 1;
}

/*
	Blocks until a message arives with a good return code or we timeout. Returns the
	rmr message buffer. Timeout value epxected in seconds.
*/
extern rmr_mbuf_t* mcl_get_msg( void* vctx, rmr_mbuf_t* msg, int timeout ) {
	mcl_ctx_t*	ctx;

	if( (ctx = (mcl_ctx_t *) vctx) == NULL ) {
		return NULL;
	}

	if( ctx->mrc == NULL ) {
		logit(  LOG_CRIT, "get msg: abort: bad rmr context reference (nil)" );
		exit( 1 );
	}

	do {
		msg = rmr_torcv_msg( ctx->mrc, msg, timeout * 1000 );				// wait for next
	} while( msg == NULL || (msg->state != RMR_OK && msg->state != RMR_ERR_TIMEOUT) );

	return msg;
}

/*
	Create the context.
*/
extern	void* mcl_mk_context( const char* dir ) {
	mcl_ctx_t*	ctx;

	if( (ctx = (mcl_ctx_t *) malloc( sizeof( *ctx ) )) != NULL ) {
		memset( ctx, 0, sizeof( *ctx ) );
		ctx->fifo_dir = strdup( dir );
		ctx->wr_hash = rmr_sym_alloc( 1001 );
		ctx->rd_hash = rmr_sym_alloc( 1001 );

		if( ctx->wr_hash == NULL  || ctx->rd_hash == NULL ) {
			logit(  LOG_ERR, "(mcl) unable to allocate hash table for fifo keys" );
			free( ctx );
			return NULL;
		}
	}

	return (void *) ctx;
}

/*
	Read the header. Best case we read the expected number of bytes, get all
	of them and find that they start with the delemiter.  Worst case
	We have to wait for all of the header, or need to synch at the next
	delimeter. We assume best case most likely and handle it as such.
*/
static void read_header( int fd, char* buf ) {
	size_t len;
	size_t need = MCL_EXHDR_SIZE;		// total needed
	size_t dneed;						// delimieter needed
	char*	rp;							// read position in buf

	len = read( fd, buf, need );
	if( len == need && strncmp( buf, MCL_DELIM, strlen( MCL_DELIM )) == 0 ) {	// best case, most likely
		return;
	}

	while( TRUE ) {
		if( len < strlen( MCL_DELIM ) ) {		// must get at least enough bytes to check delim
			rp = buf + len;
			dneed = strlen( MCL_DELIM ) - len;

			while( dneed > 0 ) {
				len = read( fd, rp, dneed );
				dneed -= len;
				rp += len;
			}
		}

		if( strncmp( buf, MCL_DELIM, strlen( MCL_DELIM )) == 0 ) {	// have a good delimiter, just need to wait for bytes
			need = MCL_EXHDR_SIZE - strlen( MCL_DELIM );
			rp = buf + (MCL_EXHDR_SIZE - need);

			while( need > 0 ) {
				len = read( fd, rp, need );
				need -= len;
				rp += len;
			}

			return;
		}

		while( buf[0] != MCL_DELIM[0] )	{	// wait for a recognised start byte to be read (may cause an additional message drop
			len = read( fd, buf, 1 );		// because we ignore start byte that might be in the buffer)
		}
	}
}


/*
	Read one record from the fifo that the message type maps to.
	Writes at max ublen bytes into the ubuf.

	If long_hdrs is true (!0), then we expect that the stream in the fifo
	has extended headers (<delim><len><time>), and will write the timestamp
	from the header into the buffer pointed to by timestamp. The buffer is
	assumed to be at least MCL_TSTAMP_SIZE bytes in length.

	Further, when extended headers are being used, this function will
	automatically resynchronise if it detects an issue.

	The function could look for the delimiter and automatically detect whether
	or not extended headers are in use, but if the stream is out of synch on the
	first read, this cannot be done, so the funciton requires that the caller
	know that the FIFO contains extended headers.
*/
static int fifo_read1( void *vctx, int mtype, char* ubuf, int ublen, int long_hdrs, char* timestamp ) {
	int fd;
	int len;
	int	msg_len;
	int	got = 0;						// number of bytes we actually got
	int need;
	char wbuf[4096];
	mcl_ctx_t*	ctx;					// our context; mostly for the rmr context reference and symtable

	if( (ctx = (mcl_ctx_t*) vctx) == NULL ) {
		errno = EINVAL;
		return 0;
	}

	if( (fd = suss_fifo( ctx, mtype, READER, NULL ))  >= 0 )  {
		if( long_hdrs ) {
			read_header( fd, wbuf );
			msg_len = need = atoi( wbuf + MCL_LEN_OFF );				// read the length
			if( timestamp ) {
				strncpy( timestamp, wbuf + MCL_TSTAMP_OFF+1, MCL_TSTAMP_SIZE );
			}
		} else {
			if( timestamp != NULL ) {						// won't be there, but ensure it's not garbage
				*timestamp = 0;
			}

			read( fd, wbuf, MCL_LEN_SIZE );					// we assume we will get all 8 bytes as there isn't a way to sync the old stream
			msg_len = need = atoi( wbuf );
		}


		if( need > ublen ) {
			need = ublen;						// cannot give them more than they can take
		}
		while( need > 0 ) {
			len = read( fd, wbuf, need > sizeof( wbuf ) ? sizeof( wbuf ) : need );
			memcpy( ubuf+got, wbuf, len );
			got += len;
			need -= len;
		}

		if( msg_len > got ) {					// we must ditch rest of this message
			need = msg_len - got;
			while( need > 0 ) {
				len = read( fd, wbuf, need > sizeof( wbuf ) ? sizeof( wbuf ) : need );
				need -= len;
			}
		}

		return got;
	}

	errno = EBADFD;
	return 0;
}

/*
	Read one record from the fifo that the message type maps to.
	Writes at max ublen bytes into the ubuf. If extended headers are in use
	this function will ignore the timestamp.

	If long_hdrs is true (!0), then we expect that the stream in the fifo
	has extended headers (<delim><len><time>).
*/
extern int mcl_fifo_read1( void *vctx, int mtype, char* ubuf, int ublen, int long_hdrs ) {
	return fifo_read1( vctx, mtype, ubuf, ublen, long_hdrs, NULL );
}

/*
	Read a single message from the FIFO returning it in the caller's buffer. If extended
	headers are being used, and the caller supplied a timestamp buffer, the timestamp
	which was in the header will be returned in that buffer.  The return value is the number
	of bytes in the buffer; 0 indicates an error and errno should be set.
*/
extern int mcl_fifo_tsread1( void *vctx, int mtype, char* ubuf, int ublen, int long_hdrs, char *timestamp ) {
	return fifo_read1( vctx, mtype, ubuf, ublen, long_hdrs, timestamp );
}


/*
	Will read messages and fan them out based on the message type. This should not
	return and if it does the caller should assume an error.

	The output to each fifo is MCL_LEN_SIZE bytes with an ASCII, zero terminated, length
	string , followed by that number of 'raw' bytes. The raw bytes are the payload
	exactly as received.

	The report parameter is the frequency, in seconds, for writing a short
	status report to stdout. If 0 then it's off.

	If long_hdr is true, then we geneate an extended header with a delimiter and
	timestamp.

	The one message which is NOT pushed into a FIFO is the RIC_HEALTH_CHECK_REQ
	message.  When the health check message is received it is responded to
	with the current state of processing (ok or err).
*/
extern void mcl_fifo_fanout( void* vctx, int report, int long_hdr ) {
	mcl_ctx_t*	ctx;					// our context; mostly for the rmr context reference and symtable
	fifo_t*		fifo;					// fifo to chalk counts on
	rmr_mbuf_t*	mbuf = NULL;			// received message buffer; recycled on each call
	char		header[128];			// header we'll pop in front of the payload
	int			fd;						// file des to write to
	long long	total = 0;				// total messages received and written
	long long	total_drops = 0;		// total messages received and written
	long		count = 0;				// messages received and written during last reporting period
	long		errors = 0;				// unsuccessful payload writes
	long		drops = 0;				// number of drops
	time_t		next_report = 0;		// we'll report every 2 seconds if report is true
	time_t		now;
	size_t		hwlen;					// write len for header
	size_t		wrote;					// number of bytes actually written
	void*		rdc_ctx = NULL;			// raw data capture context
	void*		rdc_buf = NULL;			// capture buffer

	if( (ctx = (mcl_ctx_t*) vctx) == NULL ) {
		logit(  LOG_ERR, "(mcl) invalid context given to fanout" );
		errno = EINVAL;
		return;
	}

	if( report < 0 ) {
		report = 0;
	}

	rdc_ctx = setup_rdc( );				// pull rdc directories from enviornment and initialise

	do {
		mbuf = mcl_get_msg( ctx, mbuf, report );			// wait up to report sec for msg (0 == block until message)

		if( mbuf != NULL && mbuf->state == RMR_OK ) {
			if( mbuf->mtype == RIC_HEALTH_CHECK_REQ ) {
				mbuf->mtype = RIC_HEALTH_CHECK_RESP;		// if we're here we are running and all is ok
				mbuf->sub_id = -1;
				mbuf = rmr_realloc_payload( mbuf, 128, FALSE, FALSE );	// ensure payload is large enough
				if( mbuf->payload != NULL ) {
					strncpy( mbuf->payload, "OK\n", rmr_payload_size( mbuf) );
					rmr_rts_msg( ctx->mrc, mbuf );
				}
				continue;
			}

			if( mbuf->len > 0  ) {
				if( long_hdr ) {
					build_hdr( mbuf->len, header, sizeof( header ) );
					hwlen = MCL_EXHDR_SIZE;
				} else {
					snprintf( header, sizeof( header ), "%07d", mbuf->len );			// size of payload CAUTION: 7d is MCL_LEN_SIZE-1
					hwlen = MCL_LEN_SIZE;
				}

				fd = suss_fifo( ctx, mbuf->mtype, WRITER, &fifo );					// map the message type to an open fd
				if( fd >= 0 ) {
					if( (wrote = write_all_nb( fd, header, hwlen )) == 0 ) {		// write header; 0 indicates no reader, drop silently
						drops++;
						total_drops++;
						chalk_error( fifo );
					} else {
						if( wrote != hwlen ) {
							logit( LOG_ERR, "(mcl): error writing header to fifo; mt=%d wrote=%d tried=%d: %s", mbuf->mtype, wrote, hwlen, strerror( errno ) );
							errors++;
							chalk_error( fifo );
							close_fifo( ctx, mbuf->mtype, WRITER );
						} else {
							if( write_all( fd, mbuf->payload, mbuf->len ) != mbuf->len ) {		// we wrote a header, so we must write all; no drop at this point
								logit( LOG_ERR, "(mcl): error writing payload to fifo; mt=%d: %s\n", mbuf->mtype, strerror( errno ) );
								close_fifo( ctx, mbuf->mtype, WRITER );
							} else {
								chalk_ok( fifo );
								count++;
								total++;
							}
						}
					}
				}

				if( rdc_ctx != NULL ) {						// always put the message to the rdc files if collecting; eve if pipe write failed
					rdc_buf = rdc_init_buf( mbuf->mtype, header, hwlen, rdc_buf );			// set up for write
					rdc_write( rdc_ctx, rdc_buf, mbuf->payload, mbuf->len );				// write the raw data
				}
			}
		}

		if( report ) {
			if( (now = time( NULL ) ) > next_report ) {
				rmr_sym_foreach_class( ctx->wr_hash, 0, wr_stats, &report );        // run endpoints in the active table
				fflush( stdout );

				logit( LOG_STAT, "(mcl) total writes=%lld total drops=%lld; during last %ds writes=%ld drops=%ld errs=%ld errors",
					total, total_drops, report, count, drops, errors );
				next_report = now + report;
				count = 0;
				drops = 0;
				errors = 0;

				fflush( stdout );
			}
		}

		if( ! FOREVER ) {			// allow escape during unit tests; compiled out othewise, but sonar won't see that
			free( rdc_buf );
			break;					// sonar grumbles if we put FOREVER into the while; maddening
		}
	} while( 1 );
}


/*
	Given a buffer and length, along with the message type, look up the fifo and write
	the buffer. Returns 0 on error; 1 on success.
*/
extern int mcl_fifo_one( void* vctx, const char* payload, int plen, int mtype ) {
	mcl_ctx_t*	ctx;					// our context; mostly for the rmr context reference and symtable
	fifo_t*		fifo;					// fifo to chalk counts on
	size_t		state = -1;
	int			fd;						// file des to write to

	if( plen <= 0  || payload == NULL ) {
		return 1;
	}

	if( (ctx = (mcl_ctx_t*) vctx) == NULL ) {
		logit( LOG_ERR, "(mcl) invalid context given to fifo_one\n" );
		return 0;
	}

	fd = suss_fifo( ctx, mtype, WRITER, &fifo );		// map the message type to an open fd
	if( fd >= 0 ) {
		state = write( fd, payload, plen );
	}

	return state == (size_t) plen;
}
