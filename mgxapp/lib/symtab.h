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
    Mnemonic:	symtab.h
    Abstract:	A wrapper ontop of RMRs symbol table (hash map, dict or whatever
				you think it should be called).

				We need to wrap it in order to provide shared pointers to the things
				that are referenced by the symtab, and to ensure that as long as
				the thing hasn't been removed form the table it doesn't go away.

    Date:       18 June 2020
    Author:     E. Scott Daniels
*/


#ifndef _SYMTAB_H
#define _SYMTAB_H


#include <cstdlib>
#include <string>
#include <memory>

namespace munchkin {

/*
	Hash table object.
*/
typedef struct hto {
	std::shared_ptr<void> real_obj;		// the real object the user hung off of the table
} hto_t;

class Symtab {
	private:
		void*	st;				// the symbol table "context"
		int		space = 1;		// provide a 'next space' function to get an unused space in the table

		Symtab( const Symtab& soi );		// copy is prohibited so declare these as private
		Symtab& operator=( const Symtab& soi );

	public:
		Symtab( int size );			// construction
		~Symtab();					// destruction

		Symtab( Symtab&& soi );					// moves of the table are allowed
		Symtab& operator=( Symtab&& soi );

		// ---- real work functions --------------------------------------------------
		void Implant( std::string name, int space, std::shared_ptr<void> data );
		std::shared_ptr<void> Extract( std::string name, int space );
};

} //namespace
#endif

