#ifndef _RECONFIG_SUCCESS_OLD2_H__INCLUDED_
#define _RECONFIG_SUCCESS_OLD2_H__INCLUDED_

#include "packet.h"

struct _reconfig_success_old2 {
	gs_uint64_t timestamp_ms;
	gs_sp_t gnb_id;
	gs_int64_t cause_protocol;
	gs_int64_t id_MeNB_UE_X2AP_ID;
	gs_int64_t cause_transport;
	gs_uint32_t id_MeNB_UE_X2AP_ID_Extension;
	gs_int64_t cause_radio_network;
	gs_int64_t id_SgNB_UE_X2AP_ID;
	gs_int64_t cause_misc;
};

static inline void init__reconfig_success_old2(struct _reconfig_success_old2 *m){
}

static inline gs_retval_t get_reconfig_success_old2__timestamp_ms(struct packet *p, gs_uint64_t *t){
	*t = ((struct _reconfig_success_old2 *)(p->record.packed.values))->timestamp_ms;
	return 0;
}

static inline gs_retval_t get_reconfig_success_old2__gnb_id(struct packet *p, struct gs_string *t){
t->data = ((struct _reconfig_success_old2 *)(p->record.packed.values))->gnb_id;
	t->length = strlen(t->data);
	t->owner=0;
	return 0;
}

static inline gs_retval_t get_reconfig_success_old2__cause_protocol(struct packet *p, gs_int64_t *t){
	*t = ((struct _reconfig_success_old2 *)(p->record.packed.values))->cause_protocol;
	return 0;
}

static inline gs_retval_t get_reconfig_success_old2__id_MeNB_UE_X2AP_ID(struct packet *p, gs_int64_t *t){
	*t = ((struct _reconfig_success_old2 *)(p->record.packed.values))->id_MeNB_UE_X2AP_ID;
	return 0;
}

static inline gs_retval_t get_reconfig_success_old2__cause_transport(struct packet *p, gs_int64_t *t){
	*t = ((struct _reconfig_success_old2 *)(p->record.packed.values))->cause_transport;
	return 0;
}

static inline gs_retval_t get_reconfig_success_old2__id_MeNB_UE_X2AP_ID_Extension(struct packet *p, gs_uint32_t *t){
	*t = ((struct _reconfig_success_old2 *)(p->record.packed.values))->id_MeNB_UE_X2AP_ID_Extension;
	return 0;
}

static inline gs_retval_t get_reconfig_success_old2__cause_radio_network(struct packet *p, gs_int64_t *t){
	*t = ((struct _reconfig_success_old2 *)(p->record.packed.values))->cause_radio_network;
	return 0;
}

static inline gs_retval_t get_reconfig_success_old2__id_SgNB_UE_X2AP_ID(struct packet *p, gs_int64_t *t){
	*t = ((struct _reconfig_success_old2 *)(p->record.packed.values))->id_SgNB_UE_X2AP_ID;
	return 0;
}

static inline gs_retval_t get_reconfig_success_old2__cause_misc(struct packet *p, gs_int64_t *t){
	*t = ((struct _reconfig_success_old2 *)(p->record.packed.values))->cause_misc;
	return 0;
}

#endif
