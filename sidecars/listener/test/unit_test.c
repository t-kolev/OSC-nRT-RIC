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
	Mnemonic:	unit_test.c
	Abstract:	Basic unit tests for the mc listener.
	Date:		22 August 2019
	Author:		E. Scott Daniels
*/

#define FOREVER 0			// allows forever loops in mcl code to escape after one loop

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

#define TEST_MTYPE	1000		// message type for testing
#include "test_rmr_em.c"		// emulated rmr functions (for receives)

// this/these are what we are testing; include them directly (must be after forever def)
#include "../src/mcl.c"
#include "../src/rdc.c"

/*
	Set up env things for the rdc setup call.
*/
static void set_env() {
	setenv( "MCL_RDC_ENABLE", "1", 1 );									// cause 'raw' capture to be setup
	setenv( "MCL_RDC_STAGE", "/tmp/mc_listener_test/stage", 1 );		// unit test script should create and will purge after
	setenv( "MCL_RDC_FINAL", "/tmp/mc_listener_test/final", 1 );
	setenv( "MCL_RDC_SUFFIX", ".xxx", 1 );
	setenv( "MCL_RDC_DONE", ".done", 1 );
	setenv( "MCL_RDC_FREQ", "10", 1 );
}

/*
	Parms:	[fifo-dir-name]
*/
int main( int argc,  char** argv ) {
	void*	ctx;
	void*	bad_ctx;				// context with a bad directory path for coverage/error testing
	int		errors = 0;
	char*	dname = "/tmp/fifos";
	char*	port = "4560";
	int		fd;
	int		fd2;
	int		rfd;					// a reader file des so we can read what we write
	fifo_t*	fref = NULL;			// fifo type reference; we just verify that suss sets it
	char	wbuf[2048];
	char	payload[1024];
	char*	bp;
	void*	buf;
	int state;
	char	timestamp[1024];		// read will put a timestamp here

	if( argc > 1 ) {
		dname = argv[1];
	}

	setenv( "MCL_RDC_ENABLE", "0", 1 );				/// test disabled mode for coverage
	setup_rdc( );

	set_env();							// set env that setup_rdc() looks for

	ctx = mcl_mk_context( dname );			// allocate the context
	if( ctx == NULL ) {
		fprintf( stderr, "[FAIL] couldn't make context\n" );
		exit( 1 );
	}

	mcl_set_sigh();										// prevent colobber from broken pipe

	open_fifo( ctx, TEST_MTYPE, WRITER );						// open dummy to prevent blocking reader
	rfd = open_fifo( ctx, TEST_MTYPE, READER );				// open a reader to check fanout output
	if( rfd < 0 ) {
		fprintf( stderr, "[FAIL] unable to open a pipe reader for type == 100\n" );
		errors++;
	}

	fd = suss_fifo( ctx, TEST_MTYPE, WRITER, &fref );			// should open the file for writing and return the fdes
	if( fd < 0 ) {
		fprintf( stderr, "[FAIL] suss_fifo did not return a valid fd\n" );
		errors++;
	}

	if( fref == NULL ) {
		fprintf( stderr, "[FAIL] suss_fifo did not set the fifo reference pointer\n" );
		errors++;
	} else {
		chalk_ok( fref );
		chalk_error( fref );
	}

	fd2= suss_fifo( ctx, TEST_MTYPE, 0, NULL );				// should open the file file for reading and return a different fd
	if( fd < 0 ) {
		fprintf( stderr, "[FAIL] suss_fifo did not return a valid fd\n" );
		errors++;
	}
	if( fd == fd2 ) {
		fprintf( stderr, "[FAIL] reading and writing fifo file descriptors expected to differ; both were %d\n", fd );
		errors++;
	}

	close_fifo( ctx, TEST_MTYPE, WRITER );			// close one we know is there
	close_fifo( ctx, 84306, WRITER );				// coverage on error case

	mcl_start_listening( ctx, port, 0 );			// start the listener

	// under test, the FOREVER = 0 keeps fanout from blocking; drive several times to cover all cases
	mcl_fifo_fanout( ctx, 5, 1 );					// first rmr receive call will simulate a timeout
	mcl_fifo_fanout( ctx, 5, 1 );					// second receive simulates a health check
	mcl_fifo_fanout( ctx, 5, 1 );					// 3-n return alternating timeout messages; drive so that
	mcl_fifo_fanout( ctx, 5, 1 );					// we will have several land in the FIFO
	mcl_fifo_fanout( ctx, 5, 1 );
	mcl_fifo_fanout( ctx, 5, 1 );
	mcl_fifo_fanout( ctx, 5, 1 );

	*timestamp = 0;
	state = mcl_fifo_read1( ctx, TEST_MTYPE, payload, sizeof( payload ), TRUE );
	if( state < 1 ) {
		fprintf( stderr, "[FAIL] fifo_read return positive value when expected to\n" );
		errors++;
	}
	state = mcl_fifo_tsread1( ctx, TEST_MTYPE, payload, sizeof( payload ), TRUE, timestamp );
	if( state < 1 ) {
		fprintf( stderr, "[FAIL] fifo_read with timestamp return positive value when expected to\n" );
		errors++;
	}

	state = fifo_read1( NULL, TEST_MTYPE, payload, sizeof( payload ), 1, timestamp );		// coverage error check
	if( state != 0 ) {
		fprintf( stderr, "[FAIL] fifo_read didn't return 0 when given a nil context to\n" );
		errors++;
	}

	mcl_fifo_fanout( ctx, 5, 0 );					// test with writing short header
	mcl_fifo_fanout( ctx, 5, 0 );

	// ------ some error/coverage testing ---------------------------
	logit( LOG_CRIT, "critical message" );
	logit( LOG_ERR, "error message" );
	logit( LOG_WARN, "warning message" );
	logit( LOG_STAT, "stats message" );

	fprintf( stderr, "[INFO] expected create fail message should follow\n" );
	bad_ctx = mcl_mk_context( "/nosuchdirectoryinthesystem" );		// create a context where fifo opens should fail
	if( bad_ctx == NULL ) {
		fprintf( stderr, "[FAIL] couldn't make 'bad' context" );
		exit( 1 );
	}

	fref = NULL;
	fd = suss_fifo( bad_ctx, TEST_MTYPE, 1, &fref );				// should fail to open the file for writing beacuse directory is bad
	if( fd >= 0 ) {
		fprintf( stderr, "[FAIL] suss_fifo returned a valid fd when given a context with a bad directory path\n" );
		errors++;
	}
	if( fref != NULL ) {
		fprintf( stderr, "[FAIL] suss_fifo returned an fref pointer when given a bad context\n" );
		errors++;
	}

	fd = suss_fifo( NULL, TEST_MTYPE, 1, &fref );				// coverage nil pointer check
	if( fd >= 0 ) {
		fprintf( stderr, "[FAIL] suss_fifo returned a valid fd when given a nil context a bad directory path\n" );
		errors++;
	}

	fd = suss_fifo( ctx, -1, 1, &fref );				// mad message type check
	if( fd >= 0 ) {
		fprintf( stderr, "[FAIL] suss_fifo returned a valid fd when given a bad message type\n" );
		errors++;
	}

	// -- buffer testing ------------------------------------------------------
	bp = build_hdr( 1024, wbuf, 0 );
	bp = build_hdr( 1024, NULL, 0 );
	if( bp == NULL ) {
		fprintf( stderr, "[FAIL] build_hdr didn't return a buffer pointer when given a nil buffer\n" );
		errors++;
	}
	free( bp );

	bp = build_hdr( 1024, wbuf, sizeof( wbuf ) );
	if( bp == NULL ) {
		fprintf( stderr, "[FAIL] build_hdr didn't return a buffer pointer\n" );
		errors++;
	}


	// ----- msg receive testing ----------------------------------------------------
	buf = mcl_get_msg( NULL, NULL, 1 );			// drive nil pointer checks
	if( buf != NULL ) {
		errors++;
		fprintf( stderr, "[FAIL], get_msg call with nil context returned a buffer pointer\n" );
	}

	buf = mcl_get_msg( ctx, NULL, 1 );			// drive to force coverage; nothing is sent, so we can't validate buffer


	mcl_fifo_one( NULL, NULL, 1, 1 );
	mcl_fifo_one( ctx, NULL, 1, 1 );
	mcl_fifo_one( ctx, wbuf, 0, 1 );
	mcl_fifo_one( ctx, wbuf, 10, 100 );


	// --- some rdc testing as best as we can without message generators --------
	rdc_init( NULL, NULL, ".foo", ".bar" );	// coverage testing

	ctx = setup_rdc();						// coverage test to ensure that it generates a context
	if( ctx == NULL ) {
		fprintf( stderr, "[FAIL] setup_rdc did not return a context pointer\n" );
		errors++;
	}

	rdc_set_freq( NULL, 0 );					// error/nil test
	rdc_set_freq( ctx, 0 );					// error/nil test
	rdc_set_freq( ctx, 10 );				// roll after 10seconds to test that

	build_hdr( 1024, wbuf, sizeof( wbuf ) );
	bp = NULL;
	bp = rdc_init_buf( TEST_MTYPE, wbuf, 10, bp );					// set up for write
	rdc_write( ctx, bp, payload, sizeof( payload ) );				// write the raw data

	fprintf( stderr, "[INFO] pausing to test rdc file rolling\n" );
	sleep( 15 );
	build_hdr( 1024, wbuf, sizeof( wbuf ) );
	bp = NULL;
	bp = rdc_init_buf( TEST_MTYPE, wbuf, 10, bp );
	rdc_write( ctx, bp, payload, sizeof( payload ) );


	// CAUTION:  filenames need to match those expected in the run script as it creates src, and will validate, destination files
	state = copy_unlink( "/tmp/mc_listener_test/no-such-copy_src", "/tmp/mc_listener_test/copy_dest", 0664 );  // first couple drive for error and coverage
	if( state >= 0 ) {
		fprintf( stderr, "[FAIL] copy-unlink of bad file didn't return bad state\n" );
		errors++;
	}
	state = copy_unlink( "/tmp/mc_listener_test/copy_src", "/tmp/mc_listener_test-nodir/copy_dest", 0664 );
	if( state >= 0 ) {
		fprintf( stderr, "[FAIL] copy-unlink of bad target didn't return bad state\n" );
		errors++;
	}
	state = copy_unlink( "/tmp/mc_listener_test/copy_src", "/tmp/mc_listener_test/copy_dest", 0664 );  // drive for coverage; setup script can check contents
	if( state < 0 ) {
		fprintf( stderr, "[FAIL] copy-unlink expected success but failed\n" );
		errors++;
	}
	state = mvocp( "/tmp/mc_listener_test/bad-src-mv_src", "/tmp/mc_listener_test/mv_dest" );
	if( state >= 0 ) {
		fprintf( stderr, "[FAIL] mv or copy expected failure didn't set bad state\n" );
		errors++;
	}
	state = mvocp( "/tmp/mc_listener_test/mv_src", "/tmp/mc_listener_test/mv_dest" );
	if( state < 0 ) {
		fprintf( stderr, "[FAIL] mv or copy expected to succeed  didn't set good state\n" );
		errors++;
	}


	// ---- finally, check error count, write nice cheerful message and exit ----
	if( ! errors ) {
		fprintf( stderr, "[PASS] unit_test: everything looks peachy\n" );
	} else {
		fprintf( stderr, "[FAIL] unit_test: there were %d errors\n", errors );
	}

	return errors != 0;
}

