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
    Mnemonic:	tools_test.c
    Abstract:	Unit test for tools

    Date:       19 June 2020
    Author:     E. Scott Daniels
*/


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "tools.h"

#include "ut_support.cpp"

int main( int argc, char** argv ) {
	char*	p;					// buffer pointer
	char	msg[2048];
	int		i = 41;
	int		j = 86;
	char*	t = (char *) "munchkin is a buttmunch!";
	int		errors = 0;
	long long		now_ms;
	long long		now_s;

	set_test_name( "tools_test" );

	mt_snprintf( msg, sizeof( msg ), (char *) "i=%d j=%d %q", i, j, t );
	fprintf( stderr, "got: (%s)\n", msg );
	errors += fail_if( strcmp( "i=41 j=86 \"munchkin is a buttmunch!\"", msg ) != 0, "quoted string was bad" );

	mt_snprintf( msg, sizeof( msg ), (char *) "i=%d j=%d %%q", i, j );
	fprintf( stderr, "<INFO> got: (%s)\n", msg );
	errors += fail_if( strcmp( "i=41 j=86 %q", msg ) != 0, "string with %%q wasn't handled correctly" );


	// ---------- time tests -------------------------
	now_s = time( NULL );
	now_ms = mt_now();
	fprintf( stderr, "<INFO> now_s=%lld  now_ms=%lld\n", now_s, now_ms );
	errors += fail_if( now_s != now_ms/1000000, "time values don't match up" );

	// ---------- logging tests ----------------------

	fprintf( stderr, "log messages expected to follow on stderr\n" );
	mt_log_set_level( MT_LOG_INFO );

	mt_log( MT_LOG_INFO, (char *) "<UT>  test message\n" );
	mt_log( MT_LOG_DEBUG, (char *) "<UT>  test message\n" );

	mt_log_set_hr( 1 );					// readable log messages
	mt_log( MT_LOG_INFO, (char *) "<UT>  test message\n" );

	mt_log_set_level( MT_LOG_DEBUG );
	mt_log( MT_LOG_DEBUG, (char *) "<UT>  test message\n" );

	mt_log_set_level( MT_LOG_CRIT );
	mt_log( MT_LOG_CRIT, (char *) "<UT>  test message\n" );

	mt_log_set_level( MT_LOG_ERR );
	mt_log( MT_LOG_ERR, (char *) "<UT>  test message\n" );

	mt_log_set_level( MT_LOG_WARN );
	mt_log( MT_LOG_WARN, (char *) "<UT>  test message\n" );

	mt_log_target( (char *) "/dev/null" );
	mt_log( MT_LOG_INFO, (char *) "<UT>  test message\n" );
	mt_log( MT_LOG_DEBUG, (char *) "<UT>  test message\n" );

	mt_log_target( (char *) "stderr" );

	// ------------ string based log level setting ----------------
	fprintf( stderr, "<INFO> testing debug level\n" );
	mt_log_str2level( "debug" );
	mt_log( MT_LOG_DEBUG, (char *) "<UT>  test message (debug)\n" );
	mt_log( MT_LOG_INFO, (char *) "<UT>  test message (info)\n" );
	mt_log( MT_LOG_WARN, (char *) "<UT>  test message (warn)\n" );
	mt_log( MT_LOG_ERR, (char *) "<UT>  test message (err)\n" );
	mt_log( MT_LOG_CRIT, (char *) "<UT>  test message (crit)\n" );

	fprintf( stderr, "<INFO> testing crit level\n" );
	mt_log_str2level( "crit" );
	mt_log( MT_LOG_CRIT, (char *) "<UT>  test message (crit)\n" );
	mt_log( MT_LOG_ERR, (char *) "<UT>  might see this test message (err)\n" );	// crit/err same in ric log lib
	mt_log( MT_LOG_DEBUG, (char *) "<UT>  should not see this test message (debug)\n" );
	mt_log( MT_LOG_INFO, (char *) "<UT>  should not see this test message (info)\n" );
	mt_log( MT_LOG_WARN, (char *) "<UT>  should not see this test message (warn)\n" );

	fprintf( stderr, "<INFO> testing error level\n" );
	mt_log_str2level( "error" );
	mt_log( MT_LOG_ERR, (char *) "<UT>  test message (err)\n" );
	mt_log( MT_LOG_CRIT, (char *) "<UT>  test message (crit)\n" );
	mt_log( MT_LOG_DEBUG, (char *) "<UT>  should not see this test message (debug)\n" );
	mt_log( MT_LOG_INFO, (char *) "<UT>  should not see this test message (info)\n" );
	mt_log( MT_LOG_WARN, (char *) "<UT>  should not see this test message (warn)\n" );

	fprintf( stderr, "<INFO> testing warning level\n" );
	mt_log_str2level( "warn" );
	mt_log( MT_LOG_WARN, (char *) "<UT>  test message (warn)\n" );
	mt_log( MT_LOG_DEBUG, (char *) "<UT>  should not see this test message (debug)\n" );
	mt_log( MT_LOG_INFO, (char *) "<UT>  should not see this test message (info)\n" );
	mt_log( MT_LOG_ERR, (char *) "<UT>  test message (err)\n" );
	mt_log( MT_LOG_CRIT, (char *) "<UT>  test message (crit)\n" );

	fprintf( stderr, "<INFO> testing default level\n" );
	mt_log_str2level( "default" );
	mt_log( MT_LOG_WARN, (char *) "<UT>  test message (warn)\n" );
	mt_log( MT_LOG_DEBUG, (char *) "<UT>  should not see this test message (debug)\n" );
	mt_log( MT_LOG_INFO, (char *) "<UT>  should not see this test message (info)\n" );
	mt_log( MT_LOG_ERR, (char *) "<UT>  test message (err)\n" );
	mt_log( MT_LOG_CRIT, (char *) "<UT>  test message (crit)\n" );

	// ---- json building stuff ---------------------------------
	msg[0] = 0;
	p = json_start_obj(  msg, sizeof( msg ),  (char *) "temp_measurement" );
	errors += fail_if( p == NULL, "addition of string to json did not return pointer" );

	p = json_add_string(  msg, sizeof( msg ),  (char *) "units", (char *) "celsius", MT_FL_OPEN );
	errors += fail_if( p == NULL, "addition of string to json did not return pointer" );

	p = json_add_int(  msg, sizeof( msg ),  (char *) "current", 20, 0 );
	errors += fail_if( p == NULL, "addition of int to json did not return pointer" );

	p = json_add_ll(  msg, sizeof( msg ),  (char *) "current", 20, 0 );
	errors += fail_if( p == NULL, "addition of ll to json did not return pointer" );

	p = json_add_double(  msg, sizeof( msg ),  (char *) "factor", (double) 1.8, 0 );
	errors += fail_if( p == NULL, "addition of double to json did not return pointer" );

	p = json_add_bool(  msg, sizeof( msg ),  (char *) "stp", 0, 0 );
	errors += fail_if( p == NULL, "addition of bool (false) to json did not return pointer" );

	p = json_add_bool(  msg, sizeof( msg ),  (char *) "label", 1, MT_NO_FLAGS );
	errors += fail_if( p == NULL, "addition of bool (true) to json did not return pointer" );

	p = json_add_nil(  msg, sizeof( msg ),  (char *) "label", MT_FL_CLOSE | MT_FL_LAST );
	errors += fail_if( p == NULL, "addition of bool (true) to json did not return pointer" );


	fprintf( stderr, "<INFO> %s\n", p );

	json_add_bool(  msg, 1,  (char *) "label", 1, MT_FL_CLOSE );		// should force error checking to run


	// these drive for coverage of open/close which all arn't hit in the above test
	msg[0] = 0;
	p = json_add_string(  msg, sizeof( msg ),  (char *) "units", (char *) "celsius", MT_FL_OPEN | MT_FL_CLOSE );
	errors += fail_if( p == NULL, "addition of string to json did not return pointer" );
	fprintf( stderr, "<INFO> %s\n", p );

	msg[0] = 0;
	p = json_add_int(  msg, sizeof( msg ),  (char *) "current", 20, MT_FL_ARRAY | MT_FL_OPEN | MT_FL_CLOSE );
	errors += fail_if( p == NULL, "addition of string to json did not return pointer" );
	fprintf( stderr, "<INFO> %s\n", p );

	msg[0] = 0;
	p = json_add_double(  msg, sizeof( msg ),  (char *) "factor", (double) 1.8, MT_FL_OPEN | MT_FL_CLOSE );
	errors += fail_if( p == NULL, "addition of string to json did not return pointer" );
	fprintf( stderr, "<INFO> %s\n", p );

	msg[0] = 0;
	p = json_add_double(  msg, sizeof( msg ),  (char *) "offset", (double) 32.0, MT_FL_OPEN | MT_FL_CLOSE );
	errors += fail_if( p == NULL, "addition of string to json did not return pointer" );
	fprintf( stderr, "<INFO> %s\n", p );

	msg[0] = 0;
	p = json_add_bool(  msg, sizeof( msg ),  (char *) "stp", 0, MT_FL_OPEN | MT_FL_CLOSE );
	errors += fail_if( p == NULL, "addition of string to json did not return pointer" );
	fprintf( stderr, "<INFO> %s\n", p );

	msg[0] = 0;
	p = json_add_bool(  msg, sizeof( msg ),  (char *) "label", 1, MT_FL_OPEN | MT_FL_CLOSE );
	errors += fail_if( p == NULL, "addition of string to json did not return pointer" );
	fprintf( stderr, "<INFO> %s\n", p );

	// ------ debugging funcitons -------------------------------
	dump( p, strlen( p ) );

	// ---- test usage/ensure thes must be LAST -----------------

	fprintf( stderr, "<INFO> tools_test: usage message expected to follow this message\n" );
	usage( argv[0] );

	ensure_nxt_arg( 10, 1, 1 );	// drive for coverage; it returns on success

	// this test MUST be last because it should not return and will exit with the correct good/bad exit
	// code based on current errors.
	fprintf( stderr, "<INFO> tools_test: argument error expected to follow this message\n" );
	ensure_nxt_arg( argc, 1, !!errors );

	return 1;			// if next arg returned that is an error!
}

