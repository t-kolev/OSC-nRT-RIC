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
    Mnemonic:	ves_msg_testt.cpp
    Abstract:	Unit test for ves message support

    Date:       2 July 2020
    Author:     E. Scott Daniels
*/

#include <memory>

#include "ricxfcpp/jhash.hpp"

#include "rthing.h"

#include "ves_msg.h"			// include things under test
#include "ves_msg.cpp"

#include "ut_support.cpp"

int main( int argc, char** argv ) {
	std::shared_ptr<munchkin::Rthing>	rt1;
	int	errors = 0;
	std::string sjson;
	char*	jbuf;					// json read from a file
	std::shared_ptr<xapp::Jhash> jh = NULL;				// parsed json
	std::string	event;
	munchkin::Ves_sender* s;

	set_test_name( "ves_msg_test" );

	rt1 = std::shared_ptr<munchkin::Rthing>( new munchkin::Rthing( "foobar:43086" ) );

	sjson = munchkin::eh_build( rt1,  mt_now(), "reporter:xxx", "affected:yyy" );
	fprintf( stderr, "<INFO> header = (%s)\n", sjson.c_str() );

	jbuf = read_cbuf( "data/test_measure_1.dat" );
	if( jbuf == NULL ) {
		fprintf( stderr, "<FAIL> unable to read data/test_measure_1.dat\n" );
		announce_results( errors +1 );
		exit( 1 );
	}

	jh = std::shared_ptr<xapp::Jhash>( new xapp::Jhash( jbuf ) );
	sjson = munchkin::mf_build( jh, 120000000, true );
	fprintf( stderr, "<INFO> mf = (%s)\n", sjson.c_str() );


	event = munchkin::event_build( jh, rt1, 12345678900,  2345, "reporter", "affected" );
	fprintf( stderr, "\n<INFO> event = (%s)\n", event.c_str() );


	// test curl stuff
	s = new munchkin::Ves_sender( "http://127.0.0.1:29080" );
	s->Send_event( event );
	delete s;

	announce_results( errors );
	exit( !! errors );
}
