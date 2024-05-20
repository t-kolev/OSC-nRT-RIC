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
    Mnemonic:	rthing.cpp
    Abstract:	The meat of the rthing class.
				An rthing is used to track a remote thing that is sending us
				messages.

				future: might need/want to add locks round the count modifications,
				but for now we assume munchkin is single threaded.

    Date:       18 June 2020
    Author:     E. Scott Daniels
*/

#include "tools.h"

#include "rthing.h"

namespace munchkin {

// --------------- private ------------------------------------------------

static int rthing_nxt_space = 100;		// spaces > 99 are rthings

// --------------- builders/operators  -------------------------------------

/*
	Create a new rthing
*/
munchkin::Rthing::Rthing( std::string rt_id ) :
	id( rt_id ),
	created(  mt_now() )		// used in the event header as first observed
{ /* empty body */ }

/*
	Copy builder.  Given a source object instance (soi), create a copy.
*/
munchkin::Rthing::Rthing( const munchkin::Rthing& soi ) {
	id = soi.id;
	created = soi.created;
	space = soi.space;
	mcount = soi.mcount;
	ecount = soi.ecount;
}

/*
	Assignment operator. Simiolar to the copycat, but "this" object exists and
	may have data that needs to be released prior to making a copy of the soi.
*/
munchkin::Rthing& Rthing::operator=( const munchkin::Rthing& soi ) {
	if( this != &soi ) {				// cannot do self assignment
		// if rthing ever has data references drop/free them here

		id = soi.id;
		created = soi.created;
		space = soi.space;
		mcount = soi.mcount;
		ecount = soi.ecount;
	}

	return *this;
}

/*
	Move builder.  Given a source object instance (soi), move the information from
	the soi ensuring that the destriction of the soi doesn't trash things from
	under us.
*/
munchkin::Rthing::Rthing( munchkin::Rthing&& soi ) {
	id = soi.id;
	created = soi.created;
	space = soi.space;
	mcount = soi.mcount;
	ecount = soi.ecount;


	// if rthing ever references data, ensure the ref is dropped in the soi so
	// when it is destroyed it isn't knoked out from under the new object
}

/*
	Move Assignment operator. Move the message data to the existing object
	ensure the object reference is cleaned up, and ensuring that the source
	object references are removed.
*/
munchkin::Rthing& munchkin::Rthing::operator=( munchkin::Rthing&& soi ) {
	if( this != &soi ) {				// cannot do self assignment
		// same as copy -- free anything we might reference from 'this' first

		id = soi.id;
		created = soi.created;
		space = soi.space;
		mcount = soi.mcount;
		ecount = soi.ecount;

		// same as move -- deref any references here
	}

	return *this;
}

/*
	Destroyer.
*/
munchkin::Rthing::~Rthing() {
	// nothing needed at the moment
}

// -------------------- real work functions -----------------------------------

/*
	Reset the count.
*/
int munchkin::Rthing::Clear_count() {
	mcount = 0;
}

/*
	Suss out the current count.
*/
int munchkin::Rthing::Get_count() {
	return mcount;
}

/*
	Return the creation timestamp.
*/
long long munchkin::Rthing::Get_created() {
	return created;
}

/*
	Build an event ID.
*/
std::string munchkin::Rthing::Get_eid() {
	char wbuf[1024];

	snprintf( wbuf, sizeof( wbuf ), "eid_%s_%d", id.c_str(), ecount );
	ecount++;

	return std::string( wbuf );
}

/*
	Simple increase of count
*/
void munchkin::Rthing::Inc_count() {
	mcount++;
}

/*
	Increase by a given amount; use -1 for decrement.
*/
void munchkin::Rthing::Inc_count( int amt ) {
	mcount += amt;
}

/*
	Set the last accessed time value and return the delta from
	the last access.
*/
long long munchkin::Rthing::Set_access_ts( ) {
	long long then;

	then = access_ts;
	access_ts = mt_now();

	return access_ts - then;
}


} // namespace
