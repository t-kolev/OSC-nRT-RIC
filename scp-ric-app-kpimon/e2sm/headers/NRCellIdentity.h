/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "E2SM-KPM-IEs"
 * 	found in "e2sm-kpm-v01.02.asn"
 * 	`asn1c -pdu=auto -fno-include-deps -fcompound-names -findirect-choice -gen-PER -gen-OER -no-gen-example`
 */

#ifndef	_NRCellIdentity_H_
#define	_NRCellIdentity_H_


#include <asn_application.h>

/* Including external dependencies */
#include <BIT_STRING.h>

#ifdef __cplusplus
extern "C" {
#endif

/* NRCellIdentity */
typedef BIT_STRING_t	 NRCellIdentity_t;

/* Implementation */
extern asn_per_constraints_t asn_PER_type_NRCellIdentity_constr_1;
extern asn_TYPE_descriptor_t asn_DEF_NRCellIdentity;
asn_struct_free_f NRCellIdentity_free;
asn_struct_print_f NRCellIdentity_print;
asn_constr_check_f NRCellIdentity_constraint;
ber_type_decoder_f NRCellIdentity_decode_ber;
der_type_encoder_f NRCellIdentity_encode_der;
xer_type_decoder_f NRCellIdentity_decode_xer;
xer_type_encoder_f NRCellIdentity_encode_xer;
oer_type_decoder_f NRCellIdentity_decode_oer;
oer_type_encoder_f NRCellIdentity_encode_oer;
per_type_decoder_f NRCellIdentity_decode_uper;
per_type_encoder_f NRCellIdentity_encode_uper;
per_type_decoder_f NRCellIdentity_decode_aper;
per_type_encoder_f NRCellIdentity_encode_aper;

#ifdef __cplusplus
}
#endif

#endif	/* _NRCellIdentity_H_ */
#include <asn_internal.h>
