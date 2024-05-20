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
    Mnemonic:	rthing_test.cpp
    Abstract:	Unit test for rthing class.

    Date:       19 June 2020
    Author:     E. Scott Daniels
*/

#include "rthing.h"
#include "rthing.cpp"

#include "ut_support.cpp"

int main( int argc, char** argv ) {
	munchkin::Rthing	rt1( "foobar:43086" );
	munchkin::Rthing	rt2( "boobar:19029" );
	munchkin::Rthing*	rtp;
	std::string idstr;
	long long cdate;
	int	errors = 0;

	set_test_name( "rthing_test" );

    rt1 = std::move( rt2 );						// drives move operator= function
    munchkin::Rthing rt3 = std::move( rt1 );				// drives move constructor function

    rt1 = rt2;									// drives copy operator
    munchkin::Rthing rt4 = rt2;							// drives copy construction

	rt1.Inc_count();
	errors += fail_if( rt1.Get_count() != 1, "inc count seemed not to work" );

	rt1.Inc_count( 13 );
	errors += fail_if( rt1.Get_count() != 14, "inc count for amount seemed not to work" );

	rt1.Clear_count( );
	errors += fail_if( rt1.Get_count() != 0, "clear count seemed not to work" );

	idstr = rt1.Get_eid();
	fprintf( stderr, "<INFO> eid = (%s)\n", idstr.c_str() );
	errors += fail_if( idstr.compare( "eid_boobar:19029_0" ) != 0, "eid string didn't match expected value" );

	cdate = rt1.Get_created();
	errors += fail_if( cdate == 0, "creation date smells" );

	rt1.Set_access_ts();


	rtp = new munchkin::Rthing( "goober:1212" );
	delete rtp;


	announce_results( errors );
	exit( !! errors );
}
