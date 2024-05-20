#ifndef _SGNB_ADDREQ_FOR_UE_BEARERS_H__INCLUDED_
#define _SGNB_ADDREQ_FOR_UE_BEARERS_H__INCLUDED_

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

struct _sgnb_addreq_for_ue_bearers {
	gs_uint64_t timestamp_ms;
	gs_sp_t gnb_id;
	gs_int64_t id_MeNB_UE_X2AP_ID;
	gs_int64_t MCG_eRAB_MaximumBitrateDL;
	gs_int64_t pDCPatSgNB;
	gs_int64_t drb_ID;
	gs_int64_t priorityLevel;
	ProtobufCBinaryData gTP_TEID;
	gs_int64_t pre_emptionCapability;
	gs_int64_t MCG_eRAB_GuaranteedBitrateUL;
	gs_int64_t mCGresources;
	ProtobufCBinaryData transportLayerAddress;
	gs_int64_t full_eRAB_GuaranteedBitrateUL;
	gs_int64_t sCGresources;
	gs_int64_t MCG_eRAB_MaximumBitrateUL;
	gs_int64_t full_eRAB_MaximumBitrateUL;
	gs_int64_t pre_emptionVulnerability;
	gs_int64_t e_RAB_ID;
	gs_int64_t MCG_eRAB_GuaranteedBitrateDL;
	gs_int64_t qCI;
	gs_int64_t full_eRAB_MaximumBitrateDL;
	gs_int64_t full_eRAB_GuaranteedBitrateDL;
};

static inline void init__sgnb_addreq_for_ue_bearers(struct _sgnb_addreq_for_ue_bearers *m){
}

static inline gs_retval_t get_sgnb_addreq_for_ue_bearers__timestamp_ms(struct packet *p, gs_uint64_t *t){
	*t = ((struct _sgnb_addreq_for_ue_bearers *)(p->record.packed.values))->timestamp_ms;
	return 0;
}

static inline gs_retval_t get_sgnb_addreq_for_ue_bearers__gnb_id(struct packet *p, struct gs_string *t){
	t->owner=0;
	t->data = ((struct _sgnb_addreq_for_ue_bearers *)(p->record.packed.values))->gnb_id;
	if( t->data == NULL){
		t->length=0;
		return 0;
	}
	t->length = strlen(t->data);
}

static inline gs_retval_t get_sgnb_addreq_for_ue_bearers__id_MeNB_UE_X2AP_ID(struct packet *p, gs_int64_t *t){
	*t = ((struct _sgnb_addreq_for_ue_bearers *)(p->record.packed.values))->id_MeNB_UE_X2AP_ID;
	return 0;
}

static inline gs_retval_t get_sgnb_addreq_for_ue_bearers__MCG_eRAB_MaximumBitrateDL(struct packet *p, gs_int64_t *t){
	*t = ((struct _sgnb_addreq_for_ue_bearers *)(p->record.packed.values))->MCG_eRAB_MaximumBitrateDL;
	return 0;
}

static inline gs_retval_t get_sgnb_addreq_for_ue_bearers__pDCPatSgNB(struct packet *p, gs_int64_t *t){
	*t = ((struct _sgnb_addreq_for_ue_bearers *)(p->record.packed.values))->pDCPatSgNB;
	return 0;
}

static inline gs_retval_t get_sgnb_addreq_for_ue_bearers__drb_ID(struct packet *p, gs_int64_t *t){
	*t = ((struct _sgnb_addreq_for_ue_bearers *)(p->record.packed.values))->drb_ID;
	return 0;
}

static inline gs_retval_t get_sgnb_addreq_for_ue_bearers__priorityLevel(struct packet *p, gs_int64_t *t){
	*t = ((struct _sgnb_addreq_for_ue_bearers *)(p->record.packed.values))->priorityLevel;
	return 0;
}

static inline gs_retval_t get_sgnb_addreq_for_ue_bearers__gTP_TEID(struct packet *p, struct gs_string *t){
	t->owner=0;
	t->data = ((struct _sgnb_addreq_for_ue_bearers *)(p->record.packed.values))->gTP_TEID.data;
	if(t->data==NULL){
		t->length=0;
		return 0;
	}
	t->length = ((struct _sgnb_addreq_for_ue_bearers *)(p->record.packed.values))->gTP_TEID.len;
}

static inline gs_retval_t get_sgnb_addreq_for_ue_bearers__pre_emptionCapability(struct packet *p, gs_int64_t *t){
	*t = ((struct _sgnb_addreq_for_ue_bearers *)(p->record.packed.values))->pre_emptionCapability;
	return 0;
}

static inline gs_retval_t get_sgnb_addreq_for_ue_bearers__MCG_eRAB_GuaranteedBitrateUL(struct packet *p, gs_int64_t *t){
	*t = ((struct _sgnb_addreq_for_ue_bearers *)(p->record.packed.values))->MCG_eRAB_GuaranteedBitrateUL;
	return 0;
}

static inline gs_retval_t get_sgnb_addreq_for_ue_bearers__mCGresources(struct packet *p, gs_int64_t *t){
	*t = ((struct _sgnb_addreq_for_ue_bearers *)(p->record.packed.values))->mCGresources;
	return 0;
}

static inline gs_retval_t get_sgnb_addreq_for_ue_bearers__transportLayerAddress(struct packet *p, struct gs_string *t){
	t->owner=0;
	t->data = ((struct _sgnb_addreq_for_ue_bearers *)(p->record.packed.values))->transportLayerAddress.data;
	if(t->data==NULL){
		t->length=0;
		return 0;
	}
	t->length = ((struct _sgnb_addreq_for_ue_bearers *)(p->record.packed.values))->transportLayerAddress.len;
}

static inline gs_retval_t get_sgnb_addreq_for_ue_bearers__full_eRAB_GuaranteedBitrateUL(struct packet *p, gs_int64_t *t){
	*t = ((struct _sgnb_addreq_for_ue_bearers *)(p->record.packed.values))->full_eRAB_GuaranteedBitrateUL;
	return 0;
}

static inline gs_retval_t get_sgnb_addreq_for_ue_bearers__sCGresources(struct packet *p, gs_int64_t *t){
	*t = ((struct _sgnb_addreq_for_ue_bearers *)(p->record.packed.values))->sCGresources;
	return 0;
}

static inline gs_retval_t get_sgnb_addreq_for_ue_bearers__MCG_eRAB_MaximumBitrateUL(struct packet *p, gs_int64_t *t){
	*t = ((struct _sgnb_addreq_for_ue_bearers *)(p->record.packed.values))->MCG_eRAB_MaximumBitrateUL;
	return 0;
}

static inline gs_retval_t get_sgnb_addreq_for_ue_bearers__full_eRAB_MaximumBitrateUL(struct packet *p, gs_int64_t *t){
	*t = ((struct _sgnb_addreq_for_ue_bearers *)(p->record.packed.values))->full_eRAB_MaximumBitrateUL;
	return 0;
}

static inline gs_retval_t get_sgnb_addreq_for_ue_bearers__pre_emptionVulnerability(struct packet *p, gs_int64_t *t){
	*t = ((struct _sgnb_addreq_for_ue_bearers *)(p->record.packed.values))->pre_emptionVulnerability;
	return 0;
}

static inline gs_retval_t get_sgnb_addreq_for_ue_bearers__e_RAB_ID(struct packet *p, gs_int64_t *t){
	*t = ((struct _sgnb_addreq_for_ue_bearers *)(p->record.packed.values))->e_RAB_ID;
	return 0;
}

static inline gs_retval_t get_sgnb_addreq_for_ue_bearers__MCG_eRAB_GuaranteedBitrateDL(struct packet *p, gs_int64_t *t){
	*t = ((struct _sgnb_addreq_for_ue_bearers *)(p->record.packed.values))->MCG_eRAB_GuaranteedBitrateDL;
	return 0;
}

static inline gs_retval_t get_sgnb_addreq_for_ue_bearers__qCI(struct packet *p, gs_int64_t *t){
	*t = ((struct _sgnb_addreq_for_ue_bearers *)(p->record.packed.values))->qCI;
	return 0;
}

static inline gs_retval_t get_sgnb_addreq_for_ue_bearers__full_eRAB_MaximumBitrateDL(struct packet *p, gs_int64_t *t){
	*t = ((struct _sgnb_addreq_for_ue_bearers *)(p->record.packed.values))->full_eRAB_MaximumBitrateDL;
	return 0;
}

static inline gs_retval_t get_sgnb_addreq_for_ue_bearers__full_eRAB_GuaranteedBitrateDL(struct packet *p, gs_int64_t *t){
	*t = ((struct _sgnb_addreq_for_ue_bearers *)(p->record.packed.values))->full_eRAB_GuaranteedBitrateDL;
	return 0;
}

#endif
