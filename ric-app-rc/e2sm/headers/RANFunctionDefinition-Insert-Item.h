/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "E2SM-RC-IEs"
 * 	found in "e2sm-rc-2.0.asn"
 * 	`asn1c -fcompound-names -fno-include-deps -findirect-choice -pdu=auto -gen-PER -gen-OER -no-gen-example -D .`
 */

#ifndef	_RANFunctionDefinition_Insert_Item_H_
#define	_RANFunctionDefinition_Insert_Item_H_


#include <asn_application.h>

/* Including external dependencies */
#include "RIC-Style-Type.h"
#include "RIC-Style-Name.h"
#include "RIC-Format-Type.h"
#include <asn_SEQUENCE_OF.h>
#include <constr_SEQUENCE_OF.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct RANFunctionDefinition_Insert_Indication_Item;

/* RANFunctionDefinition-Insert-Item */
typedef struct RANFunctionDefinition_Insert_Item {
	RIC_Style_Type_t	 ric_InsertStyle_Type;
	RIC_Style_Name_t	 ric_InsertStyle_Name;
	RIC_Style_Type_t	 ric_SupportedEventTriggerStyle_Type;
	RIC_Format_Type_t	 ric_ActionDefinitionFormat_Type;
	struct RANFunctionDefinition_Insert_Item__ric_InsertIndication_List {
		A_SEQUENCE_OF(struct RANFunctionDefinition_Insert_Indication_Item) list;
		
		/* Context for parsing across buffer boundaries */
		asn_struct_ctx_t _asn_ctx;
	} *ric_InsertIndication_List;
	RIC_Format_Type_t	 ric_IndicationHeaderFormat_Type;
	RIC_Format_Type_t	 ric_IndicationMessageFormat_Type;
	RIC_Format_Type_t	 ric_CallProcessIDFormat_Type;
	/*
	 * This type is extensible,
	 * possible extensions are below.
	 */
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} RANFunctionDefinition_Insert_Item_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_RANFunctionDefinition_Insert_Item;
extern asn_SEQUENCE_specifics_t asn_SPC_RANFunctionDefinition_Insert_Item_specs_1;
extern asn_TYPE_member_t asn_MBR_RANFunctionDefinition_Insert_Item_1[8];

#ifdef __cplusplus
}
#endif

#endif	/* _RANFunctionDefinition_Insert_Item_H_ */
#include <asn_internal.h>
