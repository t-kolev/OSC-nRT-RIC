#ifndef _SGNB_ACK_FOR_UE_MEASUREMENTS_H__INCLUDED_
#define _SGNB_ACK_FOR_UE_MEASUREMENTS_H__INCLUDED_

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

struct _SgNB_ack_for_ue_measurements {
	gs_uint64_t timestamp_ms;
	gs_sp_t gnb_id;
	gs_int64_t id_MeNB_UE_X2AP_ID;
	gs_int64_t id_SgNB_UE_X2AP_ID;
	gs_uint32_t id_MeNB_UE_X2AP_ID_Extension;
	gs_uint32_t ssbFrequency;
	gs_int8_t ssbFrequency_exists;
	gs_uint32_t refFreqCSI_RS;
	gs_int8_t refFreqCSI_RS_exists;
	gs_uint32_t physCellId;
	gs_int8_t physCellId_exists;
	gs_uint32_t rsrq;
	gs_int8_t rsrq_exists;
	gs_uint32_t rsrp;
	gs_int8_t rsrp_exists;
	gs_uint32_t sinr;
	gs_int8_t sinr_exists;
};

static inline void init__SgNB_ack_for_ue_measurements(struct _SgNB_ack_for_ue_measurements *m){
	m->ssbFrequency_exists=0;
	m->refFreqCSI_RS_exists=0;
	m->physCellId_exists=0;
	m->rsrq_exists=0;
	m->rsrp_exists=0;
	m->sinr_exists=0;
}

static inline gs_retval_t get_SgNB_ack_for_ue_measurements__timestamp_ms(struct packet *p, gs_uint64_t *t){
	*t = ((struct _SgNB_ack_for_ue_measurements *)(p->record.packed.values))->timestamp_ms;
	return 0;
}

static inline gs_retval_t get_SgNB_ack_for_ue_measurements__gnb_id(struct packet *p, struct gs_string *t){
	t->owner=0;
	t->data = ((struct _SgNB_ack_for_ue_measurements *)(p->record.packed.values))->gnb_id;
	if( t->data == NULL){
		t->length=0;
		return 0;
	}
	t->length = strlen(t->data);
}

static inline gs_retval_t get_SgNB_ack_for_ue_measurements__id_MeNB_UE_X2AP_ID(struct packet *p, gs_int64_t *t){
	*t = ((struct _SgNB_ack_for_ue_measurements *)(p->record.packed.values))->id_MeNB_UE_X2AP_ID;
	return 0;
}

static inline gs_retval_t get_SgNB_ack_for_ue_measurements__id_SgNB_UE_X2AP_ID(struct packet *p, gs_int64_t *t){
	*t = ((struct _SgNB_ack_for_ue_measurements *)(p->record.packed.values))->id_SgNB_UE_X2AP_ID;
	return 0;
}

static inline gs_retval_t get_SgNB_ack_for_ue_measurements__id_MeNB_UE_X2AP_ID_Extension(struct packet *p, gs_uint32_t *t){
	*t = ((struct _SgNB_ack_for_ue_measurements *)(p->record.packed.values))->id_MeNB_UE_X2AP_ID_Extension;
	return 0;
}

static inline gs_retval_t get_SgNB_ack_for_ue_measurements__ssbFrequency(struct packet *p, gs_uint32_t *t){
	*t = ((struct _SgNB_ack_for_ue_measurements *)(p->record.packed.values))->ssbFrequency;
	return (((struct _SgNB_ack_for_ue_measurements *)(p->record.packed.values))->ssbFrequency==0);
}

static inline gs_retval_t get_SgNB_ack_for_ue_measurements__refFreqCSI_RS(struct packet *p, gs_uint32_t *t){
	*t = ((struct _SgNB_ack_for_ue_measurements *)(p->record.packed.values))->refFreqCSI_RS;
	return (((struct _SgNB_ack_for_ue_measurements *)(p->record.packed.values))->refFreqCSI_RS==0);
}

static inline gs_retval_t get_SgNB_ack_for_ue_measurements__physCellId(struct packet *p, gs_uint32_t *t){
	*t = ((struct _SgNB_ack_for_ue_measurements *)(p->record.packed.values))->physCellId;
	return (((struct _SgNB_ack_for_ue_measurements *)(p->record.packed.values))->physCellId==0);
}

static inline gs_retval_t get_SgNB_ack_for_ue_measurements__rsrq(struct packet *p, gs_uint32_t *t){
	*t = ((struct _SgNB_ack_for_ue_measurements *)(p->record.packed.values))->rsrq;
	return (((struct _SgNB_ack_for_ue_measurements *)(p->record.packed.values))->rsrq==0);
}

static inline gs_retval_t get_SgNB_ack_for_ue_measurements__rsrp(struct packet *p, gs_uint32_t *t){
	*t = ((struct _SgNB_ack_for_ue_measurements *)(p->record.packed.values))->rsrp;
	return (((struct _SgNB_ack_for_ue_measurements *)(p->record.packed.values))->rsrp==0);
}

static inline gs_retval_t get_SgNB_ack_for_ue_measurements__sinr(struct packet *p, gs_uint32_t *t){
	*t = ((struct _SgNB_ack_for_ue_measurements *)(p->record.packed.values))->sinr;
	return (((struct _SgNB_ack_for_ue_measurements *)(p->record.packed.values))->sinr==0);
}

#endif
