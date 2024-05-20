#ifndef _SERV_CELL_BEAM_SSB_H__INCLUDED_
#define _SERV_CELL_BEAM_SSB_H__INCLUDED_

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

struct _serv_cell_beam_ssb {
	gs_uint64_t timestamp_ms;
	gs_sp_t gnb_id;
	gs_int64_t id_MeNB_UE_X2AP_ID;
	gs_int64_t id_SgNB_UE_X2AP_ID;
	gs_int64_t servCellID;
	gs_uint32_t physCellId;
	gs_int8_t physCellId_exists;
	gs_int64_t ssb_Index;
	gs_uint32_t rsrq;
	gs_int8_t rsrq_exists;
	gs_uint32_t rsrp;
	gs_int8_t rsrp_exists;
	gs_uint32_t sinr;
	gs_int8_t sinr_exists;
};

static inline void init__serv_cell_beam_ssb(struct _serv_cell_beam_ssb *m){
	m->physCellId_exists=0;
	m->rsrq_exists=0;
	m->rsrp_exists=0;
	m->sinr_exists=0;
}

static inline gs_retval_t get_serv_cell_beam_ssb__timestamp_ms(struct packet *p, gs_uint64_t *t){
	*t = ((struct _serv_cell_beam_ssb *)(p->record.packed.values))->timestamp_ms;
	return 0;
}

static inline gs_retval_t get_serv_cell_beam_ssb__gnb_id(struct packet *p, struct gs_string *t){
	t->owner=0;
	t->data = ((struct _serv_cell_beam_ssb *)(p->record.packed.values))->gnb_id;
	if( t->data == NULL){
		t->length=0;
		return 0;
	}
	t->length = strlen(t->data);
}

static inline gs_retval_t get_serv_cell_beam_ssb__id_MeNB_UE_X2AP_ID(struct packet *p, gs_int64_t *t){
	*t = ((struct _serv_cell_beam_ssb *)(p->record.packed.values))->id_MeNB_UE_X2AP_ID;
	return 0;
}

static inline gs_retval_t get_serv_cell_beam_ssb__id_SgNB_UE_X2AP_ID(struct packet *p, gs_int64_t *t){
	*t = ((struct _serv_cell_beam_ssb *)(p->record.packed.values))->id_SgNB_UE_X2AP_ID;
	return 0;
}

static inline gs_retval_t get_serv_cell_beam_ssb__servCellID(struct packet *p, gs_int64_t *t){
	*t = ((struct _serv_cell_beam_ssb *)(p->record.packed.values))->servCellID;
	return 0;
}

static inline gs_retval_t get_serv_cell_beam_ssb__physCellId(struct packet *p, gs_uint32_t *t){
	*t = ((struct _serv_cell_beam_ssb *)(p->record.packed.values))->physCellId;
	return (((struct _serv_cell_beam_ssb *)(p->record.packed.values))->physCellId==0);
}

static inline gs_retval_t get_serv_cell_beam_ssb__ssb_Index(struct packet *p, gs_int64_t *t){
	*t = ((struct _serv_cell_beam_ssb *)(p->record.packed.values))->ssb_Index;
	return 0;
}

static inline gs_retval_t get_serv_cell_beam_ssb__rsrq(struct packet *p, gs_uint32_t *t){
	*t = ((struct _serv_cell_beam_ssb *)(p->record.packed.values))->rsrq;
	return (((struct _serv_cell_beam_ssb *)(p->record.packed.values))->rsrq==0);
}

static inline gs_retval_t get_serv_cell_beam_ssb__rsrp(struct packet *p, gs_uint32_t *t){
	*t = ((struct _serv_cell_beam_ssb *)(p->record.packed.values))->rsrp;
	return (((struct _serv_cell_beam_ssb *)(p->record.packed.values))->rsrp==0);
}

static inline gs_retval_t get_serv_cell_beam_ssb__sinr(struct packet *p, gs_uint32_t *t){
	*t = ((struct _serv_cell_beam_ssb *)(p->record.packed.values))->sinr;
	return (((struct _serv_cell_beam_ssb *)(p->record.packed.values))->sinr==0);
}

#endif
