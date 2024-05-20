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
    Mnemonic:	symtab_test.cpp
    Abstract:	Unit test for symtab.

    Date:       19 June 2020
    Author:     E. Scott Daniels
*/


#include <string>

#include "symtab.h"
#include "symtab.cpp"

#include "ut_support.cpp"

/*
	Simple class to push into the hash
*/
class Dummy {
	public:
		int v1;
		int v2;

		Dummy( int a, int b ) : v1(a), v2(b) {}
};

int main( int argc, char** argv ) {
	int		errors = 0;
	munchkin::Symtab*	st;				// the table
	munchkin::Symtab  mst( 1023 );	// move receiver
	Dummy*	d1;				// dummy objects to reference in the table
	Dummy*	d2;
	Dummy*	d3;
	std::shared_ptr<Dummy> sd;

	set_test_name( "symtab_test" );

	st = new munchkin::Symtab( 2048 );
	d1 = new Dummy( 430, 86 );
	d2 = new Dummy( 41, 42 );



	st->Implant( "dummy1", 1, std::shared_ptr<void>( d1 ) );
	st->Implant( "dummy2", 1, std::shared_ptr<void>( d2 ) );

	sd = std::static_pointer_cast<Dummy>( st->Extract( "dummy1", 1 ) );

	fprintf( stderr, "extracted dummy1:  %d %d\n", sd->v1, sd->v2 );
	sd->v1 += 1000;

	sd = std::static_pointer_cast<Dummy>( st->Extract( "dummy1", 1 ) );
	errors +=  fail_if( sd->v1 != 1430, "dummy1 value1 after increas and extract was wrong" );

	fprintf( stderr, "extraction after modification dummy1:  %d %d\n", sd->v1, sd->v2 );

	sd = std::static_pointer_cast<Dummy>( st->Extract( "dummy3", 1 ) );
	errors += fail_if( sd != NULL, "extraction of non-existant element didn't return nil" );

											// drive moves for coverage
	mst = std::move( *st );					// move assignment
	munchkin::Symtab mst2 = std::move( mst );			// move operator

	delete st;		// coverage of destructor

	return errors;
}

