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
    Mnemonic:	ves_msg.h
    Abstract:	Header for the common event header functions

    Date:       16 June 2020
    Author:     E. Scott Daniels
*/

#ifndef _VES_MSG_H
#define  _VES_MSG_H

#include <cstdlib>
#include <string>
#include <memory>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>

#include "ricxfcpp/jhash.hpp"

#include "rthing.h"

namespace munchkin {

#define JT_VALUE	0		// type of the element in a jhash
#define JT_STRING	1
#define JT_BOOL		2
#define JT_NIL		4

#define VT_INT		0		// value types placed into the ves output
#define VT_DOUBLE	1

// event header field names
#define EH_OBJ_TAG	(char *) "commonEventHeader"
#define EH_DOMAIN	(char *) "domain"
#define EH_ID		(char *) "eventId"
#define EH_NAME		(char *) "eventName"
#define EH_TYPE		(char *) "eventType"
#define EH_IHF		(char *) "internalHeaderFields"
#define EH_ETIME	(char *) "lastEpochMicrosec"
#define EH_NFC_CODE	(char *) "nfcNamingCode"
#define EH_NF_CODE	(char *) "nfNamingCode"
#define EH_PRIORITY	(char *) "priority"
#define EH_RPTID	(char *) "reportingEntityId"
#define EH_RPTNM	(char *) "reportingEntityName"
#define EH_SEQ		(char *) "sequence"
#define EH_SRCID	(char *) "sourceId"
#define EH_SRCNM	(char *) "sourceName"
#define EH_START	(char *) "startEpochMicrosec"
#define EH_VER		(char *) "version"

// other field field names
#define OF_OBJ_TAG	(char *) "otherFields"
#define OF_NVP_TAG	(char *) "nameValuePairs"
#define OF_VER_TAG	(char *) "otherFieldsVersion"

// measurement fields field names
#define MF_OBJ_TAG	(char *) "measurementFields"
#define MF_AF_TAG	(char *) "additionalFields"
#define MF_DELTA_TAG	(char *) "measurementInterval"
#define MF_VER_TAG	(char *) "measurementFieldsVersion"

// actual version values
#define EH_VERSION_VAL	3
#define MF_VERSION_VAL	(char *) "4.0"
#define OF_VERSION_VAL	(char *) "1.0"


// ------------ prototypes -----------------------------------------------------
std::string event_build( std::shared_ptr<xapp::Jhash> jh, std::shared_ptr<Rthing> rt, long long ts, long long delta, std::string reporter, std::string affected );


// ---------------- simple class to send messages via mule (curl) to the ves thing ----------

/*
	A class that maintains information about a remote VES system and is capable
	of sending out a mule with data.
*/
class Ves_sender {
	std::string	url = "http://localhost:29080";			// bogus but gives something to test with

	public:
	Ves_sender( std::string ves_url );
	~Ves_sender(  );


	bool Send_event( const std::string& event );

};

} // namespace

#endif
