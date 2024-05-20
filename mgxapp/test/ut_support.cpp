// vi: ts=4 sw=4 noet:
/*
==================================================================================
    Copyright (c) 2020 AT&T Intellectual Property.
    Copyright (c) 2020 Nokia

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
==================================================================================
*/

/*
    Mnemonic:	ut_support.c
    Abstract:	Tools to make unit testing easier.

    Date:       16 June 2020
    Author:     E. Scott Daniels
*/

#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#include <string>

static std::string test_name = "unknown";

/*
	Set the name of the current tester
*/
extern void set_test_name( std::string name ) {
	test_name = name;
}

/*
	Returns 1 if the condition is true (not zero)
*/
extern int fail_if( int cond, std::string reason ) {
	if( cond ) {
		fprintf( stderr, "<FAIL> %s: %s\n", test_name.c_str(), reason.c_str() );
		return 1;
	}

	return 0;
}

/*
	Returns 1 if the condition is false.
*/
extern int fail_if_false( int cond, std::string reason ) {
	if( !cond ) {
		fprintf( stderr, "<FAIL> %s: %s\n", test_name.c_str(), reason.c_str() );
		return 1;
	}

	return 0;
}


/*
	Simple success/fail announcement
*/
void announce_results( int errors ) {
	if( errors ) {
		fprintf( stderr, "<FAIL> %s had errors\n", test_name.c_str() );
	} else {
		fprintf( stderr, "<PASS> %s successful\n", test_name.c_str() );
	}
}

/*
    Very simple file reader. Reads up to 8k into a single buffer and
    returns the buffer as char*.  Easier to put json test things in
    a file than strings.
*/
static char* read_cbuf( std::string fname ) {
	char*   rbuf;
	int fd;
	int len;

	rbuf = (char *) malloc( sizeof( char ) * 8192 );
	fd = open( fname.c_str(), O_RDONLY, 0 );
	if( fd < 0  ) {
		fprintf( stderr, "<ABORT> can't open test file: %s: %s\n", fname.c_str(), strerror( errno ) );
		exit( 1 );
	}

	len = read( fd, rbuf, 8190 );
	if( len < 0  ) {
		close( fd );
		fprintf( stderr, "<ABORT> read from file failed: %s: %s\n", fname.c_str(), strerror( errno ) );
		exit( 1 );
	}

	rbuf[len] = 0;
	close( fd );

	return rbuf;
}

