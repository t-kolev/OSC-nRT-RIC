/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "E2SM-RC-IEs"
 * 	found in "e2sm-rc-2.0.asn"
 * 	`asn1c -fcompound-names -fno-include-deps -findirect-choice -pdu=auto -gen-PER -gen-OER -no-gen-example -D .`
 */

#ifndef	_EventTrigger_UEevent_Info_Item_H_
#define	_EventTrigger_UEevent_Info_Item_H_


#include <asn_application.h>

/* Including external dependencies */
#include "RIC-EventTrigger-UEevent-ID.h"
#include "LogicalOR.h"
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* EventTrigger-UEevent-Info-Item */
typedef struct EventTrigger_UEevent_Info_Item {
	RIC_EventTrigger_UEevent_ID_t	 ueEventID;
	LogicalOR_t	*logicalOR;	/* OPTIONAL */
	/*
	 * This type is extensible,
	 * possible extensions are below.
	 */
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} EventTrigger_UEevent_Info_Item_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_EventTrigger_UEevent_Info_Item;
extern asn_SEQUENCE_specifics_t asn_SPC_EventTrigger_UEevent_Info_Item_specs_1;
extern asn_TYPE_member_t asn_MBR_EventTrigger_UEevent_Info_Item_1[2];

#ifdef __cplusplus
}
#endif

#endif	/* _EventTrigger_UEevent_Info_Item_H_ */
#include <asn_internal.h>
