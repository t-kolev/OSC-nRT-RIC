#ifndef _SGNB_MOD_REQ_H__INCLUDED_
#define _SGNB_MOD_REQ_H__INCLUDED_

/*
==============================================================================

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
=============================================================================
*/


#include "packet.h"


#include "/usr/local/include/protobuf-c/protobuf-c.h"

struct _sgnb_mod_req {
	gs_uint64_t timestamp_ms;
	gs_sp_t gnb_id;
	gs_int64_t cause_protocol;
	gs_int64_t id_MeNB_UE_X2AP_ID;
	gs_int64_t cause_transport;
	gs_int64_t failureType;
	gs_int64_t cause_radio_network;
	gs_int64_t id_SgNB_UE_X2AP_ID;
	gs_int64_t cause_misc;
};

static inline void init__sgnb_mod_req(struct _sgnb_mod_req *m){
}

static inline gs_retval_t get_sgnb_mod_req__timestamp_ms(struct packet *p, gs_uint64_t *t){
	*t = ((struct _sgnb_mod_req *)(p->record.packed.values))->timestamp_ms;
	return 0;
}

static inline gs_retval_t get_sgnb_mod_req__gnb_id(struct packet *p, struct gs_string *t){
	t->owner=0;
	t->data = ((struct _sgnb_mod_req *)(p->record.packed.values))->gnb_id;
	if( t->data == NULL){
		t->length=0;
		return 0;
	}
	t->length = strlen(t->data);
}

static inline gs_retval_t get_sgnb_mod_req__cause_protocol(struct packet *p, gs_int64_t *t){
	*t = ((struct _sgnb_mod_req *)(p->record.packed.values))->cause_protocol;
	return 0;
}

static inline gs_retval_t get_sgnb_mod_req__id_MeNB_UE_X2AP_ID(struct packet *p, gs_int64_t *t){
	*t = ((struct _sgnb_mod_req *)(p->record.packed.values))->id_MeNB_UE_X2AP_ID;
	return 0;
}

static inline gs_retval_t get_sgnb_mod_req__cause_transport(struct packet *p, gs_int64_t *t){
	*t = ((struct _sgnb_mod_req *)(p->record.packed.values))->cause_transport;
	return 0;
}

static inline gs_retval_t get_sgnb_mod_req__failureType(struct packet *p, gs_int64_t *t){
	*t = ((struct _sgnb_mod_req *)(p->record.packed.values))->failureType;
	return 0;
}

static inline gs_retval_t get_sgnb_mod_req__cause_radio_network(struct packet *p, gs_int64_t *t){
	*t = ((struct _sgnb_mod_req *)(p->record.packed.values))->cause_radio_network;
	return 0;
}

static inline gs_retval_t get_sgnb_mod_req__id_SgNB_UE_X2AP_ID(struct packet *p, gs_int64_t *t){
	*t = ((struct _sgnb_mod_req *)(p->record.packed.values))->id_SgNB_UE_X2AP_ID;
	return 0;
}

static inline gs_retval_t get_sgnb_mod_req__cause_misc(struct packet *p, gs_int64_t *t){
	*t = ((struct _sgnb_mod_req *)(p->record.packed.values))->cause_misc;
	return 0;
}

#endif
