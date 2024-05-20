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
    Mnemonic:	munchkin.cpp
    Abstract:	The stats muncher. Listens for RMR messages and sends them
				along to some collector. (It doesn't get any more exciting
				than this.)

				Options and Configuration
				If the config file name is provided on the command line (-c name)
				then it is parsed at the point it is encountered. Any options
				which appeear before the -c option will be overridden if they
				are defined in the config, and any options following the -c option
				will override the config file.  Use care!

    Date:       16 June 2020
    Author:     E. Scott Daniels
*/

#include <string.h>
#include <stdio.h>

#include <memory>

#include "rmr/RIC_message_types.h"

#include "ricxfcpp/message.hpp"
#include "ricxfcpp/msg_component.hpp"
#include "ricxfcpp/xapp.hpp"
#include "ricxfcpp/jhash.hpp"

#include "rthing.h"
#include "context.h"
#include "tools.h"
#include "ves_msg.h"

#include "config.h"


#ifndef RIC_METRICS
	#define RIC_METRICS 120			// a message type for us to munch
#endif

/*
	Driven for each source message. We expect that the message contains well
	formed json which we'll parse with the assumption it looks like the
	following (commas omitted for ease, comments added for clarity):
		{
			"reporter": "sender name"		// source from RMR message used if missing
			"generator"	"<string>"			// system which produced the value, to which the value applies, etc.
			"timestamp":	<time>			// unix timestamp (microseconds), current time used if missing
			"data": [
				{
					"id": "<string>"		// the status/metric ID
					"type": "<string>"		// some kind of type (counter assumed if missing)
					"value":  double
				}
			]
		}

	Data will be a pointer to our context.
*/
extern void muncher1_cb( xapp::Message& mbuf, int mtype, int subid, int len, xapp::Msg_component payload,  void* data ) {
	std::string	event;					// ves event
	char*		pbytes;					// direct porinter to the bytes in the payload
	double		timestamp = 0.0;
	double		value;					// value sussed from json goo
	long long	delta;

	if( data == NULL ) {
		mt_log( MT_LOG_WARN, "msg callback called with nil context pointer; message dropped" );
		return;
	}
	munchkin::Context* ctx = (munchkin::Context *) data;

	auto upsrc = mbuf.Get_src();
	auto reporter = std::string( (char *) upsrc.get() );		// grab sender (key to find rt knowledge)

	auto rtk = ctx->Find_rt( reporter );						// ptr to knowledge aobut a remote thing
	if( rtk == NULL ) {
		return;
	}

	rtk->Inc_count();
	delta = rtk->Set_access_ts();				// update the timetamp in the rt and give back the delta

	mt_log( MT_LOG_INFO, (char *) "callback has received a message len=%d from src=%s\n", len, reporter.c_str() );

	pbytes = (char *) payload.get();
	mt_log( MT_LOG_DEBUG, (char *) "message: %d bytes (%s)\n", len, pbytes );

	auto jh = std::shared_ptr<xapp::Jhash>( new xapp::Jhash( pbytes ) );			// parse the json, get a hash context
	if( ! jh->Parse_errors() ) {													// parse was clean

		auto sval = jh->String( (char *) "reporter" );				// overrided default if in the json
		if( sval.compare( "" ) != 0 ) {
			reporter = sval;
		}

		std::string affected = reporter;							// affected system is reporter by default
		sval = jh->String( (char *) "affected" );					// but might be overridden
		if( sval.compare( "" ) != 0 ) {
			affected = sval;
		}

		if( jh->Exists( (char *) "timestamp" ) ) {
			timestamp = jh->Value( (char *) "timestamp" );
			if( timestamp < 10000000000.0 ) {
				timestamp *= 1000000.0;					// assume seconds passed and convert to mu-s
			} else {
				if( timestamp < 1000000000000000.0 ) {
					timestamp *= 1000.0;				// assume milliseconds; convert to mu-s
				}
			}
		} else {
			timestamp = mt_now();						// mt returns mu-s
		}

		event = munchkin::event_build( jh,  rtk, timestamp, delta, reporter, affected );
		if( event.compare( "" ) != 0 ) {
			mt_log( MT_LOG_DEBUG, (const char *) "muncher_cb: generating event: %s\n", event.c_str() );

			ctx->Get_sender()->Send_event( event );
		}

		return;
	} else {
		mt_log( MT_LOG_WARN, "unparsable json received from: %s", reporter.c_str() );
	}

	mt_log( MT_LOG_WARN, (char *) "<MUNCHKIN> unable to parse the json: %s\n", pbytes );
}

/*
	Default callback for any unrecognised message.
*/
void default_cb( xapp::Message& mbuf, int mtype, int subid, int len, xapp::Msg_component payload,  void* data ) {
	mt_log( MT_LOG_WARN, (char *) "<MUNCHKIN> default_cb: unrecognised message received: type=%d subid=%d len=%d\n", mtype, subid, len );
}


int main( int argc, char** argv ) {
	std::shared_ptr<munchkin::Context>	ctx;							// our context
	std::string ves_target = "http://localhost:29080";			// default endpoint for ves curl messages
	char*						port = (char *) "4560";
	int							ai = 1;							// arg processing index
	int							nthreads = 1;
	bool						wait4rt = false;
	bool						hr_logging = false;				// true if human readable logging requested
	char*						token;
	std::shared_ptr<xapp::Jhash>	cjh;						// config file parsed into a framework jhash
	std::string					cfilename = "";					// config file (-f)
	std::string					svalue;
	std::string					port_name= "rmr-data";			// default app name needed to find in config for port
	std::string					log_level = "warn";
	std::string					log_target = "";

	if( (token = getenv( ("MG_TARGET_URL" ))) != NULL ) {		// any environment variable settings first
		ves_target = strdup( token );
	}

	// very simple flag processing
	while( ai < argc ) {
		if( argv[ai][0] != '-' )  {
			break;
		}

		switch( argv[ai][1] ) {						// we only support -x so -xy must be -x -y
			case 'c':								// config file name; parse and set values NOW!
				ensure_nxt_arg( argc, ai+1, 1 );
				cjh = munchkin::Conf_parse( argv[ai+1] );
				if( cjh != NULL ) {
					svalue = munchkin::Conf_get_port( cjh, port_name );
					if( svalue.compare( "" ) != 0 ) {
						port = strdup( svalue.c_str() );
					}
					ves_target = munchkin::Conf_get_cvstr( cjh, "collector_url", ves_target );
					hr_logging = munchkin::Conf_get_cvbool( cjh, "hr_logging", false );
					log_level = munchkin::Conf_get_cvstr( cjh, "log_level", log_level );
					log_target = munchkin::Conf_get_cvstr( cjh, "log_file", log_target );
					wait4rt = munchkin::Conf_get_cvbool( cjh, "wait4rt", true );
				}
				ai++;
				break;

			case 'd':
				log_level = "debug";
				break;

			case 'l':								// set log target away from stderr
				ensure_nxt_arg( argc, ai+1, 1 );	// ensure the data is there
				log_target = std::string( argv[ai+1] );
				ai++;
				break;

			case 'P':
				ensure_nxt_arg( argc, ai+1, 1 );
				port_name = std::string( argv[ai+1] );
				break;

			case 'p':
				ensure_nxt_arg( argc, ai+1, 1 );
				port = argv[ai+1];
				ai++;
				break;

			case 'r':
				hr_logging = true;
				break;

			case 't':
				ensure_nxt_arg( argc, ai+1, 1 );
				nthreads = atoi( argv[ai+1] );
				ai++;
				break;

			case 'T':
				ensure_nxt_arg( argc, ai+1, 1 );
				ves_target = strdup( argv[ai+1] );
				ai++;
				break;

			case 'w':
				wait4rt = true;
				ai++;
				break;

			case 'v':								// verbosity on; keep encrusted log msgs
				log_level = "info";
				break;

			case 'V':								// verbosity on; generate readable messages
				log_level = "info";
				hr_logging = true;
				break;

			case '?':
				usage( argv[0] );
				exit( 0 );
				break;

			default:
				fprintf( stderr, "unrecognised option: %s\n", argv[ai] );
				usage( argv[2] );
				exit( 1 );
				break;
		}

		ai++;
	}

	mt_log_str2level( log_level.c_str() );		// set log options before doing real work
	mt_log_set_hr( (int) hr_logging );
	if( log_target.compare( "" ) != 0 ) {
		mt_log_target( log_target.c_str() );
	}

	if( nthreads < 1 || nthreads > 32 ) {
		mt_log( MT_LOG_ERR, (char *) "thread count is not sane; please pick a value between 1 and 32 inclusive" );
		exit( 1 );
	}

	mt_log( MT_LOG_INFO, (char *) "listening on port: %s\n", port );
	mt_log( MT_LOG_INFO, (char *) "starting %d callback threads\n", nthreads );

	ctx = std::shared_ptr<munchkin::Context>( new munchkin::Context( port, wait4rt, ves_target ) );

	// start housekeeping thread here

	ctx->Register_cb( RIC_METRICS, muncher1_cb );				// register callbacks
	ctx->Register_cb( -1, default_cb );

	ctx->Run( nthreads );				// call should never return

	mt_log( MT_LOG_ERR, (char *) "internal mishap: framework returned and that's not right\n" );
}

