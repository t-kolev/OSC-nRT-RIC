#ifndef _SGNB_ACK_FOR_ADD_MOD_FOR_UE_H__INCLUDED_
#define _SGNB_ACK_FOR_ADD_MOD_FOR_UE_H__INCLUDED_

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

struct _SgNB_ack_for_add_mod_for_ue {
	gs_uint64_t timestamp_ms;
	gs_sp_t gnb_id;
	gs_int64_t id_MeNB_UE_X2AP_ID;
	gs_int64_t id_SgNB_UE_X2AP_ID;
	gs_uint32_t id_MeNB_UE_X2AP_ID_Extension;
	gs_int64_t toRelease0;
	gs_int8_t toRelease0_exists;
	gs_int64_t toRelease1;
	gs_int8_t toRelease1_exists;
	gs_int64_t toRelease2;
	gs_int8_t toRelease2_exists;
	gs_int64_t toRelease3;
	gs_int8_t toRelease3_exists;
	gs_uint8_t recoverPDCP;
	gs_int8_t recoverPDCP_exists;
	gs_uint8_t reestablishPDCP;
	gs_int8_t reestablishPDCP_exists;
	gs_int64_t drb_Identity;
	gs_int64_t eps_BearerIdentity;
};

static inline void init__SgNB_ack_for_add_mod_for_ue(struct _SgNB_ack_for_add_mod_for_ue *m){
	m->toRelease0_exists=0;
	m->toRelease1_exists=0;
	m->toRelease2_exists=0;
	m->toRelease3_exists=0;
	m->recoverPDCP_exists=0;
	m->reestablishPDCP_exists=0;
}

static inline gs_retval_t get_SgNB_ack_for_add_mod_for_ue__timestamp_ms(struct packet *p, gs_uint64_t *t){
	*t = ((struct _SgNB_ack_for_add_mod_for_ue *)(p->record.packed.values))->timestamp_ms;
	return 0;
}

static inline gs_retval_t get_SgNB_ack_for_add_mod_for_ue__gnb_id(struct packet *p, struct gs_string *t){
	t->owner=0;
	t->data = ((struct _SgNB_ack_for_add_mod_for_ue *)(p->record.packed.values))->gnb_id;
	if( t->data == NULL){
		t->length=0;
		return 0;
	}
	t->length = strlen(t->data);
}

static inline gs_retval_t get_SgNB_ack_for_add_mod_for_ue__id_MeNB_UE_X2AP_ID(struct packet *p, gs_int64_t *t){
	*t = ((struct _SgNB_ack_for_add_mod_for_ue *)(p->record.packed.values))->id_MeNB_UE_X2AP_ID;
	return 0;
}

static inline gs_retval_t get_SgNB_ack_for_add_mod_for_ue__id_SgNB_UE_X2AP_ID(struct packet *p, gs_int64_t *t){
	*t = ((struct _SgNB_ack_for_add_mod_for_ue *)(p->record.packed.values))->id_SgNB_UE_X2AP_ID;
	return 0;
}

static inline gs_retval_t get_SgNB_ack_for_add_mod_for_ue__id_MeNB_UE_X2AP_ID_Extension(struct packet *p, gs_uint32_t *t){
	*t = ((struct _SgNB_ack_for_add_mod_for_ue *)(p->record.packed.values))->id_MeNB_UE_X2AP_ID_Extension;
	return 0;
}

static inline gs_retval_t get_SgNB_ack_for_add_mod_for_ue__toRelease0(struct packet *p, gs_int64_t *t){
	*t = ((struct _SgNB_ack_for_add_mod_for_ue *)(p->record.packed.values))->toRelease0;
	return (((struct _SgNB_ack_for_add_mod_for_ue *)(p->record.packed.values))->toRelease0==0);
}

static inline gs_retval_t get_SgNB_ack_for_add_mod_for_ue__toRelease1(struct packet *p, gs_int64_t *t){
	*t = ((struct _SgNB_ack_for_add_mod_for_ue *)(p->record.packed.values))->toRelease1;
	return (((struct _SgNB_ack_for_add_mod_for_ue *)(p->record.packed.values))->toRelease1==0);
}

static inline gs_retval_t get_SgNB_ack_for_add_mod_for_ue__toRelease2(struct packet *p, gs_int64_t *t){
	*t = ((struct _SgNB_ack_for_add_mod_for_ue *)(p->record.packed.values))->toRelease2;
	return (((struct _SgNB_ack_for_add_mod_for_ue *)(p->record.packed.values))->toRelease2==0);
}

static inline gs_retval_t get_SgNB_ack_for_add_mod_for_ue__toRelease3(struct packet *p, gs_int64_t *t){
	*t = ((struct _SgNB_ack_for_add_mod_for_ue *)(p->record.packed.values))->toRelease3;
	return (((struct _SgNB_ack_for_add_mod_for_ue *)(p->record.packed.values))->toRelease3==0);
}

static inline gs_retval_t get_SgNB_ack_for_add_mod_for_ue__recoverPDCP(struct packet *p, gs_uint8_t *t){
	*t = ((struct _SgNB_ack_for_add_mod_for_ue *)(p->record.packed.values))->recoverPDCP;
	return (((struct _SgNB_ack_for_add_mod_for_ue *)(p->record.packed.values))->recoverPDCP==0);
}

static inline gs_retval_t get_SgNB_ack_for_add_mod_for_ue__reestablishPDCP(struct packet *p, gs_uint8_t *t){
	*t = ((struct _SgNB_ack_for_add_mod_for_ue *)(p->record.packed.values))->reestablishPDCP;
	return (((struct _SgNB_ack_for_add_mod_for_ue *)(p->record.packed.values))->reestablishPDCP==0);
}

static inline gs_retval_t get_SgNB_ack_for_add_mod_for_ue__drb_Identity(struct packet *p, gs_int64_t *t){
	*t = ((struct _SgNB_ack_for_add_mod_for_ue *)(p->record.packed.values))->drb_Identity;
	return 0;
}

static inline gs_retval_t get_SgNB_ack_for_add_mod_for_ue__eps_BearerIdentity(struct packet *p, gs_int64_t *t){
	*t = ((struct _SgNB_ack_for_add_mod_for_ue *)(p->record.packed.values))->eps_BearerIdentity;
	return 0;
}

#endif
