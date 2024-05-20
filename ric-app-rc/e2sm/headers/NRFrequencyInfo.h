/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "E2SM-COMMON-IEs"
 * 	found in "e2sm-common.asn"
 * 	`asn1c -fcompound-names -fno-include-deps -findirect-choice -pdu=auto -gen-PER -gen-OER -no-gen-example -D .`
 */

#ifndef	_NRFrequencyInfo_H_
#define	_NRFrequencyInfo_H_


#include <asn_application.h>

/* Including external dependencies */
#include "NR-ARFCN.h"
#include "NRFrequencyBand-List.h"
#include "NRFrequencyShift7p5khz.h"
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* NRFrequencyInfo */
typedef struct NRFrequencyInfo {
	NR_ARFCN_t	 nrARFCN;
	NRFrequencyBand_List_t	 frequencyBand_List;
	NRFrequencyShift7p5khz_t	*frequencyShift7p5khz;	/* OPTIONAL */
	/*
	 * This type is extensible,
	 * possible extensions are below.
	 */
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} NRFrequencyInfo_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_NRFrequencyInfo;
extern asn_SEQUENCE_specifics_t asn_SPC_NRFrequencyInfo_specs_1;
extern asn_TYPE_member_t asn_MBR_NRFrequencyInfo_1[3];

#ifdef __cplusplus
}
#endif

#endif	/* _NRFrequencyInfo_H_ */
#include <asn_internal.h>
