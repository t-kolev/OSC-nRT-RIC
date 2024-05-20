
/*
	Mnemonic:	rdc_extract.c
	Abstract:	Read a raw data capture file from the mc-listener
				and tease out one message type.

					0000000 40 52 44 43 				<< delim
										30 30 31 30 30 35 30 2a  << mtype
				 												30 30 30 30 << msg len
					0000020 30 37 34 00 
										40 4d 43 4c 30 30 30 30 30 34 36 00 << raw message
							:
							:

				This is a very quick and dirty thing, so it might be flakey.

				Parms from command line are file to read, and the msg type to extract.
				If mtype given is 0, then message type of each record is written to
				stdout (can be sorted -u for a list of messages in the file).

				For capture mode, a file MT_<mtype> is created for the extracted
				records.

	Date:		11 October 2019
	Author:		E. Scott Daniels
*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

int main( int argc, char** argv ) {
	char rbuf[1024 * 5];
	int fd;
	int wfd = 1;		// write file des; default to stdout
	int mtype;
	int mlen;
	char*	nxt;
	int	remain = 0;
	int	need = 20;
	int desired;
	int captured = 0;
	int	wlen = 0;

	if( argc < 3 ) {
		fprintf( stderr, "bad args.\nUsage: %s file mtype [output-file]\n", argv[0] );
		exit( 1 ) ;
	}

	fd = open( argv[1], O_RDONLY );
	if( fd < 0 ) {
		fprintf( stderr, "bad open: %s\n", strerror(errno) );
		exit( 1 );
	}

	desired = atoi( argv[2] );

	if( argc > 3 ) {
		wfd = open( argv[4], O_WRONLY | O_CREAT | O_TRUNC, 0644 );
	} 

	remain = read( fd, rbuf, sizeof( rbuf ) );
	nxt = rbuf;
	while( remain > 0 ) {
		if( remain < 20 ) {					// not enough stuff
			memcpy( rbuf, nxt, remain );	// copy remaining up front
			nxt = rbuf;
			remain += read( fd, nxt + remain, sizeof( rbuf ) - remain );
		}

		if( remain < 20 ) {					// truncated or a record > rbuf
			fprintf( stderr, "abort: @header check, truncated file, or record > read buffer size\n" );
			exit( 1 );
		}
	
		if( strncmp( nxt, "@RDC", 4 ) == 0 ) {
			mtype = atoi( nxt+4 );
			mlen = atoi( nxt+12 );
			nxt += 20;
			remain -= 20;

			if( remain < mlen ) {				// not enough stuff
				memcpy( rbuf, nxt, remain );	// copy remaining up front
				nxt = rbuf;
				remain += read( fd, nxt + remain, sizeof( rbuf ) - remain );
			}

			if( remain < mlen ) {		// truncated or a record > rbuf
				fprintf( stderr, "abort: truncated file, or record > read buffer size\n" );
				exit( 1 );
			}
			
			if( desired == 0 ) {				// just dump mtypes
				captured++;
				fprintf( stdout, "%d\n", mtype );
			} else {

				if( mtype == desired ) {
					wlen += mlen;
					write( wfd, nxt, mlen );
					captured++;
				}
			}

			nxt += mlen;
			remain -= mlen;
		} else {
			fprintf( stderr, "didn't find rdc header!?! @ %ld\n", (long) (nxt - rbuf) );
			exit( 1 );
		}
	}

	fprintf( stderr, "done, captured %d messages (%d bytes)\n", captured, wlen );
	close( fd );
}

