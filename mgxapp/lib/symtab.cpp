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
    Mnemonic:	symtab.cpp
    Abstract:	Wrap the RMR symbol table to that we can easily accept and push
				back smart pointers.  This is a very LIGHT wrapper supporting only
				implant and extract operations. Nice things like being able to delete
				and walk the full map aren't supported.

				Because the symbol table references an underlying C struct and
				related colleciton of pointers to things, it is difficult at best
				to copy the beast. The symtab thus cannot be copied and compile time
				errors will happen if tried.

    Date:       18 June 2020
    Author:     E. Scott Daniels
*/

#include <string.h>			// memset needs
#include <cstdlib>
#include <string>

#include <rmr/rmr_symtab.h>
#include "symtab.h"

namespace  munchkin {

// --- construction, destruction, copy/move --------------------------------------------


Symtab::Symtab( int size ) :
	st( rmr_sym_alloc( size ) ),
	space( 1 )
{ /* empty body */ }

Symtab::~Symtab( ) {
	if( st != NULL ) {
		fprintf( stderr, "<symtab> freeing real table %p\n", st );
		rmr_sym_free( st );
		st = NULL;
		fprintf( stderr, "<symtab> freefinished\n" );
	}
}


// ---- do NOT implement copy funcions -------------------------

/*
	Move builder.  Given a source object instance (soi), move the information from
	the soi ensuring that the destriction of the soi doesn't trash things from
	under us.
*/
Symtab::Symtab( Symtab&& soi ) {
	st = soi.st;
	space = soi.space;

	soi.st = NULL;		// remove ref so that it's not destroyed after the move (do NOT free)
}

/*
	Move Assignment operator. Move the message data to the existing object
	ensure the object reference is cleaned up, and ensuring that the source
	object references are removed.
*/
Symtab& Symtab::operator=( Symtab&& soi ) {
	if( this != &soi ) {				// cannot do self assignment

		// if ever we have something that must be delteded/freed, do it here

		st = soi.st;
		space = soi.space;

		soi.st = NULL;
	}

	return *this;
}

// ------- real work functions --------------------------------------------------------

void Symtab::Implant( std::string name, int space, std::shared_ptr<void> data ) {
	hto_t* hto;				// the hash table object

	if( st != NULL ) {
		hto = (hto_t *) malloc( sizeof( *hto ) );			// raw pointer to give rmr_sym
		memset( hto, 0, sizeof( *hto ) );					// clear to keep valgrind from complaining
		hto->real_obj = data;
		rmr_sym_put( st, name.c_str(), space, hto );
	}
}

std::shared_ptr<void> Symtab::Extract( std::string name, int space ) {
	hto_t* hto = NULL;				// the hash table object

	hto = (hto_t *) rmr_sym_get( st, name.c_str(), space );
	if( hto == NULL ) {
		return NULL;
	}

	return hto->real_obj;
}


} // namespace
