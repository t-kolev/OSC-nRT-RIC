#ifndef _RECONFIG_SUCCESS_OLD_H__INCLUDED_
#define _RECONFIG_SUCCESS_OLD_H__INCLUDED_

#include "packet.h"

struct _reconfig_success_old {
	gs_uint64_t timestamp_ms;
	gs_sp_t gnb_id;
	gs_int64_t id_MeNB_UE_X2AP_ID;
	gs_int64_t id_SgNB_UE_X2AP_ID;
	gs_uint32_t id_MeNB_UE_X2AP_ID_Extension;
};

static inline void init__reconfig_success_old(struct _reconfig_success_old *m){
}

static inline gs_retval_t get_reconfig_success_old__timestamp_ms(struct packet *p, gs_uint64_t *t){
	*t = ((struct _reconfig_success_old *)(p->record.packed.values))->timestamp_ms;
	return 0;
}

static inline gs_retval_t get_reconfig_success_old__gnb_id(struct packet *p, struct gs_string *t){
t->data = ((struct _reconfig_success_old *)(p->record.packed.values))->gnb_id;
	t->length = strlen(t->data);
	t->owner=0;
	return 0;
}

static inline gs_retval_t get_reconfig_success_old__id_MeNB_UE_X2AP_ID(struct packet *p, gs_int64_t *t){
	*t = ((struct _reconfig_success_old *)(p->record.packed.values))->id_MeNB_UE_X2AP_ID;
	return 0;
}

static inline gs_retval_t get_reconfig_success_old__id_SgNB_UE_X2AP_ID(struct packet *p, gs_int64_t *t){
	*t = ((struct _reconfig_success_old *)(p->record.packed.values))->id_SgNB_UE_X2AP_ID;
	return 0;
}

static inline gs_retval_t get_reconfig_success_old__id_MeNB_UE_X2AP_ID_Extension(struct packet *p, gs_uint32_t *t){
	*t = ((struct _reconfig_success_old *)(p->record.packed.values))->id_MeNB_UE_X2AP_ID_Extension;
	return 0;
}

#endif
