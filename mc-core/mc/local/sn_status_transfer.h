#ifndef _SN_STATUS_TRANSFER_H__INCLUDED_
#define _SN_STATUS_TRANSFER_H__INCLUDED_

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

struct _sn_status_transfer {
	gs_uint64_t timestamp_ms;
	gs_sp_t gnb_id;
	gs_int64_t id_Old_eNB_UE_X2AP_ID;
	gs_uint32_t id_SgNB_UE_X2AP_ID;
	gs_int64_t e_RAB_ID;
	gs_int64_t pDCP_SNlength18;
};

static inline void init__sn_status_transfer(struct _sn_status_transfer *m){
}

static inline gs_retval_t get_sn_status_transfer__timestamp_ms(struct packet *p, gs_uint64_t *t){
	*t = ((struct _sn_status_transfer *)(p->record.packed.values))->timestamp_ms;
	return 0;
}

static inline gs_retval_t get_sn_status_transfer__gnb_id(struct packet *p, struct gs_string *t){
	t->owner=0;
	t->data = ((struct _sn_status_transfer *)(p->record.packed.values))->gnb_id;
	if( t->data == NULL){
		t->length=0;
		return 0;
	}
	t->length = strlen(t->data);
}

static inline gs_retval_t get_sn_status_transfer__id_Old_eNB_UE_X2AP_ID(struct packet *p, gs_int64_t *t){
	*t = ((struct _sn_status_transfer *)(p->record.packed.values))->id_Old_eNB_UE_X2AP_ID;
	return 0;
}

static inline gs_retval_t get_sn_status_transfer__id_SgNB_UE_X2AP_ID(struct packet *p, gs_uint32_t *t){
	*t = ((struct _sn_status_transfer *)(p->record.packed.values))->id_SgNB_UE_X2AP_ID;
	return 0;
}

static inline gs_retval_t get_sn_status_transfer__e_RAB_ID(struct packet *p, gs_int64_t *t){
	*t = ((struct _sn_status_transfer *)(p->record.packed.values))->e_RAB_ID;
	return 0;
}

static inline gs_retval_t get_sn_status_transfer__pDCP_SNlength18(struct packet *p, gs_int64_t *t){
	*t = ((struct _sn_status_transfer *)(p->record.packed.values))->pDCP_SNlength18;
	return 0;
}

#endif
