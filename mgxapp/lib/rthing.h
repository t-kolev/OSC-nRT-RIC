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
    Mnemonic:	rthing.h
    Abstract:	Defines the rthing class. An rthing is a remote thing that is
				sending messages. We track certain things so as to auto fill
				common header bits if the thing doesn't supply them.

    Date:       18 June 2020
    Author:     E. Scott Daniels
*/


#ifndef _RTHING_H
#define _RTHING_H


#include <cstdlib>
#include <string>

namespace munchkin {

class Rthing {
	private:
		std::string	id = "";		// unique id; most likely the host:port or IP:port source address
		long long	created = 0;	// timestamp this thing was created
		long long	access_ts = 0;	// time of last access (message received)
		int			space = 1;		// space in the master symtab for tracking "personal belongings"
		int			mcount = 0;		// activity count
		int			ecount = 0;

	public:
		Rthing( std::string id );		// construction
		~Rthing();						// destruction

		Rthing( const Rthing& soi );				// copy
		Rthing& operator=( const Rthing& soi );		// copy operator
		Rthing( Rthing&& soi );						// move
		Rthing& operator=( Rthing&& soi );			// move operator

		int Clear_count();
		int Get_count();
		long long Get_created();
		std::string Get_eid();						// build an event id
		void Inc_count();
		void Inc_count( int amt );
		long long Set_access_ts( void );			// set the last accessed timestaemp and return delta
};


} // namespace
#endif
