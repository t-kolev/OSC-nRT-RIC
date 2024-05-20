#ifndef _SGNB_RELEASE_CONFIRM_FROM_MENB_ERABS_H__INCLUDED_
#define _SGNB_RELEASE_CONFIRM_FROM_MENB_ERABS_H__INCLUDED_

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

struct _sgnb_release_confirm_from_menb_erabs {
	gs_uint64_t timestamp_ms;
	gs_sp_t gnb_id;
	gs_int64_t id_MeNB_UE_X2AP_ID;
	gs_int64_t id_SgNB_UE_X2AP_ID;
	gs_uint32_t id_MeNB_UE_X2AP_ID_Extension;
	gs_int64_t sCGresources;
	gs_int64_t e_RAB_ID;
	gs_int64_t pDCPatSgNB;
	gs_int64_t mCGresources;
	ProtobufCBinaryData gTP_TEID;
	ProtobufCBinaryData transportLayerAddress;
};

static inline void init__sgnb_release_confirm_from_menb_erabs(struct _sgnb_release_confirm_from_menb_erabs *m){
}

static inline gs_retval_t get_sgnb_release_confirm_from_menb_erabs__timestamp_ms(struct packet *p, gs_uint64_t *t){
	*t = ((struct _sgnb_release_confirm_from_menb_erabs *)(p->record.packed.values))->timestamp_ms;
	return 0;
}

static inline gs_retval_t get_sgnb_release_confirm_from_menb_erabs__gnb_id(struct packet *p, struct gs_string *t){
	t->owner=0;
	t->data = ((struct _sgnb_release_confirm_from_menb_erabs *)(p->record.packed.values))->gnb_id;
	if( t->data == NULL){
		t->length=0;
		return 0;
	}
	t->length = strlen(t->data);
}

static inline gs_retval_t get_sgnb_release_confirm_from_menb_erabs__id_MeNB_UE_X2AP_ID(struct packet *p, gs_int64_t *t){
	*t = ((struct _sgnb_release_confirm_from_menb_erabs *)(p->record.packed.values))->id_MeNB_UE_X2AP_ID;
	return 0;
}

static inline gs_retval_t get_sgnb_release_confirm_from_menb_erabs__id_SgNB_UE_X2AP_ID(struct packet *p, gs_int64_t *t){
	*t = ((struct _sgnb_release_confirm_from_menb_erabs *)(p->record.packed.values))->id_SgNB_UE_X2AP_ID;
	return 0;
}

static inline gs_retval_t get_sgnb_release_confirm_from_menb_erabs__id_MeNB_UE_X2AP_ID_Extension(struct packet *p, gs_uint32_t *t){
	*t = ((struct _sgnb_release_confirm_from_menb_erabs *)(p->record.packed.values))->id_MeNB_UE_X2AP_ID_Extension;
	return 0;
}

static inline gs_retval_t get_sgnb_release_confirm_from_menb_erabs__sCGresources(struct packet *p, gs_int64_t *t){
	*t = ((struct _sgnb_release_confirm_from_menb_erabs *)(p->record.packed.values))->sCGresources;
	return 0;
}

static inline gs_retval_t get_sgnb_release_confirm_from_menb_erabs__e_RAB_ID(struct packet *p, gs_int64_t *t){
	*t = ((struct _sgnb_release_confirm_from_menb_erabs *)(p->record.packed.values))->e_RAB_ID;
	return 0;
}

static inline gs_retval_t get_sgnb_release_confirm_from_menb_erabs__pDCPatSgNB(struct packet *p, gs_int64_t *t){
	*t = ((struct _sgnb_release_confirm_from_menb_erabs *)(p->record.packed.values))->pDCPatSgNB;
	return 0;
}

static inline gs_retval_t get_sgnb_release_confirm_from_menb_erabs__mCGresources(struct packet *p, gs_int64_t *t){
	*t = ((struct _sgnb_release_confirm_from_menb_erabs *)(p->record.packed.values))->mCGresources;
	return 0;
}

static inline gs_retval_t get_sgnb_release_confirm_from_menb_erabs__gTP_TEID(struct packet *p, struct gs_string *t){
	t->owner=0;
	t->data = ((struct _sgnb_release_confirm_from_menb_erabs *)(p->record.packed.values))->gTP_TEID.data;
	if(t->data==NULL){
		t->length=0;
		return 0;
	}
	t->length = ((struct _sgnb_release_confirm_from_menb_erabs *)(p->record.packed.values))->gTP_TEID.len;
}

static inline gs_retval_t get_sgnb_release_confirm_from_menb_erabs__transportLayerAddress(struct packet *p, struct gs_string *t){
	t->owner=0;
	t->data = ((struct _sgnb_release_confirm_from_menb_erabs *)(p->record.packed.values))->transportLayerAddress.data;
	if(t->data==NULL){
		t->length=0;
		return 0;
	}
	t->length = ((struct _sgnb_release_confirm_from_menb_erabs *)(p->record.packed.values))->transportLayerAddress.len;
}

#endif
