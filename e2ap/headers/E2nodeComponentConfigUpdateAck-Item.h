/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "E2AP-PDU-Contents"
 * 	found in "pduDefinition.asn"
 * 	`asn1c -pdu=auto -fincludes-quoted -fcompound-names -findirect-choice -fno-include-deps -no-gen-example -gen-APER`
 */

#ifndef	_E2nodeComponentConfigUpdateAck_Item_H_
#define	_E2nodeComponentConfigUpdateAck_Item_H_


#include "asn_application.h"

/* Including external dependencies */
#include "E2nodeComponentInterfaceType.h"
#include "E2nodeComponentID.h"
#include "E2nodeComponentConfigurationAck.h"
#include "constr_SEQUENCE.h"

#ifdef __cplusplus
extern "C" {
#endif

/* E2nodeComponentConfigUpdateAck-Item */
typedef struct E2nodeComponentConfigUpdateAck_Item {
	E2nodeComponentInterfaceType_t	 e2nodeComponentInterfaceType;
	E2nodeComponentID_t	 e2nodeComponentID;
	E2nodeComponentConfigurationAck_t	 e2nodeComponentConfigurationAck;
	/*
	 * This type is extensible,
	 * possible extensions are below.
	 */
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} E2nodeComponentConfigUpdateAck_Item_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_E2nodeComponentConfigUpdateAck_Item;
extern asn_SEQUENCE_specifics_t asn_SPC_E2nodeComponentConfigUpdateAck_Item_specs_1;
extern asn_TYPE_member_t asn_MBR_E2nodeComponentConfigUpdateAck_Item_1[3];

#ifdef __cplusplus
}
#endif

#endif	/* _E2nodeComponentConfigUpdateAck_Item_H_ */
#include "asn_internal.h"
