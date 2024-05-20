/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "E2SM-COMMON-IEs"
 * 	found in "e2sm-common.asn"
 * 	`asn1c -fcompound-names -fno-include-deps -findirect-choice -pdu=auto -gen-PER -gen-OER -no-gen-example -D .`
 */

#include "QoSID.h"

static asn_oer_constraints_t asn_OER_type_QoSID_constr_1 CC_NOTUSED = {
	{ 0, 0 },
	-1};
static asn_per_constraints_t asn_PER_type_QoSID_constr_1 CC_NOTUSED = {
	{ APC_CONSTRAINED | APC_EXTENSIBLE,  1,  1,  0,  1 }	/* (0..1,...) */,
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 },
	0, 0	/* No PER value map */
};
static asn_TYPE_member_t asn_MBR_QoSID_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct QoSID, choice.fiveGC),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_FiveQI,
		0,
		{ 0, 0, 0 },
		0, 0, /* No default value */
		"fiveGC"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct QoSID, choice.ePC),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_QCI,
		0,
		{ 0, 0, 0 },
		0, 0, /* No default value */
		"ePC"
		},
};
static const asn_TYPE_tag2member_t asn_MAP_QoSID_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* fiveGC */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 } /* ePC */
};
static asn_CHOICE_specifics_t asn_SPC_QoSID_specs_1 = {
	sizeof(struct QoSID),
	offsetof(struct QoSID, _asn_ctx),
	offsetof(struct QoSID, present),
	sizeof(((struct QoSID *)0)->present),
	asn_MAP_QoSID_tag2el_1,
	2,	/* Count of tags in the map */
	0, 0,
	2	/* Extensions start */
};
asn_TYPE_descriptor_t asn_DEF_QoSID = {
	"QoSID",
	"QoSID",
	&asn_OP_CHOICE,
	0,	/* No effective tags (pointer) */
	0,	/* No effective tags (count) */
	0,	/* No tags (pointer) */
	0,	/* No tags (count) */
	{ &asn_OER_type_QoSID_constr_1, &asn_PER_type_QoSID_constr_1, CHOICE_constraint },
	asn_MBR_QoSID_1,
	2,	/* Elements count */
	&asn_SPC_QoSID_specs_1	/* Additional specs */
};
