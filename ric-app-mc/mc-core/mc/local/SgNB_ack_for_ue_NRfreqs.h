#ifndef _SGNB_ACK_FOR_UE_NRFREQS_H__INCLUDED_
#define _SGNB_ACK_FOR_UE_NRFREQS_H__INCLUDED_

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

struct _SgNB_ack_for_ue_NRfreqs {
	gs_uint64_t timestamp_ms;
	gs_sp_t gnb_id;
	gs_int64_t id_MeNB_UE_X2AP_ID;
	gs_int64_t id_SgNB_UE_X2AP_ID;
	gs_uint32_t id_MeNB_UE_X2AP_ID_Extension;
	gs_uint32_t measuredFrequenciesSN0;
	gs_int8_t measuredFrequenciesSN0_exists;
	gs_uint32_t measuredFrequenciesSN1;
	gs_int8_t measuredFrequenciesSN1_exists;
	gs_uint32_t measuredFrequenciesSN2;
	gs_int8_t measuredFrequenciesSN2_exists;
	gs_uint32_t measuredFrequenciesSN3;
	gs_int8_t measuredFrequenciesSN3_exists;
	gs_uint32_t measuredFrequenciesSN4;
	gs_int8_t measuredFrequenciesSN4_exists;
	gs_uint32_t measuredFrequenciesSN5;
	gs_int8_t measuredFrequenciesSN5_exists;
	gs_uint32_t measuredFrequenciesSN6;
	gs_int8_t measuredFrequenciesSN6_exists;
	gs_uint32_t measuredFrequenciesSN7;
	gs_int8_t measuredFrequenciesSN7_exists;
	gs_int64_t candidate_serving_cell_freqs0;
	gs_int8_t candidate_serving_cell_freqs0_exists;
	gs_int64_t candidate_serving_cell_freqs1;
	gs_int8_t candidate_serving_cell_freqs1_exists;
	gs_int64_t candidate_serving_cell_freqs2;
	gs_int8_t candidate_serving_cell_freqs2_exists;
	gs_int64_t candidate_serving_cell_freqs3;
	gs_int8_t candidate_serving_cell_freqs3_exists;
	gs_int64_t candidate_serving_cell_freqs4;
	gs_int8_t candidate_serving_cell_freqs4_exists;
	gs_int64_t candidate_serving_cell_freqs5;
	gs_int8_t candidate_serving_cell_freqs5_exists;
	gs_int64_t candidate_serving_cell_freqs6;
	gs_int8_t candidate_serving_cell_freqs6_exists;
	gs_int64_t candidate_serving_cell_freqs7;
	gs_int8_t candidate_serving_cell_freqs7_exists;
};

static inline void init__SgNB_ack_for_ue_NRfreqs(struct _SgNB_ack_for_ue_NRfreqs *m){
	m->measuredFrequenciesSN0_exists=0;
	m->measuredFrequenciesSN1_exists=0;
	m->measuredFrequenciesSN2_exists=0;
	m->measuredFrequenciesSN3_exists=0;
	m->measuredFrequenciesSN4_exists=0;
	m->measuredFrequenciesSN5_exists=0;
	m->measuredFrequenciesSN6_exists=0;
	m->measuredFrequenciesSN7_exists=0;
	m->candidate_serving_cell_freqs0_exists=0;
	m->candidate_serving_cell_freqs1_exists=0;
	m->candidate_serving_cell_freqs2_exists=0;
	m->candidate_serving_cell_freqs3_exists=0;
	m->candidate_serving_cell_freqs4_exists=0;
	m->candidate_serving_cell_freqs5_exists=0;
	m->candidate_serving_cell_freqs6_exists=0;
	m->candidate_serving_cell_freqs7_exists=0;
}

static inline gs_retval_t get_SgNB_ack_for_ue_NRfreqs__timestamp_ms(struct packet *p, gs_uint64_t *t){
	*t = ((struct _SgNB_ack_for_ue_NRfreqs *)(p->record.packed.values))->timestamp_ms;
	return 0;
}

static inline gs_retval_t get_SgNB_ack_for_ue_NRfreqs__gnb_id(struct packet *p, struct gs_string *t){
	t->owner=0;
	t->data = ((struct _SgNB_ack_for_ue_NRfreqs *)(p->record.packed.values))->gnb_id;
	if( t->data == NULL){
		t->length=0;
		return 0;
	}
	t->length = strlen(t->data);
}

static inline gs_retval_t get_SgNB_ack_for_ue_NRfreqs__id_MeNB_UE_X2AP_ID(struct packet *p, gs_int64_t *t){
	*t = ((struct _SgNB_ack_for_ue_NRfreqs *)(p->record.packed.values))->id_MeNB_UE_X2AP_ID;
	return 0;
}

static inline gs_retval_t get_SgNB_ack_for_ue_NRfreqs__id_SgNB_UE_X2AP_ID(struct packet *p, gs_int64_t *t){
	*t = ((struct _SgNB_ack_for_ue_NRfreqs *)(p->record.packed.values))->id_SgNB_UE_X2AP_ID;
	return 0;
}

static inline gs_retval_t get_SgNB_ack_for_ue_NRfreqs__id_MeNB_UE_X2AP_ID_Extension(struct packet *p, gs_uint32_t *t){
	*t = ((struct _SgNB_ack_for_ue_NRfreqs *)(p->record.packed.values))->id_MeNB_UE_X2AP_ID_Extension;
	return 0;
}

static inline gs_retval_t get_SgNB_ack_for_ue_NRfreqs__measuredFrequenciesSN0(struct packet *p, gs_uint32_t *t){
	*t = ((struct _SgNB_ack_for_ue_NRfreqs *)(p->record.packed.values))->measuredFrequenciesSN0;
	return (((struct _SgNB_ack_for_ue_NRfreqs *)(p->record.packed.values))->measuredFrequenciesSN0==0);
}

static inline gs_retval_t get_SgNB_ack_for_ue_NRfreqs__measuredFrequenciesSN1(struct packet *p, gs_uint32_t *t){
	*t = ((struct _SgNB_ack_for_ue_NRfreqs *)(p->record.packed.values))->measuredFrequenciesSN1;
	return (((struct _SgNB_ack_for_ue_NRfreqs *)(p->record.packed.values))->measuredFrequenciesSN1==0);
}

static inline gs_retval_t get_SgNB_ack_for_ue_NRfreqs__measuredFrequenciesSN2(struct packet *p, gs_uint32_t *t){
	*t = ((struct _SgNB_ack_for_ue_NRfreqs *)(p->record.packed.values))->measuredFrequenciesSN2;
	return (((struct _SgNB_ack_for_ue_NRfreqs *)(p->record.packed.values))->measuredFrequenciesSN2==0);
}

static inline gs_retval_t get_SgNB_ack_for_ue_NRfreqs__measuredFrequenciesSN3(struct packet *p, gs_uint32_t *t){
	*t = ((struct _SgNB_ack_for_ue_NRfreqs *)(p->record.packed.values))->measuredFrequenciesSN3;
	return (((struct _SgNB_ack_for_ue_NRfreqs *)(p->record.packed.values))->measuredFrequenciesSN3==0);
}

static inline gs_retval_t get_SgNB_ack_for_ue_NRfreqs__measuredFrequenciesSN4(struct packet *p, gs_uint32_t *t){
	*t = ((struct _SgNB_ack_for_ue_NRfreqs *)(p->record.packed.values))->measuredFrequenciesSN4;
	return (((struct _SgNB_ack_for_ue_NRfreqs *)(p->record.packed.values))->measuredFrequenciesSN4==0);
}

static inline gs_retval_t get_SgNB_ack_for_ue_NRfreqs__measuredFrequenciesSN5(struct packet *p, gs_uint32_t *t){
	*t = ((struct _SgNB_ack_for_ue_NRfreqs *)(p->record.packed.values))->measuredFrequenciesSN5;
	return (((struct _SgNB_ack_for_ue_NRfreqs *)(p->record.packed.values))->measuredFrequenciesSN5==0);
}

static inline gs_retval_t get_SgNB_ack_for_ue_NRfreqs__measuredFrequenciesSN6(struct packet *p, gs_uint32_t *t){
	*t = ((struct _SgNB_ack_for_ue_NRfreqs *)(p->record.packed.values))->measuredFrequenciesSN6;
	return (((struct _SgNB_ack_for_ue_NRfreqs *)(p->record.packed.values))->measuredFrequenciesSN6==0);
}

static inline gs_retval_t get_SgNB_ack_for_ue_NRfreqs__measuredFrequenciesSN7(struct packet *p, gs_uint32_t *t){
	*t = ((struct _SgNB_ack_for_ue_NRfreqs *)(p->record.packed.values))->measuredFrequenciesSN7;
	return (((struct _SgNB_ack_for_ue_NRfreqs *)(p->record.packed.values))->measuredFrequenciesSN7==0);
}

static inline gs_retval_t get_SgNB_ack_for_ue_NRfreqs__candidate_serving_cell_freqs0(struct packet *p, gs_int64_t *t){
	*t = ((struct _SgNB_ack_for_ue_NRfreqs *)(p->record.packed.values))->candidate_serving_cell_freqs0;
	return (((struct _SgNB_ack_for_ue_NRfreqs *)(p->record.packed.values))->candidate_serving_cell_freqs0==0);
}

static inline gs_retval_t get_SgNB_ack_for_ue_NRfreqs__candidate_serving_cell_freqs1(struct packet *p, gs_int64_t *t){
	*t = ((struct _SgNB_ack_for_ue_NRfreqs *)(p->record.packed.values))->candidate_serving_cell_freqs1;
	return (((struct _SgNB_ack_for_ue_NRfreqs *)(p->record.packed.values))->candidate_serving_cell_freqs1==0);
}

static inline gs_retval_t get_SgNB_ack_for_ue_NRfreqs__candidate_serving_cell_freqs2(struct packet *p, gs_int64_t *t){
	*t = ((struct _SgNB_ack_for_ue_NRfreqs *)(p->record.packed.values))->candidate_serving_cell_freqs2;
	return (((struct _SgNB_ack_for_ue_NRfreqs *)(p->record.packed.values))->candidate_serving_cell_freqs2==0);
}

static inline gs_retval_t get_SgNB_ack_for_ue_NRfreqs__candidate_serving_cell_freqs3(struct packet *p, gs_int64_t *t){
	*t = ((struct _SgNB_ack_for_ue_NRfreqs *)(p->record.packed.values))->candidate_serving_cell_freqs3;
	return (((struct _SgNB_ack_for_ue_NRfreqs *)(p->record.packed.values))->candidate_serving_cell_freqs3==0);
}

static inline gs_retval_t get_SgNB_ack_for_ue_NRfreqs__candidate_serving_cell_freqs4(struct packet *p, gs_int64_t *t){
	*t = ((struct _SgNB_ack_for_ue_NRfreqs *)(p->record.packed.values))->candidate_serving_cell_freqs4;
	return (((struct _SgNB_ack_for_ue_NRfreqs *)(p->record.packed.values))->candidate_serving_cell_freqs4==0);
}

static inline gs_retval_t get_SgNB_ack_for_ue_NRfreqs__candidate_serving_cell_freqs5(struct packet *p, gs_int64_t *t){
	*t = ((struct _SgNB_ack_for_ue_NRfreqs *)(p->record.packed.values))->candidate_serving_cell_freqs5;
	return (((struct _SgNB_ack_for_ue_NRfreqs *)(p->record.packed.values))->candidate_serving_cell_freqs5==0);
}

static inline gs_retval_t get_SgNB_ack_for_ue_NRfreqs__candidate_serving_cell_freqs6(struct packet *p, gs_int64_t *t){
	*t = ((struct _SgNB_ack_for_ue_NRfreqs *)(p->record.packed.values))->candidate_serving_cell_freqs6;
	return (((struct _SgNB_ack_for_ue_NRfreqs *)(p->record.packed.values))->candidate_serving_cell_freqs6==0);
}

static inline gs_retval_t get_SgNB_ack_for_ue_NRfreqs__candidate_serving_cell_freqs7(struct packet *p, gs_int64_t *t){
	*t = ((struct _SgNB_ack_for_ue_NRfreqs *)(p->record.packed.values))->candidate_serving_cell_freqs7;
	return (((struct _SgNB_ack_for_ue_NRfreqs *)(p->record.packed.values))->candidate_serving_cell_freqs7==0);
}

#endif
