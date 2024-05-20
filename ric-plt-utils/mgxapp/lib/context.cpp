// vi: ts=4 sw=4 noet :
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
    Mnemonic:	context.cpp
    Abstract:	Manages a running context for the munchkin.

    Date:       18 June 2020
    Author:     E. Scott Daniels
*/

#include <string.h>

#include <cstdlib>
#include <string>

#include "symtab.h"
#include "rthing.h"
#include "context.h"
#include "ves_msg.h"

namespace munchkin {

// --------------- private ------------------------------------------------


// --------------- builders/operators  -------------------------------------

/*
	Create a new context.

	We use the awful initialisation list syntax to avoid sonar grumblings.
	(C++ could have gotten this right and made it easire to read; sigh)
*/
munchkin::Context::Context( std::string listen_port, bool wait4ready, std::string& ves_url ) :
		x( std::shared_ptr<Xapp>( new Xapp( listen_port.c_str(), wait4ready ) ) ),
		rthings( std::shared_ptr<Symtab>( new Symtab( 2048 ) ) ),
		vsender( std::shared_ptr<Ves_sender>(new Ves_sender( ves_url ) ) )
	{ /* empty body */ }

munchkin::Context::Context( std::string listen_port, bool wait4ready ) :
		x( std::shared_ptr<Xapp>( new Xapp( listen_port.c_str(), wait4ready ) ) ),
		rthings( std::shared_ptr<Symtab>( new Symtab( 2048 ) ) ),
		vsender( std::shared_ptr<Ves_sender>( new Ves_sender( "localhost:29080" ) ) )
	{ /* empty body */ }

/*
	Copy builder.  Given a source object instance (soi), create a copy.
*/
munchkin::Context::Context( const Context& soi ) {
	x = soi.x;					// we have only one framework regardless of context
	rthings = soi.rthings;		// all copies of the context reference the same symtab
	vsender = soi.vsender;
}

/*
	Assignment operator. Simiolar to the copycat, but "this" object exists and
	may have data that needs to be released prior to making a copy of the soi.
*/
Context& munchkin::Context::operator=( const Context& soi ) {
	if( this != &soi ) {				// cannot do self assignment
		if( rthings != NULL ) {			// drop/free any data first
			rthings = NULL;
		}

		x = soi.x;							// xapp framework is referenced by all
		rthings = soi.rthings;
		vsender = soi.vsender;
	}

	return *this;
}

/*
	Move builder.  Given a source object instance (soi), move the information from
	the soi ensuring that the destriction of the soi doesn't trash things from
	under us.
*/
munchkin::Context::Context( Context&& soi ) {
	x = soi.x;
	rthings = soi.rthings;
	vsender = soi.vsender;

	soi.rthings = NULL;		// remove ref so that it's not destroyed after the move (do NOT free)
	soi.vsender = NULL;
}

/*
	Move Assignment operator. Move the message data to the existing object
	ensure the object reference is cleaned up, and ensuring that the source
	object references are removed.
*/
Context& munchkin::Context::operator=( Context&& soi ) {
	if( this != &soi ) {				// cannot do self assignment
		rthings = NULL;					// force drop
		vsender = NULL;

		x = soi.x;
		rthings = soi.rthings;
		vsender = soi.vsender;

		soi.rthings = NULL;		// remove ref so that it's not destroyed after the move (do NOT free)
		soi.vsender = NULL;
	}

	return *this;
}

/*
	Destroyer.
*/
munchkin::Context::~Context() {
	// nothing needed at the moment
}

// ----------- real work functions -----------------------------------------------------

/*
	Look up the id in the symbol table and return the remote thing we have. If
	it's not there, then create one and return that.
*/
std::shared_ptr<Rthing> munchkin::Context::Find_rt( std::string id ) {
	std::shared_ptr<Rthing> rt;		// real pointer should we need it
	std::shared_ptr<void> vt;		// hash returns a pointer to void

	vt = rthings->Extract( id, 1 );
	if( vt != NULL ) {
		return std::static_pointer_cast<Rthing>( vt );
	}

	rt = std::shared_ptr<Rthing>( new Rthing( id ) );
	rthings->Implant( id, 1, rt );

	return rt;
}

/*
	Return a shared pointer to the ves sender object.
*/
std::shared_ptr<munchkin::Ves_sender> munchkin::Context::Get_sender( ) {
	return vsender;
}

/*
	Register a callback handler for a message type.
	Simple passthrough at the moment.
*/
void munchkin::Context::Register_cb( int mtype, xapp::user_callback cb_function ) {
	if( mtype < 0 ) {
		mtype = x->DEFAULT_CALLBACK;
	}

	x->Add_msg_cb( mtype, cb_function, this );					// register the callback for mtype
}

/*
	Wrapper for the framework driver.
*/
void munchkin::Context::Run( int nthreads ) {
	x->Run( nthreads );
}

} // namespace
