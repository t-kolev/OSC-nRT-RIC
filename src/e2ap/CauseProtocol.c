/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "E2AP-IEs"
 * 	found in "e2ap-oran-wg3-v01.00.asn"
 * 	`asn1c -fno-include-deps -fcompound-names -findirect-choice -gen-PER -no-gen-OER`
 */

#include "CauseProtocol.h"

/*
 * This type is implemented using NativeEnumerated,
 * so here we adjust the DEF accordingly.
 */
asn_per_constraints_t asn_PER_type_CauseProtocol_constr_1 CC_NOTUSED = {
	{ APC_CONSTRAINED | APC_EXTENSIBLE,  3,  3,  0,  6 }	/* (0..6,...) */,
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 },
	0, 0	/* No PER value map */
};
static const asn_INTEGER_enum_map_t asn_MAP_CauseProtocol_value2enum_1[] = {
	{ 0,	21,	"transfer-syntax-error" },
	{ 1,	28,	"abstract-syntax-error-reject" },
	{ 2,	39,	"abstract-syntax-error-ignore-and-notify" },
	{ 3,	42,	"message-not-compatible-with-receiver-state" },
	{ 4,	14,	"semantic-error" },
	{ 5,	49,	"abstract-syntax-error-falsely-constructed-message" },
	{ 6,	11,	"unspecified" }
	/* This list is extensible */
};
static const unsigned int asn_MAP_CauseProtocol_enum2value_1[] = {
	5,	/* abstract-syntax-error-falsely-constructed-message(5) */
	2,	/* abstract-syntax-error-ignore-and-notify(2) */
	1,	/* abstract-syntax-error-reject(1) */
	3,	/* message-not-compatible-with-receiver-state(3) */
	4,	/* semantic-error(4) */
	0,	/* transfer-syntax-error(0) */
	6	/* unspecified(6) */
	/* This list is extensible */
};
const asn_INTEGER_specifics_t asn_SPC_CauseProtocol_specs_1 = {
	asn_MAP_CauseProtocol_value2enum_1,	/* "tag" => N; sorted by tag */
	asn_MAP_CauseProtocol_enum2value_1,	/* N => "tag"; sorted by N */
	7,	/* Number of elements in the maps */
	8,	/* Extensions before this member */
	1,	/* Strict enumeration */
	0,	/* Native long size */
	0
};
static const ber_tlv_tag_t asn_DEF_CauseProtocol_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (10 << 2))
};
asn_TYPE_descriptor_t asn_DEF_CauseProtocol = {
	"CauseProtocol",
	"CauseProtocol",
	&asn_OP_NativeEnumerated,
	asn_DEF_CauseProtocol_tags_1,
	sizeof(asn_DEF_CauseProtocol_tags_1)
		/sizeof(asn_DEF_CauseProtocol_tags_1[0]), /* 1 */
	asn_DEF_CauseProtocol_tags_1,	/* Same as above */
	sizeof(asn_DEF_CauseProtocol_tags_1)
		/sizeof(asn_DEF_CauseProtocol_tags_1[0]), /* 1 */
	{ 0, &asn_PER_type_CauseProtocol_constr_1, NativeEnumerated_constraint },
	0, 0,	/* Defined elsewhere */
	&asn_SPC_CauseProtocol_specs_1	/* Additional specs */
};

