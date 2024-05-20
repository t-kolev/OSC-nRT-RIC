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
    Mnemonic:	context.h
    Abstract:	This class provides an operational context which includes
				the message router reference, rthing symtab and other goodies
				which are needed.

    Date:       18 June 2020
    Author:     E. Scott Daniels
*/


#ifndef _CONTEXT_H
#define _CONTEXT_H


#include "ricxfcpp/xapp.hpp"
/*
#include "ricxfcpp/message.hpp"				// framework stuff
#include "ricxfcpp/msg_component.hpp"
#include "ricxfcpp/callback.hpp"
*/

#include "symtab.h"
#include "rthing.h"
#include "ves_msg.h"

namespace munchkin {

// ---------------------------------------------------------------------------------

class Context {
	private:
		std::shared_ptr<Xapp>	x;				// the xapp framework
		std::shared_ptr<Symtab>	rthings;		// hash maps source to an rthing object
		std::shared_ptr<Ves_sender>	vsender;	// curl sender

	public:
		Context( std::string listen_port, bool wait4ready );							// construction without ves
		Context( std::string listen_port, bool wait4ready, std::string& ves_url );		// construction
		~Context();								// destruction

		Context( const Context& soi );				// copy
		Context& operator=( const Context& soi );	// copy operator
		Context( Context&& soi );					// move
		Context& operator=( Context&& soi );		// move operator

		std::shared_ptr<Rthing> Find_rt( std::string id );
		std::shared_ptr<Ves_sender> Get_sender( );
		void Register_cb( int mtype, xapp::user_callback cb_function );
		void Run( int nthreads );
};

} // namespace

#endif

