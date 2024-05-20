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
    Mnemonic:	context_test.cpp
    Abstract:	Unit test for the context code

    Date:       19 June 2020
    Author:     E. Scott Daniels
*/

#include "ricxfcpp/message.hpp"		// must have for callback funciton
#include "ricxfcpp/xapp.hpp"

#include "ves_msg.h"
#include "rthing.h"

#include "context.h"		// things under test are included directly
#include "context.cpp"

#include "ut_support.cpp"

/*
	Function to pass to register callback; does nothing pratical in the test.
*/
extern void ctx_test_cb( xapp::Message& mbuf, int mtype, int subid, int len, xapp::Msg_component payload,  void* data ) {
	fprintf( stderr, "<INFO> test_cb: should never see this message\n" );
}

int main( int argc, char** argv ) {
	munchkin::Context	ctx1( "4560", false );		// create without waiting for rmr ready
	munchkin::Context	ctx2( "5560", false );
	munchkin::Context*	ctxp;
	std::shared_ptr<munchkin::Ves_sender>	sender;
	std::shared_ptr<munchkin::Rthing> rt;
	std::shared_ptr<munchkin::Rthing> rt2;
	std::string ves_target = "http://localhost:21989";
	int errors = 0;
	int	count;				// count set in the object
	int	socount;			// shared object use count

	set_test_name( "context_test" );

	ctx2 = ctx1;			// copy operator
	munchkin::Context ctx3 = ctx1;	// copy construction

	ctx2 = std::move( ctx3 );					// move assignment
	munchkin::Context ctx4 = std::move( ctx1 );			// move constructor

	rt = ctx2.Find_rt( "buttmunch:43086" );		// non-existant rt, should create
	if( fail_if( rt == NULL, "find rt didn't return a pointer" ) != 0 ) {
		announce_results( errors + 1 );
		return 1;
	}

	socount = rt.use_count();					// get current use
	rt->Inc_count();
	count = rt->Get_count();
	errors +=  fail_if( count != 1, "inc rt count failed" );

	rt2 = ctx2.Find_rt( "buttmunch:43086" );		// existing rt, should return pointer to same rt
	if( fail_if( rt2 == NULL, "find rt didn't return a pointer to existing rt" ) != 0 ) {
		announce_results( errors + 1 );
		return 1;
	}
	errors += fail_if( socount == rt.use_count(), "rt2  appears to reference the wrong object" );

	count = rt2->Get_count();
	errors +=  fail_if( count != 1, "rt2 count didnt match expected count" );

	rt->Inc_count( 5 );
	count = rt2->Get_count();
	errors +=  fail_if( count != 6, "get of rt2 count didnt match expected 6 set with rt" );


	ctx1.Register_cb( 203, ctx_test_cb );			// drive for coverage; nothing to check
	ctx1.Register_cb( -1, ctx_test_cb );			// register as default

	ctxp = new munchkin::Context( "43086", false, ves_target );		// drive alternate for coverage
	errors += fail_if( ctxp == NULL, "unable to create a context with ves url" );

	sender = ctxp->Get_sender();
	errors += fail_if( sender == NULL, "no sender pointer returned" );

	announce_results( errors );
	return !!errors;
}

