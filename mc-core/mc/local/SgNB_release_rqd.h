#ifndef _SGNB_RELEASE_RQD_H__INCLUDED_
#define _SGNB_RELEASE_RQD_H__INCLUDED_

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

struct _SgNB_release_rqd {
	gs_uint64_t timestamp_ms;
	gs_sp_t gnb_id;
	gs_int64_t cause_protocol;
	gs_int64_t id_MeNB_UE_X2AP_ID;
	gs_int64_t cause_transport;
	gs_int64_t e_RAB_ID0;
	gs_int8_t e_RAB_ID0_exists;
	gs_int64_t e_RAB_ID1;
	gs_int8_t e_RAB_ID1_exists;
	gs_int64_t e_RAB_ID2;
	gs_int8_t e_RAB_ID2_exists;
	gs_int64_t e_RAB_ID3;
	gs_int8_t e_RAB_ID3_exists;
	gs_int64_t e_RAB_ID4;
	gs_int8_t e_RAB_ID4_exists;
	gs_int64_t e_RAB_ID5;
	gs_int8_t e_RAB_ID5_exists;
	gs_int64_t e_RAB_ID6;
	gs_int8_t e_RAB_ID6_exists;
	gs_int64_t e_RAB_ID7;
	gs_int8_t e_RAB_ID7_exists;
	gs_int64_t cause_radio_network;
	gs_int64_t id_SgNB_UE_X2AP_ID;
	gs_int64_t cause_misc;
};

static inline void init__SgNB_release_rqd(struct _SgNB_release_rqd *m){
	m->e_RAB_ID0_exists=0;
	m->e_RAB_ID1_exists=0;
	m->e_RAB_ID2_exists=0;
	m->e_RAB_ID3_exists=0;
	m->e_RAB_ID4_exists=0;
	m->e_RAB_ID5_exists=0;
	m->e_RAB_ID6_exists=0;
	m->e_RAB_ID7_exists=0;
}

static inline gs_retval_t get_SgNB_release_rqd__timestamp_ms(struct packet *p, gs_uint64_t *t){
	*t = ((struct _SgNB_release_rqd *)(p->record.packed.values))->timestamp_ms;
	return 0;
}

static inline gs_retval_t get_SgNB_release_rqd__gnb_id(struct packet *p, struct gs_string *t){
	t->owner=0;
	t->data = ((struct _SgNB_release_rqd *)(p->record.packed.values))->gnb_id;
	if( t->data == NULL){
		t->length=0;
		return 0;
	}
	t->length = strlen(t->data);
}

static inline gs_retval_t get_SgNB_release_rqd__cause_protocol(struct packet *p, gs_int64_t *t){
	*t = ((struct _SgNB_release_rqd *)(p->record.packed.values))->cause_protocol;
	return 0;
}

static inline gs_retval_t get_SgNB_release_rqd__id_MeNB_UE_X2AP_ID(struct packet *p, gs_int64_t *t){
	*t = ((struct _SgNB_release_rqd *)(p->record.packed.values))->id_MeNB_UE_X2AP_ID;
	return 0;
}

static inline gs_retval_t get_SgNB_release_rqd__cause_transport(struct packet *p, gs_int64_t *t){
	*t = ((struct _SgNB_release_rqd *)(p->record.packed.values))->cause_transport;
	return 0;
}

static inline gs_retval_t get_SgNB_release_rqd__e_RAB_ID0(struct packet *p, gs_int64_t *t){
	*t = ((struct _SgNB_release_rqd *)(p->record.packed.values))->e_RAB_ID0;
	return (((struct _SgNB_release_rqd *)(p->record.packed.values))->e_RAB_ID0==0);
}

static inline gs_retval_t get_SgNB_release_rqd__e_RAB_ID1(struct packet *p, gs_int64_t *t){
	*t = ((struct _SgNB_release_rqd *)(p->record.packed.values))->e_RAB_ID1;
	return (((struct _SgNB_release_rqd *)(p->record.packed.values))->e_RAB_ID1==0);
}

static inline gs_retval_t get_SgNB_release_rqd__e_RAB_ID2(struct packet *p, gs_int64_t *t){
	*t = ((struct _SgNB_release_rqd *)(p->record.packed.values))->e_RAB_ID2;
	return (((struct _SgNB_release_rqd *)(p->record.packed.values))->e_RAB_ID2==0);
}

static inline gs_retval_t get_SgNB_release_rqd__e_RAB_ID3(struct packet *p, gs_int64_t *t){
	*t = ((struct _SgNB_release_rqd *)(p->record.packed.values))->e_RAB_ID3;
	return (((struct _SgNB_release_rqd *)(p->record.packed.values))->e_RAB_ID3==0);
}

static inline gs_retval_t get_SgNB_release_rqd__e_RAB_ID4(struct packet *p, gs_int64_t *t){
	*t = ((struct _SgNB_release_rqd *)(p->record.packed.values))->e_RAB_ID4;
	return (((struct _SgNB_release_rqd *)(p->record.packed.values))->e_RAB_ID4==0);
}

static inline gs_retval_t get_SgNB_release_rqd__e_RAB_ID5(struct packet *p, gs_int64_t *t){
	*t = ((struct _SgNB_release_rqd *)(p->record.packed.values))->e_RAB_ID5;
	return (((struct _SgNB_release_rqd *)(p->record.packed.values))->e_RAB_ID5==0);
}

static inline gs_retval_t get_SgNB_release_rqd__e_RAB_ID6(struct packet *p, gs_int64_t *t){
	*t = ((struct _SgNB_release_rqd *)(p->record.packed.values))->e_RAB_ID6;
	return (((struct _SgNB_release_rqd *)(p->record.packed.values))->e_RAB_ID6==0);
}

static inline gs_retval_t get_SgNB_release_rqd__e_RAB_ID7(struct packet *p, gs_int64_t *t){
	*t = ((struct _SgNB_release_rqd *)(p->record.packed.values))->e_RAB_ID7;
	return (((struct _SgNB_release_rqd *)(p->record.packed.values))->e_RAB_ID7==0);
}

static inline gs_retval_t get_SgNB_release_rqd__cause_radio_network(struct packet *p, gs_int64_t *t){
	*t = ((struct _SgNB_release_rqd *)(p->record.packed.values))->cause_radio_network;
	return 0;
}

static inline gs_retval_t get_SgNB_release_rqd__id_SgNB_UE_X2AP_ID(struct packet *p, gs_int64_t *t){
	*t = ((struct _SgNB_release_rqd *)(p->record.packed.values))->id_SgNB_UE_X2AP_ID;
	return 0;
}

static inline gs_retval_t get_SgNB_release_rqd__cause_misc(struct packet *p, gs_int64_t *t){
	*t = ((struct _SgNB_release_rqd *)(p->record.packed.values))->cause_misc;
	return 0;
}

#endif
