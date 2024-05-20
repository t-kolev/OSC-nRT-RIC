// vim: ts=4 sw=4 noet:
/*
--------------------------------------------------------------------------------
	Copyright (c) 2018-2019 AT&T Intellectual Property.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

	   http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
--------------------------------------------------------------------------------
*/

/*
	Mnemonic:	test_rmr_em.c.
	Abstract:	Emulates some RMR functions (message receipt mostly)
				so that we can unit test.

				This file should be included by the test programme BEFORE
				any module which uses RMR functions is included

	Date:		10 December 2019
	Author:		E. Scott Daniels
*/

#include <rmr/rmr.h>
#include <rmr/RIC_message_types.h>


/*
	The first call will generate a timeout for testing. All others will
	succeed with a message.
*/
extern rmr_mbuf_t* RMR_torcv_msg( void* ctx, rmr_mbuf_t* old_msg, int ms_to ) {
	static int timeout = 0;
	static int hcheck = 0;

	if( old_msg == NULL ) {
		old_msg = rmr_alloc_msg( ctx, 256 );
		if( old_msg == NULL ) {
			return NULL;
		}
	}

	timeout = ! timeout;					// every other message results in a timeout
	if( timeout ) {
		old_msg->state = RMR_ERR_TIMEOUT;
		return old_msg;	
	}

	if( !hcheck ) {									// send one health check message
		old_msg->mtype = RIC_HEALTH_CHECK_REQ;
		old_msg->state = 0;
		hcheck = 1;
		return old_msg;
	}


	snprintf( old_msg->payload, rmr_payload_size( old_msg ), "DUMMY MESSAGE" );
	old_msg->mtype = TEST_MTYPE;
	old_msg->len = strlen( old_msg->payload );
	old_msg->sub_id = -1;
	old_msg->state = 0;

	return old_msg;
}

/*
	Always successful if a message was passed in.
*/
extern rmr_mbuf_t* RMR_rts_msg( void* ctx, rmr_mbuf_t* msg ) {
	if( msg != NULL ) {
		msg->state = 0;
	}

	return msg;
}

/*
	Generates "short" reads 9 out of 10 times so that we can
	drive the buffer construction code that otherwise wouldn't
	be driven since long reads are usually the case.
*/
extern int Read( int fd, char* dest, int max ) {
	static int count = -1;

	count++;

	if( count % 10 ) {
		return read( fd, dest, max );		// return long read
	}

	return read( fd, dest, 3 );             // short read
}

// ----------- defines that point included code here --------------------
#define rmr_torcv_msg RMR_torcv_msg
#define rmr_rts_msg RMR_rts_msg
#define read Read
