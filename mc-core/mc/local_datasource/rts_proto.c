
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


#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include "errno.h"

#include "gsconfig.h"
#include "gshub.h"
#include "gstypes.h"
#include "lapp.h"
#include "fta.h"
#include "stdio.h"
#include "stdlib.h"
#include "packet.h"
#include "schemaparser.h"
#include "lfta/rts.h"

void rts_fta_process_packet(struct packet * p);
void rts_fta_done();
void fta_init(gs_sp_t device);

gs_uint32_t (*process_buffer)(gs_uint8_t * buffer, gs_uint32_t buflen) = NULL; // set at initialization

#define MAXLINE 1000000

static int fd=-1;
struct pollfd pfd;
static struct packet cur_packet;
static gs_sp_t name, this_device;
static gs_uint32_t verbose=0;
static gs_uint32_t startupdelay=0;
static gs_uint32_t singlefile=0;
static gs_uint32_t fifo=0;
static gs_uint32_t gshub=0;
static int socket_desc=0;

static gs_uint8_t line[MAXLINE];
static gs_uint32_t lineend=0;
static unsigned long long timestamp; // extract from input header

//----------------  Specialized proto parsing -----------
#include "x2ap_streaming.pb-c.h"
#include "ue_context_release.pb-c.h"
#include "lfta/local/dc_release.h"
#include "secondary_rat_data_usage_report.pb-c.h"
#include "lfta/local/rat_data_usage.h"
#include "sgnb_reconfiguration_complete.pb-c.h"
#include "lfta/local/reconfig_all.h"
#include "lfta/local/reconfig_success.h"
#include "lfta/local/reconfig_reject.h"
#include "sgnb_release_confirm.pb-c.h"
#include "lfta/local/sgnb_release_confirm_from_menb_erabs.h"
#include "lfta/local/sgnb_release_confirm_from_menb.h"
#include "sgnb_release_request.pb-c.h"
#include "lfta/local/release_req.h"
#include "sgnb_release_request_acknowledge.pb-c.h"
#include "lfta/local/release_req_ack.h"
#include "sgnb_release_required.pb-c.h"
#include "lfta/local/SgNB_release_rqd.h"
#include "rrctransfer.pb-c.h"
#include "lfta/local/serv_nr_cell.h"
#include "lfta/local/nr_neighbor.h"
#include "lfta/local/serv_cell_beam_csi.h"
#include "lfta/local/neighbor_beam_csi.h"
#include "lfta/local/serv_cell_beam_ssb.h"
#include "lfta/local/neighbor_beam_ssb.h"
#include "sgnb_addition_request_reject.pb-c.h"
#include "lfta/local/sgnb_add_req_reject.h"
#include "sgnb_addition_request_acknowledge.pb-c.h"
#include "lfta/local/eRABs_notadmitted_for_ue.h"
#include "lfta/local/add_req_ack_cellid.h"
#include "lfta/local/eRABs_acked_for_admit_for_ue.h"
#include "lfta/local/SgNB_ack_for_ue_NRfreqs.h"
#include "lfta/local/SgNB_ack_for_add_mod_for_ue.h"
#include "lfta/local/SgNB_ack_for_ue_measurements.h"
#include "lfta/local/SgNB_ack_for_ue_beam_csi.h"
#include "lfta/local/SgNB_ack_for_ue_beam_ssb.h"
#include "sgnb_addition_request.pb-c.h"
#include "lfta/local/sgnb_addreq_gtp_teid.h"
#include "lfta/local/sgnb_addreq_for_ue.h"
#include "lfta/local/sgnb_addreq_for_ue_bearers.h"
#include "lfta/local/sgnb_addreq_for_ue_sn_serv_ssb.h"
#include "lfta/local/sgnb_addreq_for_ue_sn_serv_csi_rs.h"
#include "lfta/local/sgnb_addreq_for_ue_mn_serv_ssb.h"
#include "lfta/local/sgnb_addreq_for_ue_mn_serv_csi_rs.h"
#include "lfta/local/sgnb_addreq_for_ue_sn_neigh_ssb.h"
#include "lfta/local/sgnb_addreq_for_ue_sn_neigh_csi_rs.h"
#include "lfta/local/sgnb_addreq_for_ue_mn_neigh_ssb.h"
#include "lfta/local/sgnb_addreq_for_ue_mn_neigh_csi_rs.h"
#include "sgnb_modification_confirm.pb-c.h"
#include "lfta/local/sgnb_mod_conf.h"
#include "sgnb_modification_request.pb-c.h"
#include "lfta/local/sgnb_mod_req.h"
#include "sgnb_modification_request_acknowledge.pb-c.h"
#include "lfta/local/sgnb_mod_req_ack.h"
#include "sgnb_modification_request_reject.pb-c.h"
#include "lfta/local/sgnb_mod_req_reject.h"
#include "sgnb_modification_required.pb-c.h"
#include "lfta/local/sgnb_mod_required.h"
#include "sgnb_modification_refuse.pb-c.h"
#include "lfta/local/sgnb_mod_refuse.h"
#include "sn_status_transfer.pb-c.h"
#include "lfta/local/sn_status_transfer.h"
gs_uint32_t process_buffer_CONRELEASE(gs_uint8_t * buffer, gs_uint32_t buflen){
	char *empty_string = "";
unsigned long long int ts_lo, ts_hi;
	StreamingProtobufs__X2APStreaming *hdr = NULL;
// ------------------------------------------
// ---  Variables for .proto ue_context_release.json, path context_release.json
	struct _dc_release *dc_release = NULL;
	StreamingProtobufs__UEContextRelease *node_0_0 = NULL;

// --------------------------------------------------
// ---  Specialized processing for .proto ue_context_release.json, path context_release.json

	dc_release = (struct _dc_release *)(cur_packet.record.packed.values);
	cur_packet.schema = 201;

	hdr = streaming_protobufs__x2_apstreaming__unpack(NULL, buflen, buffer);
	if(hdr==NULL) return -1;

	node_0_0 = hdr->uecontextrelease;
	if(node_0_0==NULL) return -2;
	if(hdr->header==NULL) return -3;

	ts_lo = hdr->header->timestamp & 0xffffffff;
	ts_hi = hdr->header->timestamp >> 32;
	dc_release->timestamp_ms = (ts_hi - 2208988800) * 1000 + ((ts_lo * 1000) >> 32);
	if(hdr->header->gnbid==NULL)
		dc_release->gnb_id = empty_string;
	else
		dc_release->gnb_id = hdr->header->gnbid->value;

	if(node_0_0->id_old_enb_ue_x2ap_id_extension){
		dc_release->id_Old_eNB_UE_X2AP_ID_Extension = node_0_0->id_old_enb_ue_x2ap_id_extension->value;
	}else{
		dc_release->id_Old_eNB_UE_X2AP_ID_Extension = 0;
	}
	dc_release->id_New_eNB_UE_X2AP_ID = node_0_0->id_new_enb_ue_x2ap_id;
	if(node_0_0->id_sgnb_ue_x2ap_id){
		dc_release->id_SgNB_UE_X2AP_ID = node_0_0->id_sgnb_ue_x2ap_id->value;
	}else{
		dc_release->id_SgNB_UE_X2AP_ID = 0;
	}
	if(node_0_0->id_new_enb_ue_x2ap_id_extension){
		dc_release->id_New_eNB_UE_X2AP_ID_Extension = node_0_0->id_new_enb_ue_x2ap_id_extension->value;
	}else{
		dc_release->id_New_eNB_UE_X2AP_ID_Extension = 0;
	}
	dc_release->id_Old_eNB_UE_X2AP_ID = node_0_0->id_old_enb_ue_x2ap_id;
	rts_fta_process_packet(&cur_packet);
	streaming_protobufs__uecontext_release__free_unpacked(node_0_0,NULL);
	return 0;
}

gs_uint32_t process_buffer_RATDATAUSAGE(gs_uint8_t * buffer, gs_uint32_t buflen){
	char *empty_string = "";
unsigned long long int ts_lo, ts_hi;
	StreamingProtobufs__X2APStreaming *hdr = NULL;
// ------------------------------------------
// ---  Variables for .proto secondary_rat_data_usage_report.json, path rat_data_usage.json
	struct _rat_data_usage *rat_data_usage = NULL;
	StreamingProtobufs__SecondaryRATDataUsageReport *node_0_0 = NULL;
	StreamingProtobufs__SecondaryRATDataUsageReportIEs *node_0_1 = NULL;
	StreamingProtobufs__SecondaryRATUsageReportList *node_0_2 = NULL;
	StreamingProtobufs__SecondaryRATUsageReportItemIEs *node_0_3 = NULL;
	gs_uint32_t i_0_3;
	StreamingProtobufs__SecondaryRATUsageReportItem *node_0_4 = NULL;
	StreamingProtobufs__ERABUsageReportList *node_0_5 = NULL;
	StreamingProtobufs__ERABUsageReportItemIEs *node_0_6 = NULL;
	gs_uint32_t i_0_6;
	StreamingProtobufs__ERABUsageReportItem *node_0_7 = NULL;

// --------------------------------------------------
// ---  Specialized processing for .proto secondary_rat_data_usage_report.json, path rat_data_usage.json

	rat_data_usage = (struct _rat_data_usage *)(cur_packet.record.packed.values);
	cur_packet.schema = 1501;

	hdr = streaming_protobufs__x2_apstreaming__unpack(NULL, buflen, buffer);
	if(hdr==NULL) return -1;

	node_0_0 = hdr->secondaryratdatausagereport;
	if(node_0_0==NULL) return -2;
	if(hdr->header==NULL) return -3;

	ts_lo = hdr->header->timestamp & 0xffffffff;
	ts_hi = hdr->header->timestamp >> 32;
	rat_data_usage->timestamp_ms = (ts_hi - 2208988800) * 1000 + ((ts_lo * 1000) >> 32);
	if(hdr->header->gnbid==NULL)
		rat_data_usage->gnb_id = empty_string;
	else
		rat_data_usage->gnb_id = hdr->header->gnbid->value;

	node_0_1 = node_0_0->protocolies;
	if(node_0_0->protocolies){
		rat_data_usage->id_MeNB_UE_X2AP_ID = node_0_1->id_menb_ue_x2ap_id;
		rat_data_usage->id_SgNB_UE_X2AP_ID = node_0_1->id_sgnb_ue_x2ap_id;
		if(node_0_1->id_menb_ue_x2ap_id_extension){
			rat_data_usage->id_MeNB_UE_X2AP_ID_Extension = node_0_1->id_menb_ue_x2ap_id_extension->value;
		}else{
			rat_data_usage->id_MeNB_UE_X2AP_ID_Extension = 0;
		}
		node_0_2 = node_0_1->id_secondaryratusagereportlist;
		if(node_0_1->id_secondaryratusagereportlist){
			for(i_0_3=0;i_0_3<node_0_2->n_items; i_0_3++){
				node_0_3 = node_0_2->items[i_0_3];
				node_0_4 = node_0_3->id_secondaryratusagereport_item;
				if(node_0_3->id_secondaryratusagereport_item){
					rat_data_usage->e_RAB_ID = node_0_4->e_rab_id;
					rat_data_usage->secondaryRATType = node_0_4->secondaryrattype;
					node_0_5 = node_0_4->e_rabusagereportlist;
					if(node_0_4->e_rabusagereportlist){
						for(i_0_6=0;i_0_6<node_0_5->n_items; i_0_6++){
							node_0_6 = node_0_5->items[i_0_6];
							node_0_7 = node_0_6->id_e_rabusagereport_item;
							if(node_0_6->id_e_rabusagereport_item){
								rat_data_usage->startTimeStamp = node_0_7->starttimestamp;
								rat_data_usage->endTimeStamp = node_0_7->endtimestamp;
								rat_data_usage->usageCountDL = node_0_7->usagecountdl;
								rts_fta_process_packet(&cur_packet);
							}
						}
					}
				}
			}
		}
	}
	streaming_protobufs__secondary_ratdata_usage_report__free_unpacked(node_0_0,NULL);
	return 0;
}

gs_uint32_t process_buffer_RECONCOMPLETE(gs_uint8_t * buffer, gs_uint32_t buflen){
	char *empty_string = "";
unsigned long long int ts_lo, ts_hi;
	StreamingProtobufs__X2APStreaming *hdr = NULL;
// ------------------------------------------
// ---  Variables for .proto sgnb_reconfiguration_complete.json, path recon_complete.json
	struct _reconfig_all *reconfig_all = NULL;
	StreamingProtobufs__SgNBReconfigurationComplete *node_0_0 = NULL;
// ------------------------------------------
// ---  Variables for .proto sgnb_reconfiguration_complete.json, path recon_complete.json
	struct _reconfig_success *reconfig_success = NULL;
	StreamingProtobufs__SgNBReconfigurationComplete *node_1_0 = NULL;
	StreamingProtobufs__ResponseInformationSgNBReconfComp *node_1_1 = NULL;
	StreamingProtobufs__ResponseInformationSgNBReconfCompRejectByMeNBItem *node_1_2 = NULL;
// ------------------------------------------
// ---  Variables for .proto sgnb_reconfiguration_complete.json, path recon_complete.json
	struct _reconfig_reject *reconfig_reject = NULL;
	StreamingProtobufs__SgNBReconfigurationComplete *node_2_0 = NULL;
	StreamingProtobufs__ResponseInformationSgNBReconfComp *node_2_1 = NULL;
	StreamingProtobufs__ResponseInformationSgNBReconfCompRejectByMeNBItem *node_2_2 = NULL;

// --------------------------------------------------
// ---  Specialized processing for .proto sgnb_reconfiguration_complete.json, path recon_complete.json

	reconfig_all = (struct _reconfig_all *)(cur_packet.record.packed.values);
	cur_packet.schema = 103;

	hdr = streaming_protobufs__x2_apstreaming__unpack(NULL, buflen, buffer);
	if(hdr==NULL) return -1;

	node_0_0 = hdr->sgnbreconfigurationcomplete;
	if(node_0_0==NULL) return -2;
	if(hdr->header==NULL) return -3;

	ts_lo = hdr->header->timestamp & 0xffffffff;
	ts_hi = hdr->header->timestamp >> 32;
	reconfig_all->timestamp_ms = (ts_hi - 2208988800) * 1000 + ((ts_lo * 1000) >> 32);
	if(hdr->header->gnbid==NULL)
		reconfig_all->gnb_id = empty_string;
	else
		reconfig_all->gnb_id = hdr->header->gnbid->value;

	reconfig_all->id_MeNB_UE_X2AP_ID = node_0_0->id_menb_ue_x2ap_id;
	reconfig_all->id_SgNB_UE_X2AP_ID = node_0_0->id_sgnb_ue_x2ap_id;
	if(node_0_0->id_menb_ue_x2ap_id_extension){
		reconfig_all->id_MeNB_UE_X2AP_ID_Extension = node_0_0->id_menb_ue_x2ap_id_extension->value;
	}else{
		reconfig_all->id_MeNB_UE_X2AP_ID_Extension = 0;
	}
	rts_fta_process_packet(&cur_packet);
// --------------------------------------------------
// ---  Specialized processing for .proto sgnb_reconfiguration_complete.json, path recon_complete.json

	reconfig_success = (struct _reconfig_success *)(cur_packet.record.packed.values);
	cur_packet.schema = 101;
	node_1_0 = node_0_0;
	ts_lo = hdr->header->timestamp & 0xffffffff;
	ts_hi = hdr->header->timestamp >> 32;
	reconfig_success->timestamp_ms = (ts_hi - 2208988800) * 1000 + ((ts_lo * 1000) >> 32);
	if(hdr->header->gnbid==NULL)
		reconfig_success->gnb_id = empty_string;
	else
		reconfig_success->gnb_id = hdr->header->gnbid->value;

	reconfig_success->id_MeNB_UE_X2AP_ID = node_1_0->id_menb_ue_x2ap_id;
	reconfig_success->id_SgNB_UE_X2AP_ID = node_1_0->id_sgnb_ue_x2ap_id;
	if(node_1_0->id_menb_ue_x2ap_id_extension){
		reconfig_success->id_MeNB_UE_X2AP_ID_Extension = node_1_0->id_menb_ue_x2ap_id_extension->value;
	}else{
		reconfig_success->id_MeNB_UE_X2AP_ID_Extension = 0;
	}
	node_1_1 = node_1_0->id_responseinformationsgnbreconfcomp;
	if(!(node_1_0->id_responseinformationsgnbreconfcomp)){
		rts_fta_process_packet(&cur_packet);
	}else{
		node_1_2 = node_1_1->reject_by_menb_sgnbreconfcomp;
		if(!(node_1_1->value_case == STREAMING_PROTOBUFS__RESPONSE_INFORMATION_SG_NBRECONF_COMP__VALUE_REJECT_BY__ME_NB__SG_NBRECONF_COMP)){
			rts_fta_process_packet(&cur_packet);
		}else{
			rts_fta_process_packet(&cur_packet);
		}
	}
// --------------------------------------------------
// ---  Specialized processing for .proto sgnb_reconfiguration_complete.json, path recon_complete.json

	reconfig_reject = (struct _reconfig_reject *)(cur_packet.record.packed.values);
	cur_packet.schema = 102;
	node_2_0 = node_1_0;
	ts_lo = hdr->header->timestamp & 0xffffffff;
	ts_hi = hdr->header->timestamp >> 32;
	reconfig_reject->timestamp_ms = (ts_hi - 2208988800) * 1000 + ((ts_lo * 1000) >> 32);
	if(hdr->header->gnbid==NULL)
		reconfig_reject->gnb_id = empty_string;
	else
		reconfig_reject->gnb_id = hdr->header->gnbid->value;

	reconfig_reject->id_MeNB_UE_X2AP_ID = node_2_0->id_menb_ue_x2ap_id;
	reconfig_reject->id_SgNB_UE_X2AP_ID = node_2_0->id_sgnb_ue_x2ap_id;
	if(node_2_0->id_menb_ue_x2ap_id_extension){
		reconfig_reject->id_MeNB_UE_X2AP_ID_Extension = node_2_0->id_menb_ue_x2ap_id_extension->value;
	}else{
		reconfig_reject->id_MeNB_UE_X2AP_ID_Extension = 0;
	}
	node_2_1 = node_2_0->id_responseinformationsgnbreconfcomp;
	if(node_2_0->id_responseinformationsgnbreconfcomp){
		node_2_2 = node_2_1->reject_by_menb_sgnbreconfcomp;
		if(node_2_1->value_case == STREAMING_PROTOBUFS__RESPONSE_INFORMATION_SG_NBRECONF_COMP__VALUE_REJECT_BY__ME_NB__SG_NBRECONF_COMP){
			if(node_2_2->cause && node_2_2->cause->radionetwork){
				reconfig_reject->cause_radio_network = node_2_2->cause->radionetwork->value;
			}else{
				reconfig_reject->cause_radio_network = -1;
			}
			if(node_2_2->cause && node_2_2->cause->transport){
				reconfig_reject->cause_transport = node_2_2->cause->transport->value;
			}else{
				reconfig_reject->cause_transport = -1;
			}
			if(node_2_2->cause && node_2_2->cause->protocol){
				reconfig_reject->cause_protocol = node_2_2->cause->protocol->value;
			}else{
				reconfig_reject->cause_protocol = -1;
			}
			if(node_2_2->cause && node_2_2->cause->misc){
				reconfig_reject->cause_misc = node_2_2->cause->misc->value;
			}else{
				reconfig_reject->cause_misc = -1;
			}
			rts_fta_process_packet(&cur_packet);
		}
	}
	streaming_protobufs__sg_nbreconfiguration_complete__free_unpacked(node_0_0,NULL);
	return 0;
}

gs_uint32_t process_buffer_RELCONF(gs_uint8_t * buffer, gs_uint32_t buflen){
	char *empty_string = "";
unsigned long long int ts_lo, ts_hi;
	StreamingProtobufs__X2APStreaming *hdr = NULL;
// ------------------------------------------
// ---  Variables for .proto sgnb_release_confirm.json, path release_confirm.json
	struct _sgnb_release_confirm_from_menb_erabs *sgnb_release_confirm_from_menb_erabs = NULL;
	StreamingProtobufs__SgNBReleaseConfirm *node_0_0 = NULL;
	StreamingProtobufs__SgNBReleaseConfirmIEs *node_0_1 = NULL;
	StreamingProtobufs__ERABsToBeReleasedSgNBRelConfList *node_0_2 = NULL;
	StreamingProtobufs__ERABsToBeReleasedSgNBRelConfItem *node_0_3 = NULL;
	gs_uint32_t i_0_3;
	StreamingProtobufs__ERABsToBeReleasedSgNBRelConfSgNBPDCPpresent *node_0_4 = NULL;
	StreamingProtobufs__GTPtunnelEndpoint *node_0_5 = NULL;
// ------------------------------------------
// ---  Variables for .proto sgnb_release_confirm.json, path release_confirm.json
	struct _sgnb_release_confirm_from_menb *sgnb_release_confirm_from_menb = NULL;
	StreamingProtobufs__SgNBReleaseConfirm *node_1_0 = NULL;
	StreamingProtobufs__SgNBReleaseConfirmIEs *node_1_1 = NULL;

// --------------------------------------------------
// ---  Specialized processing for .proto sgnb_release_confirm.json, path release_confirm.json

	sgnb_release_confirm_from_menb_erabs = (struct _sgnb_release_confirm_from_menb_erabs *)(cur_packet.record.packed.values);
	cur_packet.schema = 1101;

	hdr = streaming_protobufs__x2_apstreaming__unpack(NULL, buflen, buffer);
	if(hdr==NULL) return -1;

	node_0_0 = hdr->sgnbreleaseconfirm;
	if(node_0_0==NULL) return -2;
	if(hdr->header==NULL) return -3;

	ts_lo = hdr->header->timestamp & 0xffffffff;
	ts_hi = hdr->header->timestamp >> 32;
	sgnb_release_confirm_from_menb_erabs->timestamp_ms = (ts_hi - 2208988800) * 1000 + ((ts_lo * 1000) >> 32);
	if(hdr->header->gnbid==NULL)
		sgnb_release_confirm_from_menb_erabs->gnb_id = empty_string;
	else
		sgnb_release_confirm_from_menb_erabs->gnb_id = hdr->header->gnbid->value;

	node_0_1 = node_0_0->protocolies;
	if(node_0_0->protocolies){
		sgnb_release_confirm_from_menb_erabs->id_MeNB_UE_X2AP_ID = node_0_1->id_menb_ue_x2ap_id;
		sgnb_release_confirm_from_menb_erabs->id_SgNB_UE_X2AP_ID = node_0_1->id_sgnb_ue_x2ap_id;
		if(node_0_1->id_menb_ue_x2ap_id_extension){
			sgnb_release_confirm_from_menb_erabs->id_MeNB_UE_X2AP_ID_Extension = node_0_1->id_menb_ue_x2ap_id_extension->value;
		}else{
			sgnb_release_confirm_from_menb_erabs->id_MeNB_UE_X2AP_ID_Extension = 0;
		}
		node_0_2 = node_0_1->id_e_rabs_tobereleased_sgnbrelconflist;
		if(node_0_1->id_e_rabs_tobereleased_sgnbrelconflist){
			for(i_0_3=0;i_0_3<node_0_2->n_id_e_rabs_tobereleased_sgnbrelconf_item; i_0_3++){
				node_0_3 = node_0_2->id_e_rabs_tobereleased_sgnbrelconf_item[i_0_3];
				if(node_0_3->en_dc_resourceconfiguration){
					sgnb_release_confirm_from_menb_erabs->sCGresources = node_0_3->en_dc_resourceconfiguration->scgresources;
				}else{
					sgnb_release_confirm_from_menb_erabs->sCGresources = -1;
				}
				sgnb_release_confirm_from_menb_erabs->e_RAB_ID = node_0_3->e_rab_id;
				if(node_0_3->en_dc_resourceconfiguration){
					sgnb_release_confirm_from_menb_erabs->pDCPatSgNB = node_0_3->en_dc_resourceconfiguration->pdcpatsgnb;
				}else{
					sgnb_release_confirm_from_menb_erabs->pDCPatSgNB = -1;
				}
				if(node_0_3->en_dc_resourceconfiguration){
					sgnb_release_confirm_from_menb_erabs->mCGresources = node_0_3->en_dc_resourceconfiguration->mcgresources;
				}else{
					sgnb_release_confirm_from_menb_erabs->mCGresources = -1;
				}
				node_0_4 = node_0_3->sgnbpdcppresent;
				if(node_0_3->sgnbpdcppresent){
					node_0_5 = node_0_4->dl_gtptunnelendpoint;
					if(node_0_4->dl_gtptunnelendpoint){
						sgnb_release_confirm_from_menb_erabs->gTP_TEID = node_0_5->gtp_teid;
						sgnb_release_confirm_from_menb_erabs->transportLayerAddress = node_0_5->transportlayeraddress;
						rts_fta_process_packet(&cur_packet);
					}
				}
			}
		}
	}
// --------------------------------------------------
// ---  Specialized processing for .proto sgnb_release_confirm.json, path release_confirm.json

	sgnb_release_confirm_from_menb = (struct _sgnb_release_confirm_from_menb *)(cur_packet.record.packed.values);
	cur_packet.schema = 1102;
	node_1_0 = node_0_0;
	ts_lo = hdr->header->timestamp & 0xffffffff;
	ts_hi = hdr->header->timestamp >> 32;
	sgnb_release_confirm_from_menb->timestamp_ms = (ts_hi - 2208988800) * 1000 + ((ts_lo * 1000) >> 32);
	if(hdr->header->gnbid==NULL)
		sgnb_release_confirm_from_menb->gnb_id = empty_string;
	else
		sgnb_release_confirm_from_menb->gnb_id = hdr->header->gnbid->value;

	node_1_1 = node_1_0->protocolies;
	if(node_1_0->protocolies){
		sgnb_release_confirm_from_menb->id_MeNB_UE_X2AP_ID = node_1_1->id_menb_ue_x2ap_id;
		sgnb_release_confirm_from_menb->id_SgNB_UE_X2AP_ID = node_1_1->id_sgnb_ue_x2ap_id;
		if(node_1_1->id_menb_ue_x2ap_id_extension){
			sgnb_release_confirm_from_menb->id_MeNB_UE_X2AP_ID_Extension = node_1_1->id_menb_ue_x2ap_id_extension->value;
		}else{
			sgnb_release_confirm_from_menb->id_MeNB_UE_X2AP_ID_Extension = 0;
		}
		rts_fta_process_packet(&cur_packet);
	}
	streaming_protobufs__sg_nbrelease_confirm__free_unpacked(node_0_0,NULL);
	return 0;
}

gs_uint32_t process_buffer_RELREQ(gs_uint8_t * buffer, gs_uint32_t buflen){
	char *empty_string = "";
unsigned long long int ts_lo, ts_hi;
	StreamingProtobufs__X2APStreaming *hdr = NULL;
// ------------------------------------------
// ---  Variables for .proto sgnb_release_request.json, path release_req.json
	struct _release_req *release_req = NULL;
	StreamingProtobufs__SgNBReleaseRequest *node_0_0 = NULL;
	StreamingProtobufs__SgNBReleaseRequestIEs *node_0_1 = NULL;

// --------------------------------------------------
// ---  Specialized processing for .proto sgnb_release_request.json, path release_req.json

	release_req = (struct _release_req *)(cur_packet.record.packed.values);
	cur_packet.schema = 801;

	hdr = streaming_protobufs__x2_apstreaming__unpack(NULL, buflen, buffer);
	if(hdr==NULL) return -1;

	node_0_0 = hdr->sgnbreleaserequest;
	if(node_0_0==NULL) return -2;
	if(hdr->header==NULL) return -3;

	ts_lo = hdr->header->timestamp & 0xffffffff;
	ts_hi = hdr->header->timestamp >> 32;
	release_req->timestamp_ms = (ts_hi - 2208988800) * 1000 + ((ts_lo * 1000) >> 32);
	if(hdr->header->gnbid==NULL)
		release_req->gnb_id = empty_string;
	else
		release_req->gnb_id = hdr->header->gnbid->value;

	node_0_1 = node_0_0->protocolies;
	if(node_0_0->protocolies){
		if(node_0_1->id_cause && node_0_1->id_cause->protocol){
			release_req->cause_protocol = node_0_1->id_cause->protocol->value;
		}else{
			release_req->cause_protocol = -1;
		}
		release_req->id_MeNB_UE_X2AP_ID = node_0_1->id_menb_ue_x2ap_id;
		if(node_0_1->id_cause && node_0_1->id_cause->transport){
			release_req->cause_transport = node_0_1->id_cause->transport->value;
		}else{
			release_req->cause_transport = -1;
		}
		if(node_0_1->id_menb_ue_x2ap_id_extension){
			release_req->id_MeNB_UE_X2AP_ID_Extension = node_0_1->id_menb_ue_x2ap_id_extension->value;
		}else{
			release_req->id_MeNB_UE_X2AP_ID_Extension = 0;
		}
		if(node_0_1->id_cause && node_0_1->id_cause->radionetwork){
			release_req->cause_radio_network = node_0_1->id_cause->radionetwork->value;
		}else{
			release_req->cause_radio_network = -1;
		}
		if(node_0_1->id_sgnb_ue_x2ap_id){
			release_req->id_SgNB_UE_X2AP_ID = node_0_1->id_sgnb_ue_x2ap_id->value;
		}else{
			release_req->id_SgNB_UE_X2AP_ID = 0;
		}
		if(node_0_1->id_cause && node_0_1->id_cause->misc){
			release_req->cause_misc = node_0_1->id_cause->misc->value;
		}else{
			release_req->cause_misc = -1;
		}
		rts_fta_process_packet(&cur_packet);
	}
	streaming_protobufs__sg_nbrelease_request__free_unpacked(node_0_0,NULL);
	return 0;
}

gs_uint32_t process_buffer_RELREQACK(gs_uint8_t * buffer, gs_uint32_t buflen){
	char *empty_string = "";
unsigned long long int ts_lo, ts_hi;
	StreamingProtobufs__X2APStreaming *hdr = NULL;
// ------------------------------------------
// ---  Variables for .proto sgnb_release_request_acknowledge.json, path release_req_ack.json
	struct _release_req_ack *release_req_ack = NULL;
	StreamingProtobufs__SgNBReleaseRequestAcknowledge *node_0_0 = NULL;
	StreamingProtobufs__SgNBReleaseRequestAcknowledgeIEs *node_0_1 = NULL;

// --------------------------------------------------
// ---  Specialized processing for .proto sgnb_release_request_acknowledge.json, path release_req_ack.json

	release_req_ack = (struct _release_req_ack *)(cur_packet.record.packed.values);
	cur_packet.schema = 901;

	hdr = streaming_protobufs__x2_apstreaming__unpack(NULL, buflen, buffer);
	if(hdr==NULL) return -1;

	node_0_0 = hdr->sgnbreleaserequestacknowledge;
	if(node_0_0==NULL) return -2;
	if(hdr->header==NULL) return -3;

	ts_lo = hdr->header->timestamp & 0xffffffff;
	ts_hi = hdr->header->timestamp >> 32;
	release_req_ack->timestamp_ms = (ts_hi - 2208988800) * 1000 + ((ts_lo * 1000) >> 32);
	if(hdr->header->gnbid==NULL)
		release_req_ack->gnb_id = empty_string;
	else
		release_req_ack->gnb_id = hdr->header->gnbid->value;

	node_0_1 = node_0_0->protocolies;
	if(node_0_0->protocolies){
		release_req_ack->id_MeNB_UE_X2AP_ID = node_0_1->id_menb_ue_x2ap_id;
		release_req_ack->id_SgNB_UE_X2AP_ID = node_0_1->id_sgnb_ue_x2ap_id;
		if(node_0_1->id_menb_ue_x2ap_id_extension){
			release_req_ack->id_MeNB_UE_X2AP_ID_Extension = node_0_1->id_menb_ue_x2ap_id_extension->value;
		}else{
			release_req_ack->id_MeNB_UE_X2AP_ID_Extension = 0;
		}
		rts_fta_process_packet(&cur_packet);
	}
	streaming_protobufs__sg_nbrelease_request_acknowledge__free_unpacked(node_0_0,NULL);
	return 0;
}

gs_uint32_t process_buffer_SGNBRELEASERQD(gs_uint8_t * buffer, gs_uint32_t buflen){
	char *empty_string = "";
unsigned long long int ts_lo, ts_hi;
	StreamingProtobufs__X2APStreaming *hdr = NULL;
// ------------------------------------------
// ---  Variables for .proto sgnb_release_required.json, path release_rqd.json
	struct _SgNB_release_rqd *SgNB_release_rqd = NULL;
	StreamingProtobufs__SgNBReleaseRequired *node_0_0 = NULL;
	StreamingProtobufs__SgNBReleaseRequiredIEs *node_0_1 = NULL;

// --------------------------------------------------
// ---  Specialized processing for .proto sgnb_release_required.json, path release_rqd.json

	SgNB_release_rqd = (struct _SgNB_release_rqd *)(cur_packet.record.packed.values);
	cur_packet.schema = 1001;

	hdr = streaming_protobufs__x2_apstreaming__unpack(NULL, buflen, buffer);
	if(hdr==NULL) return -1;

	node_0_0 = hdr->sgnbreleaserequired;
	if(node_0_0==NULL) return -2;
	if(hdr->header==NULL) return -3;

	ts_lo = hdr->header->timestamp & 0xffffffff;
	ts_hi = hdr->header->timestamp >> 32;
	SgNB_release_rqd->timestamp_ms = (ts_hi - 2208988800) * 1000 + ((ts_lo * 1000) >> 32);
	if(hdr->header->gnbid==NULL)
		SgNB_release_rqd->gnb_id = empty_string;
	else
		SgNB_release_rqd->gnb_id = hdr->header->gnbid->value;

	node_0_1 = node_0_0->protocolies;
	if(node_0_0->protocolies){
		if(node_0_1->id_cause && node_0_1->id_cause->protocol){
			SgNB_release_rqd->cause_protocol = node_0_1->id_cause->protocol->value;
		}else{
			SgNB_release_rqd->cause_protocol = -1;
		}
		SgNB_release_rqd->id_MeNB_UE_X2AP_ID = node_0_1->id_menb_ue_x2ap_id;
		if(node_0_1->id_cause && node_0_1->id_cause->transport){
			SgNB_release_rqd->cause_transport = node_0_1->id_cause->transport->value;
		}else{
			SgNB_release_rqd->cause_transport = -1;
		}
		if(node_0_1->id_e_rabs_tobereleased_sgnbrelreqdlist && node_0_1->id_e_rabs_tobereleased_sgnbrelreqdlist->id_e_rabs_tobereleased_sgnbrelreqd_item && node_0_1->id_e_rabs_tobereleased_sgnbrelreqdlist->n_id_e_rabs_tobereleased_sgnbrelreqd_item > 0){
			SgNB_release_rqd->e_RAB_ID0 = node_0_1->id_e_rabs_tobereleased_sgnbrelreqdlist->id_e_rabs_tobereleased_sgnbrelreqd_item[0]->e_rab_id;
			SgNB_release_rqd->e_RAB_ID0_exists = 1;
		}else{
			SgNB_release_rqd->e_RAB_ID0_exists = 0;
		}
		if(node_0_1->id_e_rabs_tobereleased_sgnbrelreqdlist && node_0_1->id_e_rabs_tobereleased_sgnbrelreqdlist->id_e_rabs_tobereleased_sgnbrelreqd_item && node_0_1->id_e_rabs_tobereleased_sgnbrelreqdlist->n_id_e_rabs_tobereleased_sgnbrelreqd_item > 1){
			SgNB_release_rqd->e_RAB_ID1 = node_0_1->id_e_rabs_tobereleased_sgnbrelreqdlist->id_e_rabs_tobereleased_sgnbrelreqd_item[1]->e_rab_id;
			SgNB_release_rqd->e_RAB_ID1_exists = 1;
		}else{
			SgNB_release_rqd->e_RAB_ID1_exists = 0;
		}
		if(node_0_1->id_e_rabs_tobereleased_sgnbrelreqdlist && node_0_1->id_e_rabs_tobereleased_sgnbrelreqdlist->id_e_rabs_tobereleased_sgnbrelreqd_item && node_0_1->id_e_rabs_tobereleased_sgnbrelreqdlist->n_id_e_rabs_tobereleased_sgnbrelreqd_item > 2){
			SgNB_release_rqd->e_RAB_ID2 = node_0_1->id_e_rabs_tobereleased_sgnbrelreqdlist->id_e_rabs_tobereleased_sgnbrelreqd_item[2]->e_rab_id;
			SgNB_release_rqd->e_RAB_ID2_exists = 1;
		}else{
			SgNB_release_rqd->e_RAB_ID2_exists = 0;
		}
		if(node_0_1->id_e_rabs_tobereleased_sgnbrelreqdlist && node_0_1->id_e_rabs_tobereleased_sgnbrelreqdlist->id_e_rabs_tobereleased_sgnbrelreqd_item && node_0_1->id_e_rabs_tobereleased_sgnbrelreqdlist->n_id_e_rabs_tobereleased_sgnbrelreqd_item > 3){
			SgNB_release_rqd->e_RAB_ID3 = node_0_1->id_e_rabs_tobereleased_sgnbrelreqdlist->id_e_rabs_tobereleased_sgnbrelreqd_item[3]->e_rab_id;
			SgNB_release_rqd->e_RAB_ID3_exists = 1;
		}else{
			SgNB_release_rqd->e_RAB_ID3_exists = 0;
		}
		if(node_0_1->id_e_rabs_tobereleased_sgnbrelreqdlist && node_0_1->id_e_rabs_tobereleased_sgnbrelreqdlist->id_e_rabs_tobereleased_sgnbrelreqd_item && node_0_1->id_e_rabs_tobereleased_sgnbrelreqdlist->n_id_e_rabs_tobereleased_sgnbrelreqd_item > 4){
			SgNB_release_rqd->e_RAB_ID4 = node_0_1->id_e_rabs_tobereleased_sgnbrelreqdlist->id_e_rabs_tobereleased_sgnbrelreqd_item[4]->e_rab_id;
			SgNB_release_rqd->e_RAB_ID4_exists = 1;
		}else{
			SgNB_release_rqd->e_RAB_ID4_exists = 0;
		}
		if(node_0_1->id_e_rabs_tobereleased_sgnbrelreqdlist && node_0_1->id_e_rabs_tobereleased_sgnbrelreqdlist->id_e_rabs_tobereleased_sgnbrelreqd_item && node_0_1->id_e_rabs_tobereleased_sgnbrelreqdlist->n_id_e_rabs_tobereleased_sgnbrelreqd_item > 5){
			SgNB_release_rqd->e_RAB_ID5 = node_0_1->id_e_rabs_tobereleased_sgnbrelreqdlist->id_e_rabs_tobereleased_sgnbrelreqd_item[5]->e_rab_id;
			SgNB_release_rqd->e_RAB_ID5_exists = 1;
		}else{
			SgNB_release_rqd->e_RAB_ID5_exists = 0;
		}
		if(node_0_1->id_e_rabs_tobereleased_sgnbrelreqdlist && node_0_1->id_e_rabs_tobereleased_sgnbrelreqdlist->id_e_rabs_tobereleased_sgnbrelreqd_item && node_0_1->id_e_rabs_tobereleased_sgnbrelreqdlist->n_id_e_rabs_tobereleased_sgnbrelreqd_item > 6){
			SgNB_release_rqd->e_RAB_ID6 = node_0_1->id_e_rabs_tobereleased_sgnbrelreqdlist->id_e_rabs_tobereleased_sgnbrelreqd_item[6]->e_rab_id;
			SgNB_release_rqd->e_RAB_ID6_exists = 1;
		}else{
			SgNB_release_rqd->e_RAB_ID6_exists = 0;
		}
		if(node_0_1->id_e_rabs_tobereleased_sgnbrelreqdlist && node_0_1->id_e_rabs_tobereleased_sgnbrelreqdlist->id_e_rabs_tobereleased_sgnbrelreqd_item && node_0_1->id_e_rabs_tobereleased_sgnbrelreqdlist->n_id_e_rabs_tobereleased_sgnbrelreqd_item > 7){
			SgNB_release_rqd->e_RAB_ID7 = node_0_1->id_e_rabs_tobereleased_sgnbrelreqdlist->id_e_rabs_tobereleased_sgnbrelreqd_item[7]->e_rab_id;
			SgNB_release_rqd->e_RAB_ID7_exists = 1;
		}else{
			SgNB_release_rqd->e_RAB_ID7_exists = 0;
		}
		if(node_0_1->id_cause && node_0_1->id_cause->radionetwork){
			SgNB_release_rqd->cause_radio_network = node_0_1->id_cause->radionetwork->value;
		}else{
			SgNB_release_rqd->cause_radio_network = -1;
		}
		SgNB_release_rqd->id_SgNB_UE_X2AP_ID = node_0_1->id_sgnb_ue_x2ap_id;
		if(node_0_1->id_cause && node_0_1->id_cause->misc){
			SgNB_release_rqd->cause_misc = node_0_1->id_cause->misc->value;
		}else{
			SgNB_release_rqd->cause_misc = -1;
		}
		rts_fta_process_packet(&cur_packet);
	}
	streaming_protobufs__sg_nbrelease_required__free_unpacked(node_0_0,NULL);
	return 0;
}

gs_uint32_t process_buffer_RRCXFER(gs_uint8_t * buffer, gs_uint32_t buflen){
	char *empty_string = "";
unsigned long long int ts_lo, ts_hi;
	StreamingProtobufs__X2APStreaming *hdr = NULL;
// ------------------------------------------
// ---  Variables for .proto rrctransfer.json, path rrc_metrics.json
	struct _serv_nr_cell *serv_nr_cell = NULL;
	StreamingProtobufs__RRCTransfer *node_0_0 = NULL;
	StreamingProtobufs__RRCTransferIEs *node_0_1 = NULL;
	StreamingProtobufs__UENRMeasurement *node_0_2 = NULL;
	StreamingProtobufs__RRCContainer *node_0_3 = NULL;
	StreamingProtobufs__ULDCCHMessageType *node_0_4 = NULL;
	StreamingProtobufs__MeasurementReport *node_0_5 = NULL;
	StreamingProtobufs__MeasurementReportIEs *node_0_6 = NULL;
	StreamingProtobufs__MeasResults *node_0_7 = NULL;
	StreamingProtobufs__MeasResultServMOList *node_0_8 = NULL;
	StreamingProtobufs__MeasResultServMO *node_0_9 = NULL;
	gs_uint32_t i_0_9;
	StreamingProtobufs__MeasResultNR *node_0_10 = NULL;
	StreamingProtobufs__MeasResult *node_0_11 = NULL;
	StreamingProtobufs__CellResults *node_0_12 = NULL;
	StreamingProtobufs__MeasQuantityResults *node_0_13 = NULL;
// ------------------------------------------
// ---  Variables for .proto rrctransfer.json, path rrc_metrics.json
	struct _nr_neighbor *nr_neighbor = NULL;
	StreamingProtobufs__RRCTransfer *node_1_0 = NULL;
	StreamingProtobufs__RRCTransferIEs *node_1_1 = NULL;
	StreamingProtobufs__UENRMeasurement *node_1_2 = NULL;
	StreamingProtobufs__RRCContainer *node_1_3 = NULL;
	StreamingProtobufs__ULDCCHMessageType *node_1_4 = NULL;
	StreamingProtobufs__MeasurementReport *node_1_5 = NULL;
	StreamingProtobufs__MeasurementReportIEs *node_1_6 = NULL;
	StreamingProtobufs__MeasResults *node_1_7 = NULL;
	StreamingProtobufs__MeasResultListNR *node_1_8 = NULL;
	StreamingProtobufs__MeasResultNR *node_1_9 = NULL;
	gs_uint32_t i_1_9;
	StreamingProtobufs__MeasResult *node_1_10 = NULL;
	StreamingProtobufs__CellResults *node_1_11 = NULL;
	StreamingProtobufs__MeasQuantityResults *node_1_12 = NULL;
// ------------------------------------------
// ---  Variables for .proto rrctransfer.json, path rrc_metrics.json
	struct _serv_cell_beam_csi *serv_cell_beam_csi = NULL;
	StreamingProtobufs__RRCTransfer *node_2_0 = NULL;
	StreamingProtobufs__RRCTransferIEs *node_2_1 = NULL;
	StreamingProtobufs__UENRMeasurement *node_2_2 = NULL;
	StreamingProtobufs__RRCContainer *node_2_3 = NULL;
	StreamingProtobufs__ULDCCHMessageType *node_2_4 = NULL;
	StreamingProtobufs__MeasurementReport *node_2_5 = NULL;
	StreamingProtobufs__MeasurementReportIEs *node_2_6 = NULL;
	StreamingProtobufs__MeasResults *node_2_7 = NULL;
	StreamingProtobufs__MeasResultServMOList *node_2_8 = NULL;
	StreamingProtobufs__MeasResultServMO *node_2_9 = NULL;
	gs_uint32_t i_2_9;
	StreamingProtobufs__MeasResultNR *node_2_10 = NULL;
	StreamingProtobufs__MeasResult *node_2_11 = NULL;
	StreamingProtobufs__RsIndexResults *node_2_12 = NULL;
	StreamingProtobufs__ResultsPerCSIRSIndexList *node_2_13 = NULL;
	StreamingProtobufs__ResultsPerCSIRSIndex *node_2_14 = NULL;
	gs_uint32_t i_2_14;
	StreamingProtobufs__MeasQuantityResults *node_2_15 = NULL;
// ------------------------------------------
// ---  Variables for .proto rrctransfer.json, path rrc_metrics.json
	struct _neighbor_beam_csi *neighbor_beam_csi = NULL;
	StreamingProtobufs__RRCTransfer *node_3_0 = NULL;
	StreamingProtobufs__RRCTransferIEs *node_3_1 = NULL;
	StreamingProtobufs__UENRMeasurement *node_3_2 = NULL;
	StreamingProtobufs__RRCContainer *node_3_3 = NULL;
	StreamingProtobufs__ULDCCHMessageType *node_3_4 = NULL;
	StreamingProtobufs__MeasurementReport *node_3_5 = NULL;
	StreamingProtobufs__MeasurementReportIEs *node_3_6 = NULL;
	StreamingProtobufs__MeasResults *node_3_7 = NULL;
	StreamingProtobufs__MeasResultListNR *node_3_8 = NULL;
	StreamingProtobufs__MeasResultNR *node_3_9 = NULL;
	gs_uint32_t i_3_9;
	StreamingProtobufs__MeasResult *node_3_10 = NULL;
	StreamingProtobufs__RsIndexResults *node_3_11 = NULL;
	StreamingProtobufs__ResultsPerCSIRSIndexList *node_3_12 = NULL;
	StreamingProtobufs__ResultsPerCSIRSIndex *node_3_13 = NULL;
	gs_uint32_t i_3_13;
	StreamingProtobufs__MeasQuantityResults *node_3_14 = NULL;
// ------------------------------------------
// ---  Variables for .proto rrctransfer.json, path rrc_metrics.json
	struct _serv_cell_beam_ssb *serv_cell_beam_ssb = NULL;
	StreamingProtobufs__RRCTransfer *node_4_0 = NULL;
	StreamingProtobufs__RRCTransferIEs *node_4_1 = NULL;
	StreamingProtobufs__UENRMeasurement *node_4_2 = NULL;
	StreamingProtobufs__RRCContainer *node_4_3 = NULL;
	StreamingProtobufs__ULDCCHMessageType *node_4_4 = NULL;
	StreamingProtobufs__MeasurementReport *node_4_5 = NULL;
	StreamingProtobufs__MeasurementReportIEs *node_4_6 = NULL;
	StreamingProtobufs__MeasResults *node_4_7 = NULL;
	StreamingProtobufs__MeasResultServMOList *node_4_8 = NULL;
	StreamingProtobufs__MeasResultServMO *node_4_9 = NULL;
	gs_uint32_t i_4_9;
	StreamingProtobufs__MeasResultNR *node_4_10 = NULL;
	StreamingProtobufs__MeasResult *node_4_11 = NULL;
	StreamingProtobufs__RsIndexResults *node_4_12 = NULL;
	StreamingProtobufs__ResultsPerSSBIndexList *node_4_13 = NULL;
	StreamingProtobufs__ResultsPerSSBIndex *node_4_14 = NULL;
	gs_uint32_t i_4_14;
	StreamingProtobufs__MeasQuantityResults *node_4_15 = NULL;
// ------------------------------------------
// ---  Variables for .proto rrctransfer.json, path rrc_metrics.json
	struct _neighbor_beam_ssb *neighbor_beam_ssb = NULL;
	StreamingProtobufs__RRCTransfer *node_5_0 = NULL;
	StreamingProtobufs__RRCTransferIEs *node_5_1 = NULL;
	StreamingProtobufs__UENRMeasurement *node_5_2 = NULL;
	StreamingProtobufs__RRCContainer *node_5_3 = NULL;
	StreamingProtobufs__ULDCCHMessageType *node_5_4 = NULL;
	StreamingProtobufs__MeasurementReport *node_5_5 = NULL;
	StreamingProtobufs__MeasurementReportIEs *node_5_6 = NULL;
	StreamingProtobufs__MeasResults *node_5_7 = NULL;
	StreamingProtobufs__MeasResultListNR *node_5_8 = NULL;
	StreamingProtobufs__MeasResultNR *node_5_9 = NULL;
	gs_uint32_t i_5_9;
	StreamingProtobufs__MeasResult *node_5_10 = NULL;
	StreamingProtobufs__RsIndexResults *node_5_11 = NULL;
	StreamingProtobufs__ResultsPerSSBIndexList *node_5_12 = NULL;
	StreamingProtobufs__ResultsPerSSBIndex *node_5_13 = NULL;
	gs_uint32_t i_5_13;
	StreamingProtobufs__MeasQuantityResults *node_5_14 = NULL;

// --------------------------------------------------
// ---  Specialized processing for .proto rrctransfer.json, path rrc_metrics.json

	serv_nr_cell = (struct _serv_nr_cell *)(cur_packet.record.packed.values);
	cur_packet.schema = 1;

	hdr = streaming_protobufs__x2_apstreaming__unpack(NULL, buflen, buffer);
	if(hdr==NULL) return -1;

	node_0_0 = hdr->rrctransfer;
	if(node_0_0==NULL) return -2;
	if(hdr->header==NULL) return -3;

	ts_lo = hdr->header->timestamp & 0xffffffff;
	ts_hi = hdr->header->timestamp >> 32;
	serv_nr_cell->timestamp_ms = (ts_hi - 2208988800) * 1000 + ((ts_lo * 1000) >> 32);
	if(hdr->header->gnbid==NULL)
		serv_nr_cell->gnb_id = empty_string;
	else
		serv_nr_cell->gnb_id = hdr->header->gnbid->value;

	node_0_1 = node_0_0->rrctransfer_ies;
	if(node_0_0->rrctransfer_ies){
		serv_nr_cell->id_MeNB_UE_X2AP_ID = node_0_1->id_menb_ue_x2ap_id;
		serv_nr_cell->id_SgNB_UE_X2AP_ID = node_0_1->id_sgnb_ue_x2ap_id;
		node_0_2 = node_0_1->id_uenrmeasurement;
		if(node_0_1->id_uenrmeasurement){
			node_0_3 = node_0_2->uenrmeasurements;
			if(node_0_2->uenrmeasurements){
				node_0_4 = node_0_3->ul_dcch_message;
				if(node_0_3->ul_dcch_message){
					node_0_5 = node_0_4->measurementreport;
					if(node_0_4->measurementreport){
						node_0_6 = node_0_5->measurementreport;
						if(node_0_5->measurementreport){
							node_0_7 = node_0_6->measresults;
							if(node_0_6->measresults){
								node_0_8 = node_0_7->measresultservingmolist;
								if(node_0_7->measresultservingmolist){
									for(i_0_9=0;i_0_9<node_0_8->n_items; i_0_9++){
										node_0_9 = node_0_8->items[i_0_9];
										serv_nr_cell->servCellID = node_0_9->servcellid;
										node_0_10 = node_0_9->measresultservingcell;
										if(node_0_9->measresultservingcell){
											if(node_0_10->physcellid){
												serv_nr_cell->physCellId = node_0_10->physcellid->value;
												serv_nr_cell->physCellId_exists = 1;
											}else{
												serv_nr_cell->physCellId_exists = 0;
											}
											node_0_11 = node_0_10->measresult;
											if(node_0_10->measresult){
												node_0_12 = node_0_11->cellresults;
												if(node_0_11->cellresults){
													node_0_13 = node_0_12->resultsssb_cell;
													if(node_0_12->resultsssb_cell){
														if(node_0_13->rsrq){
															serv_nr_cell->rsrq = node_0_13->rsrq->value;
															serv_nr_cell->rsrq_exists = 1;
														}else{
															serv_nr_cell->rsrq_exists = 0;
														}
														if(node_0_13->rsrp){
															serv_nr_cell->rsrp = node_0_13->rsrp->value;
															serv_nr_cell->rsrp_exists = 1;
														}else{
															serv_nr_cell->rsrp_exists = 0;
														}
														if(node_0_13->sinr){
															serv_nr_cell->sinr = node_0_13->sinr->value;
															serv_nr_cell->sinr_exists = 1;
														}else{
															serv_nr_cell->sinr_exists = 0;
														}
														rts_fta_process_packet(&cur_packet);
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
// --------------------------------------------------
// ---  Specialized processing for .proto rrctransfer.json, path rrc_metrics.json

	nr_neighbor = (struct _nr_neighbor *)(cur_packet.record.packed.values);
	cur_packet.schema = 4;
	node_1_0 = node_0_0;
	ts_lo = hdr->header->timestamp & 0xffffffff;
	ts_hi = hdr->header->timestamp >> 32;
	nr_neighbor->timestamp_ms = (ts_hi - 2208988800) * 1000 + ((ts_lo * 1000) >> 32);
	if(hdr->header->gnbid==NULL)
		nr_neighbor->gnb_id = empty_string;
	else
		nr_neighbor->gnb_id = hdr->header->gnbid->value;

	node_1_1 = node_1_0->rrctransfer_ies;
	if(node_1_0->rrctransfer_ies){
		nr_neighbor->id_MeNB_UE_X2AP_ID = node_1_1->id_menb_ue_x2ap_id;
		nr_neighbor->id_SgNB_UE_X2AP_ID = node_1_1->id_sgnb_ue_x2ap_id;
		node_1_2 = node_1_1->id_uenrmeasurement;
		if(node_1_1->id_uenrmeasurement){
			node_1_3 = node_1_2->uenrmeasurements;
			if(node_1_2->uenrmeasurements){
				node_1_4 = node_1_3->ul_dcch_message;
				if(node_1_3->ul_dcch_message){
					node_1_5 = node_1_4->measurementreport;
					if(node_1_4->measurementreport){
						node_1_6 = node_1_5->measurementreport;
						if(node_1_5->measurementreport){
							node_1_7 = node_1_6->measresults;
							if(node_1_6->measresults){
								node_1_8 = node_1_7->measresultlistnr;
								if(node_1_7->measresultlistnr){
									for(i_1_9=0;i_1_9<node_1_8->n_items; i_1_9++){
										node_1_9 = node_1_8->items[i_1_9];
										if(node_1_9->physcellid){
											nr_neighbor->physCellId = node_1_9->physcellid->value;
											nr_neighbor->physCellId_exists = 1;
										}else{
											nr_neighbor->physCellId_exists = 0;
										}
										node_1_10 = node_1_9->measresult;
										if(node_1_9->measresult){
											node_1_11 = node_1_10->cellresults;
											if(node_1_10->cellresults){
												node_1_12 = node_1_11->resultsssb_cell;
												if(node_1_11->resultsssb_cell){
													if(node_1_12->rsrq){
														nr_neighbor->rsrq = node_1_12->rsrq->value;
														nr_neighbor->rsrq_exists = 1;
													}else{
														nr_neighbor->rsrq_exists = 0;
													}
													if(node_1_12->rsrp){
														nr_neighbor->rsrp = node_1_12->rsrp->value;
														nr_neighbor->rsrp_exists = 1;
													}else{
														nr_neighbor->rsrp_exists = 0;
													}
													if(node_1_12->sinr){
														nr_neighbor->sinr = node_1_12->sinr->value;
														nr_neighbor->sinr_exists = 1;
													}else{
														nr_neighbor->sinr_exists = 0;
													}
													rts_fta_process_packet(&cur_packet);
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
// --------------------------------------------------
// ---  Specialized processing for .proto rrctransfer.json, path rrc_metrics.json

	serv_cell_beam_csi = (struct _serv_cell_beam_csi *)(cur_packet.record.packed.values);
	cur_packet.schema = 2;
	node_2_0 = node_1_0;
	ts_lo = hdr->header->timestamp & 0xffffffff;
	ts_hi = hdr->header->timestamp >> 32;
	serv_cell_beam_csi->timestamp_ms = (ts_hi - 2208988800) * 1000 + ((ts_lo * 1000) >> 32);
	if(hdr->header->gnbid==NULL)
		serv_cell_beam_csi->gnb_id = empty_string;
	else
		serv_cell_beam_csi->gnb_id = hdr->header->gnbid->value;

	node_2_1 = node_2_0->rrctransfer_ies;
	if(node_2_0->rrctransfer_ies){
		serv_cell_beam_csi->id_MeNB_UE_X2AP_ID = node_2_1->id_menb_ue_x2ap_id;
		serv_cell_beam_csi->id_SgNB_UE_X2AP_ID = node_2_1->id_sgnb_ue_x2ap_id;
		node_2_2 = node_2_1->id_uenrmeasurement;
		if(node_2_1->id_uenrmeasurement){
			node_2_3 = node_2_2->uenrmeasurements;
			if(node_2_2->uenrmeasurements){
				node_2_4 = node_2_3->ul_dcch_message;
				if(node_2_3->ul_dcch_message){
					node_2_5 = node_2_4->measurementreport;
					if(node_2_4->measurementreport){
						node_2_6 = node_2_5->measurementreport;
						if(node_2_5->measurementreport){
							node_2_7 = node_2_6->measresults;
							if(node_2_6->measresults){
								node_2_8 = node_2_7->measresultservingmolist;
								if(node_2_7->measresultservingmolist){
									for(i_2_9=0;i_2_9<node_2_8->n_items; i_2_9++){
										node_2_9 = node_2_8->items[i_2_9];
										serv_cell_beam_csi->servCellID = node_2_9->servcellid;
										node_2_10 = node_2_9->measresultservingcell;
										if(node_2_9->measresultservingcell){
											if(node_2_10->physcellid){
												serv_cell_beam_csi->physCellId = node_2_10->physcellid->value;
												serv_cell_beam_csi->physCellId_exists = 1;
											}else{
												serv_cell_beam_csi->physCellId_exists = 0;
											}
											node_2_11 = node_2_10->measresult;
											if(node_2_10->measresult){
												node_2_12 = node_2_11->rsindexresults;
												if(node_2_11->rsindexresults){
													node_2_13 = node_2_12->resultscsi_rs_indexes;
													if(node_2_12->resultscsi_rs_indexes){
														for(i_2_14=0;i_2_14<node_2_13->n_items; i_2_14++){
															node_2_14 = node_2_13->items[i_2_14];
															serv_cell_beam_csi->csi_rs_index = node_2_14->csi_rs_index;
															node_2_15 = node_2_14->csi_rs_results;
															if(node_2_14->csi_rs_results){
																if(node_2_15->rsrq){
																	serv_cell_beam_csi->rsrq = node_2_15->rsrq->value;
																	serv_cell_beam_csi->rsrq_exists = 1;
																}else{
																	serv_cell_beam_csi->rsrq_exists = 0;
																}
																if(node_2_15->rsrp){
																	serv_cell_beam_csi->rsrp = node_2_15->rsrp->value;
																	serv_cell_beam_csi->rsrp_exists = 1;
																}else{
																	serv_cell_beam_csi->rsrp_exists = 0;
																}
																if(node_2_15->sinr){
																	serv_cell_beam_csi->sinr = node_2_15->sinr->value;
																	serv_cell_beam_csi->sinr_exists = 1;
																}else{
																	serv_cell_beam_csi->sinr_exists = 0;
																}
																rts_fta_process_packet(&cur_packet);
															}
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
// --------------------------------------------------
// ---  Specialized processing for .proto rrctransfer.json, path rrc_metrics.json

	neighbor_beam_csi = (struct _neighbor_beam_csi *)(cur_packet.record.packed.values);
	cur_packet.schema = 5;
	node_3_0 = node_2_0;
	ts_lo = hdr->header->timestamp & 0xffffffff;
	ts_hi = hdr->header->timestamp >> 32;
	neighbor_beam_csi->timestamp_ms = (ts_hi - 2208988800) * 1000 + ((ts_lo * 1000) >> 32);
	if(hdr->header->gnbid==NULL)
		neighbor_beam_csi->gnb_id = empty_string;
	else
		neighbor_beam_csi->gnb_id = hdr->header->gnbid->value;

	node_3_1 = node_3_0->rrctransfer_ies;
	if(node_3_0->rrctransfer_ies){
		neighbor_beam_csi->id_MeNB_UE_X2AP_ID = node_3_1->id_menb_ue_x2ap_id;
		neighbor_beam_csi->id_SgNB_UE_X2AP_ID = node_3_1->id_sgnb_ue_x2ap_id;
		node_3_2 = node_3_1->id_uenrmeasurement;
		if(node_3_1->id_uenrmeasurement){
			node_3_3 = node_3_2->uenrmeasurements;
			if(node_3_2->uenrmeasurements){
				node_3_4 = node_3_3->ul_dcch_message;
				if(node_3_3->ul_dcch_message){
					node_3_5 = node_3_4->measurementreport;
					if(node_3_4->measurementreport){
						node_3_6 = node_3_5->measurementreport;
						if(node_3_5->measurementreport){
							node_3_7 = node_3_6->measresults;
							if(node_3_6->measresults){
								node_3_8 = node_3_7->measresultlistnr;
								if(node_3_7->measresultlistnr){
									for(i_3_9=0;i_3_9<node_3_8->n_items; i_3_9++){
										node_3_9 = node_3_8->items[i_3_9];
										if(node_3_9->physcellid){
											neighbor_beam_csi->physCellId = node_3_9->physcellid->value;
											neighbor_beam_csi->physCellId_exists = 1;
										}else{
											neighbor_beam_csi->physCellId_exists = 0;
										}
										node_3_10 = node_3_9->measresult;
										if(node_3_9->measresult){
											node_3_11 = node_3_10->rsindexresults;
											if(node_3_10->rsindexresults){
												node_3_12 = node_3_11->resultscsi_rs_indexes;
												if(node_3_11->resultscsi_rs_indexes){
													for(i_3_13=0;i_3_13<node_3_12->n_items; i_3_13++){
														node_3_13 = node_3_12->items[i_3_13];
														neighbor_beam_csi->csi_rs_index = node_3_13->csi_rs_index;
														node_3_14 = node_3_13->csi_rs_results;
														if(node_3_13->csi_rs_results){
															if(node_3_14->rsrq){
																neighbor_beam_csi->rsrq = node_3_14->rsrq->value;
																neighbor_beam_csi->rsrq_exists = 1;
															}else{
																neighbor_beam_csi->rsrq_exists = 0;
															}
															if(node_3_14->rsrp){
																neighbor_beam_csi->rsrp = node_3_14->rsrp->value;
																neighbor_beam_csi->rsrp_exists = 1;
															}else{
																neighbor_beam_csi->rsrp_exists = 0;
															}
															if(node_3_14->sinr){
																neighbor_beam_csi->sinr = node_3_14->sinr->value;
																neighbor_beam_csi->sinr_exists = 1;
															}else{
																neighbor_beam_csi->sinr_exists = 0;
															}
															rts_fta_process_packet(&cur_packet);
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
// --------------------------------------------------
// ---  Specialized processing for .proto rrctransfer.json, path rrc_metrics.json

	serv_cell_beam_ssb = (struct _serv_cell_beam_ssb *)(cur_packet.record.packed.values);
	cur_packet.schema = 3;
	node_4_0 = node_3_0;
	ts_lo = hdr->header->timestamp & 0xffffffff;
	ts_hi = hdr->header->timestamp >> 32;
	serv_cell_beam_ssb->timestamp_ms = (ts_hi - 2208988800) * 1000 + ((ts_lo * 1000) >> 32);
	if(hdr->header->gnbid==NULL)
		serv_cell_beam_ssb->gnb_id = empty_string;
	else
		serv_cell_beam_ssb->gnb_id = hdr->header->gnbid->value;

	node_4_1 = node_4_0->rrctransfer_ies;
	if(node_4_0->rrctransfer_ies){
		serv_cell_beam_ssb->id_MeNB_UE_X2AP_ID = node_4_1->id_menb_ue_x2ap_id;
		serv_cell_beam_ssb->id_SgNB_UE_X2AP_ID = node_4_1->id_sgnb_ue_x2ap_id;
		node_4_2 = node_4_1->id_uenrmeasurement;
		if(node_4_1->id_uenrmeasurement){
			node_4_3 = node_4_2->uenrmeasurements;
			if(node_4_2->uenrmeasurements){
				node_4_4 = node_4_3->ul_dcch_message;
				if(node_4_3->ul_dcch_message){
					node_4_5 = node_4_4->measurementreport;
					if(node_4_4->measurementreport){
						node_4_6 = node_4_5->measurementreport;
						if(node_4_5->measurementreport){
							node_4_7 = node_4_6->measresults;
							if(node_4_6->measresults){
								node_4_8 = node_4_7->measresultservingmolist;
								if(node_4_7->measresultservingmolist){
									for(i_4_9=0;i_4_9<node_4_8->n_items; i_4_9++){
										node_4_9 = node_4_8->items[i_4_9];
										serv_cell_beam_ssb->servCellID = node_4_9->servcellid;
										node_4_10 = node_4_9->measresultservingcell;
										if(node_4_9->measresultservingcell){
											if(node_4_10->physcellid){
												serv_cell_beam_ssb->physCellId = node_4_10->physcellid->value;
												serv_cell_beam_ssb->physCellId_exists = 1;
											}else{
												serv_cell_beam_ssb->physCellId_exists = 0;
											}
											node_4_11 = node_4_10->measresult;
											if(node_4_10->measresult){
												node_4_12 = node_4_11->rsindexresults;
												if(node_4_11->rsindexresults){
													node_4_13 = node_4_12->resultsssb_indexes;
													if(node_4_12->resultsssb_indexes){
														for(i_4_14=0;i_4_14<node_4_13->n_items; i_4_14++){
															node_4_14 = node_4_13->items[i_4_14];
															serv_cell_beam_ssb->ssb_Index = node_4_14->ssb_index;
															node_4_15 = node_4_14->ssb_results;
															if(node_4_14->ssb_results){
																if(node_4_15->rsrq){
																	serv_cell_beam_ssb->rsrq = node_4_15->rsrq->value;
																	serv_cell_beam_ssb->rsrq_exists = 1;
																}else{
																	serv_cell_beam_ssb->rsrq_exists = 0;
																}
																if(node_4_15->rsrp){
																	serv_cell_beam_ssb->rsrp = node_4_15->rsrp->value;
																	serv_cell_beam_ssb->rsrp_exists = 1;
																}else{
																	serv_cell_beam_ssb->rsrp_exists = 0;
																}
																if(node_4_15->sinr){
																	serv_cell_beam_ssb->sinr = node_4_15->sinr->value;
																	serv_cell_beam_ssb->sinr_exists = 1;
																}else{
																	serv_cell_beam_ssb->sinr_exists = 0;
																}
																rts_fta_process_packet(&cur_packet);
															}
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
// --------------------------------------------------
// ---  Specialized processing for .proto rrctransfer.json, path rrc_metrics.json

	neighbor_beam_ssb = (struct _neighbor_beam_ssb *)(cur_packet.record.packed.values);
	cur_packet.schema = 6;
	node_5_0 = node_4_0;
	ts_lo = hdr->header->timestamp & 0xffffffff;
	ts_hi = hdr->header->timestamp >> 32;
	neighbor_beam_ssb->timestamp_ms = (ts_hi - 2208988800) * 1000 + ((ts_lo * 1000) >> 32);
	if(hdr->header->gnbid==NULL)
		neighbor_beam_ssb->gnb_id = empty_string;
	else
		neighbor_beam_ssb->gnb_id = hdr->header->gnbid->value;

	node_5_1 = node_5_0->rrctransfer_ies;
	if(node_5_0->rrctransfer_ies){
		neighbor_beam_ssb->id_MeNB_UE_X2AP_ID = node_5_1->id_menb_ue_x2ap_id;
		neighbor_beam_ssb->id_SgNB_UE_X2AP_ID = node_5_1->id_sgnb_ue_x2ap_id;
		node_5_2 = node_5_1->id_uenrmeasurement;
		if(node_5_1->id_uenrmeasurement){
			node_5_3 = node_5_2->uenrmeasurements;
			if(node_5_2->uenrmeasurements){
				node_5_4 = node_5_3->ul_dcch_message;
				if(node_5_3->ul_dcch_message){
					node_5_5 = node_5_4->measurementreport;
					if(node_5_4->measurementreport){
						node_5_6 = node_5_5->measurementreport;
						if(node_5_5->measurementreport){
							node_5_7 = node_5_6->measresults;
							if(node_5_6->measresults){
								node_5_8 = node_5_7->measresultlistnr;
								if(node_5_7->measresultlistnr){
									for(i_5_9=0;i_5_9<node_5_8->n_items; i_5_9++){
										node_5_9 = node_5_8->items[i_5_9];
										if(node_5_9->physcellid){
											neighbor_beam_ssb->physCellId = node_5_9->physcellid->value;
											neighbor_beam_ssb->physCellId_exists = 1;
										}else{
											neighbor_beam_ssb->physCellId_exists = 0;
										}
										node_5_10 = node_5_9->measresult;
										if(node_5_9->measresult){
											node_5_11 = node_5_10->rsindexresults;
											if(node_5_10->rsindexresults){
												node_5_12 = node_5_11->resultsssb_indexes;
												if(node_5_11->resultsssb_indexes){
													for(i_5_13=0;i_5_13<node_5_12->n_items; i_5_13++){
														node_5_13 = node_5_12->items[i_5_13];
														neighbor_beam_ssb->ssb_Index = node_5_13->ssb_index;
														node_5_14 = node_5_13->ssb_results;
														if(node_5_13->ssb_results){
															if(node_5_14->rsrq){
																neighbor_beam_ssb->rsrq = node_5_14->rsrq->value;
																neighbor_beam_ssb->rsrq_exists = 1;
															}else{
																neighbor_beam_ssb->rsrq_exists = 0;
															}
															if(node_5_14->rsrp){
																neighbor_beam_ssb->rsrp = node_5_14->rsrp->value;
																neighbor_beam_ssb->rsrp_exists = 1;
															}else{
																neighbor_beam_ssb->rsrp_exists = 0;
															}
															if(node_5_14->sinr){
																neighbor_beam_ssb->sinr = node_5_14->sinr->value;
																neighbor_beam_ssb->sinr_exists = 1;
															}else{
																neighbor_beam_ssb->sinr_exists = 0;
															}
															rts_fta_process_packet(&cur_packet);
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	streaming_protobufs__rrctransfer__free_unpacked(node_0_0,NULL);
	return 0;
}

gs_uint32_t process_buffer_ADDREQREJECT(gs_uint8_t * buffer, gs_uint32_t buflen){
	char *empty_string = "";
unsigned long long int ts_lo, ts_hi;
	StreamingProtobufs__X2APStreaming *hdr = NULL;
// ------------------------------------------
// ---  Variables for .proto sgnb_addition_request_reject.json, path sgnb_add_req_reject.json
	struct _sgnb_add_req_reject *sgnb_add_req_reject = NULL;
	StreamingProtobufs__SgNBAdditionRequestReject *node_0_0 = NULL;
	StreamingProtobufs__Cause *node_0_1 = NULL;

// --------------------------------------------------
// ---  Specialized processing for .proto sgnb_addition_request_reject.json, path sgnb_add_req_reject.json

	sgnb_add_req_reject = (struct _sgnb_add_req_reject *)(cur_packet.record.packed.values);
	cur_packet.schema = 701;

	hdr = streaming_protobufs__x2_apstreaming__unpack(NULL, buflen, buffer);
	if(hdr==NULL) return -1;

	node_0_0 = hdr->sgnbadditionrequestreject;
	if(node_0_0==NULL) return -2;
	if(hdr->header==NULL) return -3;

	ts_lo = hdr->header->timestamp & 0xffffffff;
	ts_hi = hdr->header->timestamp >> 32;
	sgnb_add_req_reject->timestamp_ms = (ts_hi - 2208988800) * 1000 + ((ts_lo * 1000) >> 32);
	if(hdr->header->gnbid==NULL)
		sgnb_add_req_reject->gnb_id = empty_string;
	else
		sgnb_add_req_reject->gnb_id = hdr->header->gnbid->value;

	sgnb_add_req_reject->id_MeNB_UE_X2AP_ID = node_0_0->id_menb_ue_x2ap_id;
	if(node_0_0->id_sgnb_ue_x2ap_id){
		sgnb_add_req_reject->id_SgNB_UE_X2AP_ID = node_0_0->id_sgnb_ue_x2ap_id->value;
		sgnb_add_req_reject->id_SgNB_UE_X2AP_ID_exists = 1;
	}else{
		sgnb_add_req_reject->id_SgNB_UE_X2AP_ID_exists = 0;
	}
	node_0_1 = node_0_0->id_cause;
	if(node_0_0->id_cause){
		if(node_0_1->radionetwork){
			sgnb_add_req_reject->cause_radio_network = node_0_1->radionetwork->value;
		}else{
			sgnb_add_req_reject->cause_radio_network = -1;
		}
		if(node_0_1->transport){
			sgnb_add_req_reject->cause_transport = node_0_1->transport->value;
		}else{
			sgnb_add_req_reject->cause_transport = -1;
		}
		if(node_0_1->protocol){
			sgnb_add_req_reject->cause_protocol = node_0_1->protocol->value;
		}else{
			sgnb_add_req_reject->cause_protocol = -1;
		}
		if(node_0_1->misc){
			sgnb_add_req_reject->cause_misc = node_0_1->misc->value;
		}else{
			sgnb_add_req_reject->cause_misc = -1;
		}
		rts_fta_process_packet(&cur_packet);
	}
	streaming_protobufs__sg_nbaddition_request_reject__free_unpacked(node_0_0,NULL);
	return 0;
}

gs_uint32_t process_buffer_SGNB_ADDITION_REQ_ACK(gs_uint8_t * buffer, gs_uint32_t buflen){
	char *empty_string = "";
unsigned long long int ts_lo, ts_hi;
	StreamingProtobufs__X2APStreaming *hdr = NULL;
// ------------------------------------------
// ---  Variables for .proto sgnb_addition_request_acknowledge.json, path sgnb_addition_ack.json
	struct _eRABs_notadmitted_for_ue *eRABs_notadmitted_for_ue = NULL;
	StreamingProtobufs__SgNBAdditionRequestAcknowledge *node_0_0 = NULL;
	StreamingProtobufs__ERABList *node_0_1 = NULL;
	StreamingProtobufs__ERABItemIEs *node_0_2 = NULL;
	gs_uint32_t i_0_2;
	StreamingProtobufs__ERABItem *node_0_3 = NULL;
// ------------------------------------------
// ---  Variables for .proto sgnb_addition_request_acknowledge.json, path sgnb_addition_ack.json
	struct _add_req_ack_cellid *add_req_ack_cellid = NULL;
	StreamingProtobufs__SgNBAdditionRequestAcknowledge *node_1_0 = NULL;
	StreamingProtobufs__CGConfig *node_1_1 = NULL;
	StreamingProtobufs__CGConfigCriticalExtensionsChoice1 *node_1_2 = NULL;
	StreamingProtobufs__CGConfigIEs *node_1_3 = NULL;
	StreamingProtobufs__RRCReconfiguration *node_1_4 = NULL;
	StreamingProtobufs__RRCReconfigurationIEs *node_1_5 = NULL;
	StreamingProtobufs__CellGroupConfig *node_1_6 = NULL;
	StreamingProtobufs__SpCellConfig *node_1_7 = NULL;
	StreamingProtobufs__ReconfigurationWithSync *node_1_8 = NULL;
	StreamingProtobufs__ServingCellConfigCommon *node_1_9 = NULL;
// ------------------------------------------
// ---  Variables for .proto sgnb_addition_request_acknowledge.json, path sgnb_addition_ack.json
	struct _eRABs_acked_for_admit_for_ue *eRABs_acked_for_admit_for_ue = NULL;
	StreamingProtobufs__SgNBAdditionRequestAcknowledge *node_2_0 = NULL;
	StreamingProtobufs__ERABsAdmittedToBeAddedSgNBAddReqAckList *node_2_1 = NULL;
	StreamingProtobufs__ERABsAdmittedToBeAddedSgNBAddReqAckItem *node_2_2 = NULL;
	gs_uint32_t i_2_2;
// ------------------------------------------
// ---  Variables for .proto sgnb_addition_request_acknowledge.json, path sgnb_addition_ack.json
	struct _SgNB_ack_for_ue_NRfreqs *SgNB_ack_for_ue_NRfreqs = NULL;
	StreamingProtobufs__SgNBAdditionRequestAcknowledge *node_3_0 = NULL;
	StreamingProtobufs__CGConfig *node_3_1 = NULL;
	StreamingProtobufs__CGConfigCriticalExtensionsChoice1 *node_3_2 = NULL;
	StreamingProtobufs__CGConfigIEs *node_3_3 = NULL;
// ------------------------------------------
// ---  Variables for .proto sgnb_addition_request_acknowledge.json, path sgnb_addition_ack.json
	struct _SgNB_ack_for_add_mod_for_ue *SgNB_ack_for_add_mod_for_ue = NULL;
	StreamingProtobufs__SgNBAdditionRequestAcknowledge *node_4_0 = NULL;
	StreamingProtobufs__CGConfig *node_4_1 = NULL;
	StreamingProtobufs__CGConfigCriticalExtensionsChoice1 *node_4_2 = NULL;
	StreamingProtobufs__CGConfigIEs *node_4_3 = NULL;
	StreamingProtobufs__RadioBearerConfig *node_4_4 = NULL;
	StreamingProtobufs__DRBToAddModList *node_4_5 = NULL;
	StreamingProtobufs__DRBToAddMod *node_4_6 = NULL;
	gs_uint32_t i_4_6;
// ------------------------------------------
// ---  Variables for .proto sgnb_addition_request_acknowledge.json, path sgnb_addition_ack.json
	struct _SgNB_ack_for_ue_measurements *SgNB_ack_for_ue_measurements = NULL;
	StreamingProtobufs__SgNBAdditionRequestAcknowledge *node_5_0 = NULL;
	StreamingProtobufs__CGConfig *node_5_1 = NULL;
	StreamingProtobufs__CGConfigCriticalExtensionsChoice1 *node_5_2 = NULL;
	StreamingProtobufs__CGConfigIEs *node_5_3 = NULL;
	StreamingProtobufs__MeasResultList2NR *node_5_4 = NULL;
	StreamingProtobufs__MeasResult2NR *node_5_5 = NULL;
	gs_uint32_t i_5_5;
	StreamingProtobufs__MeasResultNR *node_5_6 = NULL;
	StreamingProtobufs__MeasResult *node_5_7 = NULL;
	StreamingProtobufs__CellResults *node_5_8 = NULL;
	StreamingProtobufs__MeasQuantityResults *node_5_9 = NULL;
// ------------------------------------------
// ---  Variables for .proto sgnb_addition_request_acknowledge.json, path sgnb_addition_ack.json
	struct _SgNB_ack_for_ue_beam_csi *SgNB_ack_for_ue_beam_csi = NULL;
	StreamingProtobufs__SgNBAdditionRequestAcknowledge *node_6_0 = NULL;
	StreamingProtobufs__CGConfig *node_6_1 = NULL;
	StreamingProtobufs__CGConfigCriticalExtensionsChoice1 *node_6_2 = NULL;
	StreamingProtobufs__CGConfigIEs *node_6_3 = NULL;
	StreamingProtobufs__MeasResultList2NR *node_6_4 = NULL;
	StreamingProtobufs__MeasResult2NR *node_6_5 = NULL;
	gs_uint32_t i_6_5;
	StreamingProtobufs__MeasResultNR *node_6_6 = NULL;
	StreamingProtobufs__MeasResult *node_6_7 = NULL;
	StreamingProtobufs__RsIndexResults *node_6_8 = NULL;
	StreamingProtobufs__ResultsPerCSIRSIndexList *node_6_9 = NULL;
	StreamingProtobufs__ResultsPerCSIRSIndex *node_6_10 = NULL;
	gs_uint32_t i_6_10;
	StreamingProtobufs__MeasQuantityResults *node_6_11 = NULL;
// ------------------------------------------
// ---  Variables for .proto sgnb_addition_request_acknowledge.json, path sgnb_addition_ack.json
	struct _SgNB_ack_for_ue_beam_ssb *SgNB_ack_for_ue_beam_ssb = NULL;
	StreamingProtobufs__SgNBAdditionRequestAcknowledge *node_7_0 = NULL;
	StreamingProtobufs__CGConfig *node_7_1 = NULL;
	StreamingProtobufs__CGConfigCriticalExtensionsChoice1 *node_7_2 = NULL;
	StreamingProtobufs__CGConfigIEs *node_7_3 = NULL;
	StreamingProtobufs__MeasResultList2NR *node_7_4 = NULL;
	StreamingProtobufs__MeasResult2NR *node_7_5 = NULL;
	gs_uint32_t i_7_5;
	StreamingProtobufs__MeasResultNR *node_7_6 = NULL;
	StreamingProtobufs__MeasResult *node_7_7 = NULL;
	StreamingProtobufs__RsIndexResults *node_7_8 = NULL;
	StreamingProtobufs__ResultsPerSSBIndexList *node_7_9 = NULL;
	StreamingProtobufs__ResultsPerSSBIndex *node_7_10 = NULL;
	gs_uint32_t i_7_10;
	StreamingProtobufs__MeasQuantityResults *node_7_11 = NULL;

// --------------------------------------------------
// ---  Specialized processing for .proto sgnb_addition_request_acknowledge.json, path sgnb_addition_ack.json

	eRABs_notadmitted_for_ue = (struct _eRABs_notadmitted_for_ue *)(cur_packet.record.packed.values);
	cur_packet.schema = 501;

	hdr = streaming_protobufs__x2_apstreaming__unpack(NULL, buflen, buffer);
	if(hdr==NULL) return -1;

	node_0_0 = hdr->sgnbadditionrequestacknowledge;
	if(node_0_0==NULL) return -2;
	if(hdr->header==NULL) return -3;

	ts_lo = hdr->header->timestamp & 0xffffffff;
	ts_hi = hdr->header->timestamp >> 32;
	eRABs_notadmitted_for_ue->timestamp_ms = (ts_hi - 2208988800) * 1000 + ((ts_lo * 1000) >> 32);
	if(hdr->header->gnbid==NULL)
		eRABs_notadmitted_for_ue->gnb_id = empty_string;
	else
		eRABs_notadmitted_for_ue->gnb_id = hdr->header->gnbid->value;

	eRABs_notadmitted_for_ue->id_MeNB_UE_X2AP_ID = node_0_0->id_menb_ue_x2ap_id;
	eRABs_notadmitted_for_ue->id_SgNB_UE_X2AP_ID = node_0_0->id_sgnb_ue_x2ap_id;
	if(node_0_0->id_menb_ue_x2ap_id_extension){
		eRABs_notadmitted_for_ue->id_MeNB_UE_X2AP_ID_Extension = node_0_0->id_menb_ue_x2ap_id_extension->value;
	}else{
		eRABs_notadmitted_for_ue->id_MeNB_UE_X2AP_ID_Extension = 0;
	}
	node_0_1 = node_0_0->id_e_rabs_notadmitted_list;
	if(node_0_0->id_e_rabs_notadmitted_list){
		for(i_0_2=0;i_0_2<node_0_1->n_items; i_0_2++){
			node_0_2 = node_0_1->items[i_0_2];
			node_0_3 = node_0_2->id_e_rab_item;
			if(node_0_2->id_e_rab_item){
				eRABs_notadmitted_for_ue->e_RAB_ID = node_0_3->e_rab_id;
				if(node_0_3->cause && node_0_3->cause->transport){
					eRABs_notadmitted_for_ue->cause_transport = node_0_3->cause->transport->value;
				}else{
					eRABs_notadmitted_for_ue->cause_transport = -1;
				}
				if(node_0_3->cause && node_0_3->cause->protocol){
					eRABs_notadmitted_for_ue->cause_protocol = node_0_3->cause->protocol->value;
				}else{
					eRABs_notadmitted_for_ue->cause_protocol = -1;
				}
				if(node_0_3->cause && node_0_3->cause->misc){
					eRABs_notadmitted_for_ue->cause_misc = node_0_3->cause->misc->value;
				}else{
					eRABs_notadmitted_for_ue->cause_misc = -1;
				}
				if(node_0_3->cause && node_0_3->cause->radionetwork){
					eRABs_notadmitted_for_ue->cause_radio_network = node_0_3->cause->radionetwork->value;
				}else{
					eRABs_notadmitted_for_ue->cause_radio_network = -1;
				}
				rts_fta_process_packet(&cur_packet);
			}
		}
	}
// --------------------------------------------------
// ---  Specialized processing for .proto sgnb_addition_request_acknowledge.json, path sgnb_addition_ack.json

	add_req_ack_cellid = (struct _add_req_ack_cellid *)(cur_packet.record.packed.values);
	cur_packet.schema = 10000;
	node_1_0 = node_0_0;
	ts_lo = hdr->header->timestamp & 0xffffffff;
	ts_hi = hdr->header->timestamp >> 32;
	add_req_ack_cellid->timestamp_ms = (ts_hi - 2208988800) * 1000 + ((ts_lo * 1000) >> 32);
	if(hdr->header->gnbid==NULL)
		add_req_ack_cellid->gnb_id = empty_string;
	else
		add_req_ack_cellid->gnb_id = hdr->header->gnbid->value;

	add_req_ack_cellid->id_MeNB_UE_X2AP_ID = node_1_0->id_menb_ue_x2ap_id;
	add_req_ack_cellid->id_SgNB_UE_X2AP_ID = node_1_0->id_sgnb_ue_x2ap_id;
	if(node_1_0->id_menb_ue_x2ap_id_extension){
		add_req_ack_cellid->id_MeNB_UE_X2AP_ID_Extension = node_1_0->id_menb_ue_x2ap_id_extension->value;
	}else{
		add_req_ack_cellid->id_MeNB_UE_X2AP_ID_Extension = 0;
	}
	node_1_1 = node_1_0->id_sgnbtomenbcontainer;
	if(node_1_0->id_sgnbtomenbcontainer){
		node_1_2 = node_1_1->criticalextensionschoice1;
		if(node_1_1->criticalextensionschoice1){
			node_1_3 = node_1_2->protocolies;
			if(node_1_2->protocolies){
				node_1_4 = node_1_3->scg_cellgroupconfig;
				if(node_1_3->scg_cellgroupconfig){
					node_1_5 = node_1_4->rrcreconfiguration;
					if(node_1_4->rrcreconfiguration){
						node_1_6 = node_1_5->secondarycellgroup;
						if(node_1_5->secondarycellgroup){
							node_1_7 = node_1_6->spcellconfig;
							if(node_1_6->spcellconfig){
								node_1_8 = node_1_7->reconfigurationwithsync;
								if(node_1_7->reconfigurationwithsync){
									node_1_9 = node_1_8->spcellconfigcommon;
									if(node_1_8->spcellconfigcommon){
										if(node_1_9->physcellid){
											add_req_ack_cellid->physCellId = node_1_9->physcellid->value;
											add_req_ack_cellid->physCellId_exists = 1;
										}else{
											add_req_ack_cellid->physCellId_exists = 0;
										}
										rts_fta_process_packet(&cur_packet);
									}
								}
							}
						}
					}
				}
			}
		}
	}
// --------------------------------------------------
// ---  Specialized processing for .proto sgnb_addition_request_acknowledge.json, path sgnb_addition_ack.json

	eRABs_acked_for_admit_for_ue = (struct _eRABs_acked_for_admit_for_ue *)(cur_packet.record.packed.values);
	cur_packet.schema = 502;
	node_2_0 = node_1_0;
	ts_lo = hdr->header->timestamp & 0xffffffff;
	ts_hi = hdr->header->timestamp >> 32;
	eRABs_acked_for_admit_for_ue->timestamp_ms = (ts_hi - 2208988800) * 1000 + ((ts_lo * 1000) >> 32);
	if(hdr->header->gnbid==NULL)
		eRABs_acked_for_admit_for_ue->gnb_id = empty_string;
	else
		eRABs_acked_for_admit_for_ue->gnb_id = hdr->header->gnbid->value;

	eRABs_acked_for_admit_for_ue->id_MeNB_UE_X2AP_ID = node_2_0->id_menb_ue_x2ap_id;
	eRABs_acked_for_admit_for_ue->id_SgNB_UE_X2AP_ID = node_2_0->id_sgnb_ue_x2ap_id;
	if(node_2_0->id_menb_ue_x2ap_id_extension){
		eRABs_acked_for_admit_for_ue->id_MeNB_UE_X2AP_ID_Extension = node_2_0->id_menb_ue_x2ap_id_extension->value;
	}else{
		eRABs_acked_for_admit_for_ue->id_MeNB_UE_X2AP_ID_Extension = 0;
	}
	node_2_1 = node_2_0->id_e_rabs_admitted_tobeadded_sgnbaddreqacklist;
	if(node_2_0->id_e_rabs_admitted_tobeadded_sgnbaddreqacklist){
		for(i_2_2=0;i_2_2<node_2_1->n_id_e_rabs_admitted_tobeadded_sgnbaddreqack_item; i_2_2++){
			node_2_2 = node_2_1->id_e_rabs_admitted_tobeadded_sgnbaddreqack_item[i_2_2];
			if(node_2_2->sgnbpdcppresent && node_2_2->sgnbpdcppresent->mcg_e_rab_level_qos_parameters && node_2_2->sgnbpdcppresent->mcg_e_rab_level_qos_parameters->allocationandretentionpriority){
				eRABs_acked_for_admit_for_ue->ARP = node_2_2->sgnbpdcppresent->mcg_e_rab_level_qos_parameters->allocationandretentionpriority->prioritylevel;
			}else{
				eRABs_acked_for_admit_for_ue->ARP = 0;
			}
			if(node_2_2->sgnbpdcppresent && node_2_2->sgnbpdcppresent->dl_forwarding_gtptunnelendpoint){
				eRABs_acked_for_admit_for_ue->gTP_TEID_dl = node_2_2->sgnbpdcppresent->dl_forwarding_gtptunnelendpoint->gtp_teid;
				eRABs_acked_for_admit_for_ue->gTP_TEID_dl_exists = 1;
			}else{
				eRABs_acked_for_admit_for_ue->gTP_TEID_dl_exists = 0;
			}
			if(node_2_2->en_dc_resourceconfiguration){
				eRABs_acked_for_admit_for_ue->mCGresources = node_2_2->en_dc_resourceconfiguration->mcgresources;
				eRABs_acked_for_admit_for_ue->mCGresources_exists = 1;
			}else{
				eRABs_acked_for_admit_for_ue->mCGresources_exists = 0;
			}
			if(node_2_2->sgnbpdcppresent && node_2_2->sgnbpdcppresent->dl_forwarding_gtptunnelendpoint){
				eRABs_acked_for_admit_for_ue->transportLayerAddress_dl = node_2_2->sgnbpdcppresent->dl_forwarding_gtptunnelendpoint->transportlayeraddress;
				eRABs_acked_for_admit_for_ue->transportLayerAddress_dl_exists = 1;
			}else{
				eRABs_acked_for_admit_for_ue->transportLayerAddress_dl_exists = 0;
			}
			if(node_2_2->en_dc_resourceconfiguration){
				eRABs_acked_for_admit_for_ue->pDCPatSgNB = node_2_2->en_dc_resourceconfiguration->pdcpatsgnb;
				eRABs_acked_for_admit_for_ue->pDCPatSgNB_exists = 1;
			}else{
				eRABs_acked_for_admit_for_ue->pDCPatSgNB_exists = 0;
			}
			if(node_2_2->en_dc_resourceconfiguration){
				eRABs_acked_for_admit_for_ue->sCGresources = node_2_2->en_dc_resourceconfiguration->scgresources;
				eRABs_acked_for_admit_for_ue->sCGresources_exists = 1;
			}else{
				eRABs_acked_for_admit_for_ue->sCGresources_exists = 0;
			}
			eRABs_acked_for_admit_for_ue->e_RAB_ID = node_2_2->e_rab_id;
			if(node_2_2->sgnbpdcppresent && node_2_2->sgnbpdcppresent->mcg_e_rab_level_qos_parameters){
				eRABs_acked_for_admit_for_ue->qCI = node_2_2->sgnbpdcppresent->mcg_e_rab_level_qos_parameters->qci;
			}else{
				eRABs_acked_for_admit_for_ue->qCI = 0;
			}
			rts_fta_process_packet(&cur_packet);
		}
	}
// --------------------------------------------------
// ---  Specialized processing for .proto sgnb_addition_request_acknowledge.json, path sgnb_addition_ack.json

	SgNB_ack_for_ue_NRfreqs = (struct _SgNB_ack_for_ue_NRfreqs *)(cur_packet.record.packed.values);
	cur_packet.schema = 503;
	node_3_0 = node_2_0;
	ts_lo = hdr->header->timestamp & 0xffffffff;
	ts_hi = hdr->header->timestamp >> 32;
	SgNB_ack_for_ue_NRfreqs->timestamp_ms = (ts_hi - 2208988800) * 1000 + ((ts_lo * 1000) >> 32);
	if(hdr->header->gnbid==NULL)
		SgNB_ack_for_ue_NRfreqs->gnb_id = empty_string;
	else
		SgNB_ack_for_ue_NRfreqs->gnb_id = hdr->header->gnbid->value;

	SgNB_ack_for_ue_NRfreqs->id_MeNB_UE_X2AP_ID = node_3_0->id_menb_ue_x2ap_id;
	SgNB_ack_for_ue_NRfreqs->id_SgNB_UE_X2AP_ID = node_3_0->id_sgnb_ue_x2ap_id;
	if(node_3_0->id_menb_ue_x2ap_id_extension){
		SgNB_ack_for_ue_NRfreqs->id_MeNB_UE_X2AP_ID_Extension = node_3_0->id_menb_ue_x2ap_id_extension->value;
	}else{
		SgNB_ack_for_ue_NRfreqs->id_MeNB_UE_X2AP_ID_Extension = 0;
	}
	node_3_1 = node_3_0->id_sgnbtomenbcontainer;
	if(node_3_0->id_sgnbtomenbcontainer){
		node_3_2 = node_3_1->criticalextensionschoice1;
		if(node_3_1->criticalextensionschoice1){
			node_3_3 = node_3_2->protocolies;
			if(node_3_2->protocolies){
				if(node_3_3->measconfigsn && node_3_3->measconfigsn->measuredfrequenciessn && node_3_3->measconfigsn->n_measuredfrequenciessn > 0 && node_3_3->measconfigsn->measuredfrequenciessn[0]->measuredfrequency){
					SgNB_ack_for_ue_NRfreqs->measuredFrequenciesSN0 = node_3_3->measconfigsn->measuredfrequenciessn[0]->measuredfrequency->value;
					SgNB_ack_for_ue_NRfreqs->measuredFrequenciesSN0_exists = 1;
				}else{
					SgNB_ack_for_ue_NRfreqs->measuredFrequenciesSN0_exists = 0;
				}
				if(node_3_3->measconfigsn && node_3_3->measconfigsn->measuredfrequenciessn && node_3_3->measconfigsn->n_measuredfrequenciessn > 1 && node_3_3->measconfigsn->measuredfrequenciessn[1]->measuredfrequency){
					SgNB_ack_for_ue_NRfreqs->measuredFrequenciesSN1 = node_3_3->measconfigsn->measuredfrequenciessn[1]->measuredfrequency->value;
					SgNB_ack_for_ue_NRfreqs->measuredFrequenciesSN1_exists = 1;
				}else{
					SgNB_ack_for_ue_NRfreqs->measuredFrequenciesSN1_exists = 0;
				}
				if(node_3_3->measconfigsn && node_3_3->measconfigsn->measuredfrequenciessn && node_3_3->measconfigsn->n_measuredfrequenciessn > 2 && node_3_3->measconfigsn->measuredfrequenciessn[2]->measuredfrequency){
					SgNB_ack_for_ue_NRfreqs->measuredFrequenciesSN2 = node_3_3->measconfigsn->measuredfrequenciessn[2]->measuredfrequency->value;
					SgNB_ack_for_ue_NRfreqs->measuredFrequenciesSN2_exists = 1;
				}else{
					SgNB_ack_for_ue_NRfreqs->measuredFrequenciesSN2_exists = 0;
				}
				if(node_3_3->measconfigsn && node_3_3->measconfigsn->measuredfrequenciessn && node_3_3->measconfigsn->n_measuredfrequenciessn > 3 && node_3_3->measconfigsn->measuredfrequenciessn[3]->measuredfrequency){
					SgNB_ack_for_ue_NRfreqs->measuredFrequenciesSN3 = node_3_3->measconfigsn->measuredfrequenciessn[3]->measuredfrequency->value;
					SgNB_ack_for_ue_NRfreqs->measuredFrequenciesSN3_exists = 1;
				}else{
					SgNB_ack_for_ue_NRfreqs->measuredFrequenciesSN3_exists = 0;
				}
				if(node_3_3->measconfigsn && node_3_3->measconfigsn->measuredfrequenciessn && node_3_3->measconfigsn->n_measuredfrequenciessn > 4 && node_3_3->measconfigsn->measuredfrequenciessn[4]->measuredfrequency){
					SgNB_ack_for_ue_NRfreqs->measuredFrequenciesSN4 = node_3_3->measconfigsn->measuredfrequenciessn[4]->measuredfrequency->value;
					SgNB_ack_for_ue_NRfreqs->measuredFrequenciesSN4_exists = 1;
				}else{
					SgNB_ack_for_ue_NRfreqs->measuredFrequenciesSN4_exists = 0;
				}
				if(node_3_3->measconfigsn && node_3_3->measconfigsn->measuredfrequenciessn && node_3_3->measconfigsn->n_measuredfrequenciessn > 5 && node_3_3->measconfigsn->measuredfrequenciessn[5]->measuredfrequency){
					SgNB_ack_for_ue_NRfreqs->measuredFrequenciesSN5 = node_3_3->measconfigsn->measuredfrequenciessn[5]->measuredfrequency->value;
					SgNB_ack_for_ue_NRfreqs->measuredFrequenciesSN5_exists = 1;
				}else{
					SgNB_ack_for_ue_NRfreqs->measuredFrequenciesSN5_exists = 0;
				}
				if(node_3_3->measconfigsn && node_3_3->measconfigsn->measuredfrequenciessn && node_3_3->measconfigsn->n_measuredfrequenciessn > 6 && node_3_3->measconfigsn->measuredfrequenciessn[6]->measuredfrequency){
					SgNB_ack_for_ue_NRfreqs->measuredFrequenciesSN6 = node_3_3->measconfigsn->measuredfrequenciessn[6]->measuredfrequency->value;
					SgNB_ack_for_ue_NRfreqs->measuredFrequenciesSN6_exists = 1;
				}else{
					SgNB_ack_for_ue_NRfreqs->measuredFrequenciesSN6_exists = 0;
				}
				if(node_3_3->measconfigsn && node_3_3->measconfigsn->measuredfrequenciessn && node_3_3->measconfigsn->n_measuredfrequenciessn > 7 && node_3_3->measconfigsn->measuredfrequenciessn[7]->measuredfrequency){
					SgNB_ack_for_ue_NRfreqs->measuredFrequenciesSN7 = node_3_3->measconfigsn->measuredfrequenciessn[7]->measuredfrequency->value;
					SgNB_ack_for_ue_NRfreqs->measuredFrequenciesSN7_exists = 1;
				}else{
					SgNB_ack_for_ue_NRfreqs->measuredFrequenciesSN7_exists = 0;
				}
				if(node_3_3->candidateservingfreqlistnr && node_3_3->candidateservingfreqlistnr->n_items > 0){
					SgNB_ack_for_ue_NRfreqs->candidate_serving_cell_freqs0 = node_3_3->candidateservingfreqlistnr->items[0];
					SgNB_ack_for_ue_NRfreqs->candidate_serving_cell_freqs0_exists = 1;
				}else{
					SgNB_ack_for_ue_NRfreqs->candidate_serving_cell_freqs0_exists = 0;
				}
				if(node_3_3->candidateservingfreqlistnr && node_3_3->candidateservingfreqlistnr->n_items > 1){
					SgNB_ack_for_ue_NRfreqs->candidate_serving_cell_freqs1 = node_3_3->candidateservingfreqlistnr->items[1];
					SgNB_ack_for_ue_NRfreqs->candidate_serving_cell_freqs1_exists = 1;
				}else{
					SgNB_ack_for_ue_NRfreqs->candidate_serving_cell_freqs1_exists = 0;
				}
				if(node_3_3->candidateservingfreqlistnr && node_3_3->candidateservingfreqlistnr->n_items > 2){
					SgNB_ack_for_ue_NRfreqs->candidate_serving_cell_freqs2 = node_3_3->candidateservingfreqlistnr->items[2];
					SgNB_ack_for_ue_NRfreqs->candidate_serving_cell_freqs2_exists = 1;
				}else{
					SgNB_ack_for_ue_NRfreqs->candidate_serving_cell_freqs2_exists = 0;
				}
				if(node_3_3->candidateservingfreqlistnr && node_3_3->candidateservingfreqlistnr->n_items > 3){
					SgNB_ack_for_ue_NRfreqs->candidate_serving_cell_freqs3 = node_3_3->candidateservingfreqlistnr->items[3];
					SgNB_ack_for_ue_NRfreqs->candidate_serving_cell_freqs3_exists = 1;
				}else{
					SgNB_ack_for_ue_NRfreqs->candidate_serving_cell_freqs3_exists = 0;
				}
				if(node_3_3->candidateservingfreqlistnr && node_3_3->candidateservingfreqlistnr->n_items > 4){
					SgNB_ack_for_ue_NRfreqs->candidate_serving_cell_freqs4 = node_3_3->candidateservingfreqlistnr->items[4];
					SgNB_ack_for_ue_NRfreqs->candidate_serving_cell_freqs4_exists = 1;
				}else{
					SgNB_ack_for_ue_NRfreqs->candidate_serving_cell_freqs4_exists = 0;
				}
				if(node_3_3->candidateservingfreqlistnr && node_3_3->candidateservingfreqlistnr->n_items > 5){
					SgNB_ack_for_ue_NRfreqs->candidate_serving_cell_freqs5 = node_3_3->candidateservingfreqlistnr->items[5];
					SgNB_ack_for_ue_NRfreqs->candidate_serving_cell_freqs5_exists = 1;
				}else{
					SgNB_ack_for_ue_NRfreqs->candidate_serving_cell_freqs5_exists = 0;
				}
				if(node_3_3->candidateservingfreqlistnr && node_3_3->candidateservingfreqlistnr->n_items > 6){
					SgNB_ack_for_ue_NRfreqs->candidate_serving_cell_freqs6 = node_3_3->candidateservingfreqlistnr->items[6];
					SgNB_ack_for_ue_NRfreqs->candidate_serving_cell_freqs6_exists = 1;
				}else{
					SgNB_ack_for_ue_NRfreqs->candidate_serving_cell_freqs6_exists = 0;
				}
				if(node_3_3->candidateservingfreqlistnr && node_3_3->candidateservingfreqlistnr->n_items > 7){
					SgNB_ack_for_ue_NRfreqs->candidate_serving_cell_freqs7 = node_3_3->candidateservingfreqlistnr->items[7];
					SgNB_ack_for_ue_NRfreqs->candidate_serving_cell_freqs7_exists = 1;
				}else{
					SgNB_ack_for_ue_NRfreqs->candidate_serving_cell_freqs7_exists = 0;
				}
				rts_fta_process_packet(&cur_packet);
			}
		}
	}
// --------------------------------------------------
// ---  Specialized processing for .proto sgnb_addition_request_acknowledge.json, path sgnb_addition_ack.json

	SgNB_ack_for_add_mod_for_ue = (struct _SgNB_ack_for_add_mod_for_ue *)(cur_packet.record.packed.values);
	cur_packet.schema = 504;
	node_4_0 = node_3_0;
	ts_lo = hdr->header->timestamp & 0xffffffff;
	ts_hi = hdr->header->timestamp >> 32;
	SgNB_ack_for_add_mod_for_ue->timestamp_ms = (ts_hi - 2208988800) * 1000 + ((ts_lo * 1000) >> 32);
	if(hdr->header->gnbid==NULL)
		SgNB_ack_for_add_mod_for_ue->gnb_id = empty_string;
	else
		SgNB_ack_for_add_mod_for_ue->gnb_id = hdr->header->gnbid->value;

	SgNB_ack_for_add_mod_for_ue->id_MeNB_UE_X2AP_ID = node_4_0->id_menb_ue_x2ap_id;
	SgNB_ack_for_add_mod_for_ue->id_SgNB_UE_X2AP_ID = node_4_0->id_sgnb_ue_x2ap_id;
	if(node_4_0->id_menb_ue_x2ap_id_extension){
		SgNB_ack_for_add_mod_for_ue->id_MeNB_UE_X2AP_ID_Extension = node_4_0->id_menb_ue_x2ap_id_extension->value;
	}else{
		SgNB_ack_for_add_mod_for_ue->id_MeNB_UE_X2AP_ID_Extension = 0;
	}
	node_4_1 = node_4_0->id_sgnbtomenbcontainer;
	if(node_4_0->id_sgnbtomenbcontainer){
		node_4_2 = node_4_1->criticalextensionschoice1;
		if(node_4_1->criticalextensionschoice1){
			node_4_3 = node_4_2->protocolies;
			if(node_4_2->protocolies){
				node_4_4 = node_4_3->scg_rb_config;
				if(node_4_3->scg_rb_config){
					if(node_4_4->drb_toreleaselist && node_4_4->drb_toreleaselist->n_items > 0){
						SgNB_ack_for_add_mod_for_ue->toRelease0 = node_4_4->drb_toreleaselist->items[0];
						SgNB_ack_for_add_mod_for_ue->toRelease0_exists = 1;
					}else{
						SgNB_ack_for_add_mod_for_ue->toRelease0_exists = 0;
					}
					if(node_4_4->drb_toreleaselist && node_4_4->drb_toreleaselist->n_items > 1){
						SgNB_ack_for_add_mod_for_ue->toRelease1 = node_4_4->drb_toreleaselist->items[1];
						SgNB_ack_for_add_mod_for_ue->toRelease1_exists = 1;
					}else{
						SgNB_ack_for_add_mod_for_ue->toRelease1_exists = 0;
					}
					if(node_4_4->drb_toreleaselist && node_4_4->drb_toreleaselist->n_items > 2){
						SgNB_ack_for_add_mod_for_ue->toRelease2 = node_4_4->drb_toreleaselist->items[2];
						SgNB_ack_for_add_mod_for_ue->toRelease2_exists = 1;
					}else{
						SgNB_ack_for_add_mod_for_ue->toRelease2_exists = 0;
					}
					if(node_4_4->drb_toreleaselist && node_4_4->drb_toreleaselist->n_items > 3){
						SgNB_ack_for_add_mod_for_ue->toRelease3 = node_4_4->drb_toreleaselist->items[3];
						SgNB_ack_for_add_mod_for_ue->toRelease3_exists = 1;
					}else{
						SgNB_ack_for_add_mod_for_ue->toRelease3_exists = 0;
					}
					node_4_5 = node_4_4->drb_toaddmodlist;
					if(node_4_4->drb_toaddmodlist){
						for(i_4_6=0;i_4_6<node_4_5->n_items; i_4_6++){
							node_4_6 = node_4_5->items[i_4_6];
							if(node_4_6->recoverpdcp){
								SgNB_ack_for_add_mod_for_ue->recoverPDCP = node_4_6->recoverpdcp->value;
								SgNB_ack_for_add_mod_for_ue->recoverPDCP_exists = 1;
							}else{
								SgNB_ack_for_add_mod_for_ue->recoverPDCP_exists = 0;
							}
							if(node_4_6->reestablishpdcp){
								SgNB_ack_for_add_mod_for_ue->reestablishPDCP = node_4_6->reestablishpdcp->value;
								SgNB_ack_for_add_mod_for_ue->reestablishPDCP_exists = 1;
							}else{
								SgNB_ack_for_add_mod_for_ue->reestablishPDCP_exists = 0;
							}
							SgNB_ack_for_add_mod_for_ue->drb_Identity = node_4_6->drb_identity;
							SgNB_ack_for_add_mod_for_ue->eps_BearerIdentity = node_4_6->eps_beareridentity;
							rts_fta_process_packet(&cur_packet);
						}
					}
				}
			}
		}
	}
// --------------------------------------------------
// ---  Specialized processing for .proto sgnb_addition_request_acknowledge.json, path sgnb_addition_ack.json

	SgNB_ack_for_ue_measurements = (struct _SgNB_ack_for_ue_measurements *)(cur_packet.record.packed.values);
	cur_packet.schema = 505;
	node_5_0 = node_4_0;
	ts_lo = hdr->header->timestamp & 0xffffffff;
	ts_hi = hdr->header->timestamp >> 32;
	SgNB_ack_for_ue_measurements->timestamp_ms = (ts_hi - 2208988800) * 1000 + ((ts_lo * 1000) >> 32);
	if(hdr->header->gnbid==NULL)
		SgNB_ack_for_ue_measurements->gnb_id = empty_string;
	else
		SgNB_ack_for_ue_measurements->gnb_id = hdr->header->gnbid->value;

	SgNB_ack_for_ue_measurements->id_MeNB_UE_X2AP_ID = node_5_0->id_menb_ue_x2ap_id;
	SgNB_ack_for_ue_measurements->id_SgNB_UE_X2AP_ID = node_5_0->id_sgnb_ue_x2ap_id;
	if(node_5_0->id_menb_ue_x2ap_id_extension){
		SgNB_ack_for_ue_measurements->id_MeNB_UE_X2AP_ID_Extension = node_5_0->id_menb_ue_x2ap_id_extension->value;
	}else{
		SgNB_ack_for_ue_measurements->id_MeNB_UE_X2AP_ID_Extension = 0;
	}
	node_5_1 = node_5_0->id_sgnbtomenbcontainer;
	if(node_5_0->id_sgnbtomenbcontainer){
		node_5_2 = node_5_1->criticalextensionschoice1;
		if(node_5_1->criticalextensionschoice1){
			node_5_3 = node_5_2->protocolies;
			if(node_5_2->protocolies){
				node_5_4 = node_5_3->candidatecellinfolistsn;
				if(node_5_3->candidatecellinfolistsn){
					for(i_5_5=0;i_5_5<node_5_4->n_items; i_5_5++){
						node_5_5 = node_5_4->items[i_5_5];
						if(node_5_5->ssbfrequency){
							SgNB_ack_for_ue_measurements->ssbFrequency = node_5_5->ssbfrequency->value;
							SgNB_ack_for_ue_measurements->ssbFrequency_exists = 1;
						}else{
							SgNB_ack_for_ue_measurements->ssbFrequency_exists = 0;
						}
						if(node_5_5->reffreqcsi_rs){
							SgNB_ack_for_ue_measurements->refFreqCSI_RS = node_5_5->reffreqcsi_rs->value;
							SgNB_ack_for_ue_measurements->refFreqCSI_RS_exists = 1;
						}else{
							SgNB_ack_for_ue_measurements->refFreqCSI_RS_exists = 0;
						}
						node_5_6 = node_5_5->measresultservingcell;
						if(node_5_5->measresultservingcell){
							if(node_5_6->physcellid){
								SgNB_ack_for_ue_measurements->physCellId = node_5_6->physcellid->value;
								SgNB_ack_for_ue_measurements->physCellId_exists = 1;
							}else{
								SgNB_ack_for_ue_measurements->physCellId_exists = 0;
							}
							node_5_7 = node_5_6->measresult;
							if(node_5_6->measresult){
								node_5_8 = node_5_7->cellresults;
								if(node_5_7->cellresults){
									node_5_9 = node_5_8->resultscsi_rs_cell;
									if(node_5_8->resultscsi_rs_cell){
										if(node_5_9->rsrq){
											SgNB_ack_for_ue_measurements->rsrq = node_5_9->rsrq->value;
											SgNB_ack_for_ue_measurements->rsrq_exists = 1;
										}else{
											SgNB_ack_for_ue_measurements->rsrq_exists = 0;
										}
										if(node_5_9->rsrp){
											SgNB_ack_for_ue_measurements->rsrp = node_5_9->rsrp->value;
											SgNB_ack_for_ue_measurements->rsrp_exists = 1;
										}else{
											SgNB_ack_for_ue_measurements->rsrp_exists = 0;
										}
										if(node_5_9->sinr){
											SgNB_ack_for_ue_measurements->sinr = node_5_9->sinr->value;
											SgNB_ack_for_ue_measurements->sinr_exists = 1;
										}else{
											SgNB_ack_for_ue_measurements->sinr_exists = 0;
										}
										rts_fta_process_packet(&cur_packet);
									}
								}
							}
						}
					}
				}
			}
		}
	}
// --------------------------------------------------
// ---  Specialized processing for .proto sgnb_addition_request_acknowledge.json, path sgnb_addition_ack.json

	SgNB_ack_for_ue_beam_csi = (struct _SgNB_ack_for_ue_beam_csi *)(cur_packet.record.packed.values);
	cur_packet.schema = 506;
	node_6_0 = node_5_0;
	ts_lo = hdr->header->timestamp & 0xffffffff;
	ts_hi = hdr->header->timestamp >> 32;
	SgNB_ack_for_ue_beam_csi->timestamp_ms = (ts_hi - 2208988800) * 1000 + ((ts_lo * 1000) >> 32);
	if(hdr->header->gnbid==NULL)
		SgNB_ack_for_ue_beam_csi->gnb_id = empty_string;
	else
		SgNB_ack_for_ue_beam_csi->gnb_id = hdr->header->gnbid->value;

	SgNB_ack_for_ue_beam_csi->id_MeNB_UE_X2AP_ID = node_6_0->id_menb_ue_x2ap_id;
	SgNB_ack_for_ue_beam_csi->id_SgNB_UE_X2AP_ID = node_6_0->id_sgnb_ue_x2ap_id;
	if(node_6_0->id_menb_ue_x2ap_id_extension){
		SgNB_ack_for_ue_beam_csi->id_MeNB_UE_X2AP_ID_Extension = node_6_0->id_menb_ue_x2ap_id_extension->value;
	}else{
		SgNB_ack_for_ue_beam_csi->id_MeNB_UE_X2AP_ID_Extension = 0;
	}
	node_6_1 = node_6_0->id_sgnbtomenbcontainer;
	if(node_6_0->id_sgnbtomenbcontainer){
		node_6_2 = node_6_1->criticalextensionschoice1;
		if(node_6_1->criticalextensionschoice1){
			node_6_3 = node_6_2->protocolies;
			if(node_6_2->protocolies){
				node_6_4 = node_6_3->candidatecellinfolistsn;
				if(node_6_3->candidatecellinfolistsn){
					for(i_6_5=0;i_6_5<node_6_4->n_items; i_6_5++){
						node_6_5 = node_6_4->items[i_6_5];
						if(node_6_5->ssbfrequency){
							SgNB_ack_for_ue_beam_csi->ssbFrequency = node_6_5->ssbfrequency->value;
							SgNB_ack_for_ue_beam_csi->ssbFrequency_exists = 1;
						}else{
							SgNB_ack_for_ue_beam_csi->ssbFrequency_exists = 0;
						}
						if(node_6_5->reffreqcsi_rs){
							SgNB_ack_for_ue_beam_csi->refFreqCSI_RS = node_6_5->reffreqcsi_rs->value;
							SgNB_ack_for_ue_beam_csi->refFreqCSI_RS_exists = 1;
						}else{
							SgNB_ack_for_ue_beam_csi->refFreqCSI_RS_exists = 0;
						}
						node_6_6 = node_6_5->measresultservingcell;
						if(node_6_5->measresultservingcell){
							if(node_6_6->physcellid){
								SgNB_ack_for_ue_beam_csi->physCellId = node_6_6->physcellid->value;
								SgNB_ack_for_ue_beam_csi->physCellId_exists = 1;
							}else{
								SgNB_ack_for_ue_beam_csi->physCellId_exists = 0;
							}
							node_6_7 = node_6_6->measresult;
							if(node_6_6->measresult){
								node_6_8 = node_6_7->rsindexresults;
								if(node_6_7->rsindexresults){
									node_6_9 = node_6_8->resultscsi_rs_indexes;
									if(node_6_8->resultscsi_rs_indexes){
										for(i_6_10=0;i_6_10<node_6_9->n_items; i_6_10++){
											node_6_10 = node_6_9->items[i_6_10];
											SgNB_ack_for_ue_beam_csi->csi_rs_index = node_6_10->csi_rs_index;
											node_6_11 = node_6_10->csi_rs_results;
											if(node_6_10->csi_rs_results){
												if(node_6_11->rsrq){
													SgNB_ack_for_ue_beam_csi->rsrq = node_6_11->rsrq->value;
													SgNB_ack_for_ue_beam_csi->rsrq_exists = 1;
												}else{
													SgNB_ack_for_ue_beam_csi->rsrq_exists = 0;
												}
												if(node_6_11->rsrp){
													SgNB_ack_for_ue_beam_csi->rsrp = node_6_11->rsrp->value;
													SgNB_ack_for_ue_beam_csi->rsrp_exists = 1;
												}else{
													SgNB_ack_for_ue_beam_csi->rsrp_exists = 0;
												}
												if(node_6_11->sinr){
													SgNB_ack_for_ue_beam_csi->sinr = node_6_11->sinr->value;
													SgNB_ack_for_ue_beam_csi->sinr_exists = 1;
												}else{
													SgNB_ack_for_ue_beam_csi->sinr_exists = 0;
												}
												rts_fta_process_packet(&cur_packet);
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
// --------------------------------------------------
// ---  Specialized processing for .proto sgnb_addition_request_acknowledge.json, path sgnb_addition_ack.json

	SgNB_ack_for_ue_beam_ssb = (struct _SgNB_ack_for_ue_beam_ssb *)(cur_packet.record.packed.values);
	cur_packet.schema = 507;
	node_7_0 = node_6_0;
	ts_lo = hdr->header->timestamp & 0xffffffff;
	ts_hi = hdr->header->timestamp >> 32;
	SgNB_ack_for_ue_beam_ssb->timestamp_ms = (ts_hi - 2208988800) * 1000 + ((ts_lo * 1000) >> 32);
	if(hdr->header->gnbid==NULL)
		SgNB_ack_for_ue_beam_ssb->gnb_id = empty_string;
	else
		SgNB_ack_for_ue_beam_ssb->gnb_id = hdr->header->gnbid->value;

	SgNB_ack_for_ue_beam_ssb->id_MeNB_UE_X2AP_ID = node_7_0->id_menb_ue_x2ap_id;
	SgNB_ack_for_ue_beam_ssb->id_SgNB_UE_X2AP_ID = node_7_0->id_sgnb_ue_x2ap_id;
	if(node_7_0->id_menb_ue_x2ap_id_extension){
		SgNB_ack_for_ue_beam_ssb->id_MeNB_UE_X2AP_ID_Extension = node_7_0->id_menb_ue_x2ap_id_extension->value;
	}else{
		SgNB_ack_for_ue_beam_ssb->id_MeNB_UE_X2AP_ID_Extension = 0;
	}
	node_7_1 = node_7_0->id_sgnbtomenbcontainer;
	if(node_7_0->id_sgnbtomenbcontainer){
		node_7_2 = node_7_1->criticalextensionschoice1;
		if(node_7_1->criticalextensionschoice1){
			node_7_3 = node_7_2->protocolies;
			if(node_7_2->protocolies){
				node_7_4 = node_7_3->candidatecellinfolistsn;
				if(node_7_3->candidatecellinfolistsn){
					for(i_7_5=0;i_7_5<node_7_4->n_items; i_7_5++){
						node_7_5 = node_7_4->items[i_7_5];
						if(node_7_5->ssbfrequency){
							SgNB_ack_for_ue_beam_ssb->ssbFrequency = node_7_5->ssbfrequency->value;
							SgNB_ack_for_ue_beam_ssb->ssbFrequency_exists = 1;
						}else{
							SgNB_ack_for_ue_beam_ssb->ssbFrequency_exists = 0;
						}
						if(node_7_5->reffreqcsi_rs){
							SgNB_ack_for_ue_beam_ssb->refFreqCSI_RS = node_7_5->reffreqcsi_rs->value;
							SgNB_ack_for_ue_beam_ssb->refFreqCSI_RS_exists = 1;
						}else{
							SgNB_ack_for_ue_beam_ssb->refFreqCSI_RS_exists = 0;
						}
						node_7_6 = node_7_5->measresultservingcell;
						if(node_7_5->measresultservingcell){
							if(node_7_6->physcellid){
								SgNB_ack_for_ue_beam_ssb->physCellId = node_7_6->physcellid->value;
								SgNB_ack_for_ue_beam_ssb->physCellId_exists = 1;
							}else{
								SgNB_ack_for_ue_beam_ssb->physCellId_exists = 0;
							}
							node_7_7 = node_7_6->measresult;
							if(node_7_6->measresult){
								node_7_8 = node_7_7->rsindexresults;
								if(node_7_7->rsindexresults){
									node_7_9 = node_7_8->resultsssb_indexes;
									if(node_7_8->resultsssb_indexes){
										for(i_7_10=0;i_7_10<node_7_9->n_items; i_7_10++){
											node_7_10 = node_7_9->items[i_7_10];
											SgNB_ack_for_ue_beam_ssb->ssb_Index = node_7_10->ssb_index;
											node_7_11 = node_7_10->ssb_results;
											if(node_7_10->ssb_results){
												if(node_7_11->rsrq){
													SgNB_ack_for_ue_beam_ssb->rsrq = node_7_11->rsrq->value;
													SgNB_ack_for_ue_beam_ssb->rsrq_exists = 1;
												}else{
													SgNB_ack_for_ue_beam_ssb->rsrq_exists = 0;
												}
												if(node_7_11->rsrp){
													SgNB_ack_for_ue_beam_ssb->rsrp = node_7_11->rsrp->value;
													SgNB_ack_for_ue_beam_ssb->rsrp_exists = 1;
												}else{
													SgNB_ack_for_ue_beam_ssb->rsrp_exists = 0;
												}
												if(node_7_11->sinr){
													SgNB_ack_for_ue_beam_ssb->sinr = node_7_11->sinr->value;
													SgNB_ack_for_ue_beam_ssb->sinr_exists = 1;
												}else{
													SgNB_ack_for_ue_beam_ssb->sinr_exists = 0;
												}
												rts_fta_process_packet(&cur_packet);
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	streaming_protobufs__sg_nbaddition_request_acknowledge__free_unpacked(node_0_0,NULL);
	return 0;
}

gs_uint32_t process_buffer_SGNB_ADDITION_REQ(gs_uint8_t * buffer, gs_uint32_t buflen){
	char *empty_string = "";
unsigned long long int ts_lo, ts_hi;
	StreamingProtobufs__X2APStreaming *hdr = NULL;
// ------------------------------------------
// ---  Variables for .proto sgnb_addition_request.json, path sgnb_addition_req.json
	struct _sgnb_addreq_gtp_teid *sgnb_addreq_gtp_teid = NULL;
	StreamingProtobufs__SgNBAdditionRequest *node_0_0 = NULL;
	StreamingProtobufs__SgNBAdditionRequestIEs *node_0_1 = NULL;
	StreamingProtobufs__ERABsToBeAddedSgNBAddReqList *node_0_2 = NULL;
	StreamingProtobufs__ERABsToBeAddedSgNBAddReqItemIEs *node_0_3 = NULL;
	gs_uint32_t i_0_3;
	StreamingProtobufs__ERABsToBeAddedSgNBAddReqItem *node_0_4 = NULL;
	StreamingProtobufs__ERABsToBeAddedSgNBAddReqSgNBPDCPpresent *node_0_5 = NULL;
	StreamingProtobufs__GTPtunnelEndpoint *node_0_6 = NULL;
// ------------------------------------------
// ---  Variables for .proto sgnb_addition_request.json, path sgnb_addition_req.json
	struct _sgnb_addreq_for_ue *sgnb_addreq_for_ue = NULL;
	StreamingProtobufs__SgNBAdditionRequest *node_1_0 = NULL;
	StreamingProtobufs__SgNBAdditionRequestIEs *node_1_1 = NULL;
	StreamingProtobufs__ECGI *node_1_2 = NULL;
// ------------------------------------------
// ---  Variables for .proto sgnb_addition_request.json, path sgnb_addition_req.json
	struct _sgnb_addreq_for_ue_bearers *sgnb_addreq_for_ue_bearers = NULL;
	StreamingProtobufs__SgNBAdditionRequest *node_2_0 = NULL;
	StreamingProtobufs__SgNBAdditionRequestIEs *node_2_1 = NULL;
	StreamingProtobufs__ERABsToBeAddedSgNBAddReqList *node_2_2 = NULL;
	StreamingProtobufs__ERABsToBeAddedSgNBAddReqItemIEs *node_2_3 = NULL;
	gs_uint32_t i_2_3;
	StreamingProtobufs__ERABsToBeAddedSgNBAddReqItem *node_2_4 = NULL;
// ------------------------------------------
// ---  Variables for .proto sgnb_addition_request.json, path sgnb_addition_req.json
	struct _sgnb_addreq_for_ue_sn_serv_ssb *sgnb_addreq_for_ue_sn_serv_ssb = NULL;
	StreamingProtobufs__SgNBAdditionRequest *node_3_0 = NULL;
	StreamingProtobufs__SgNBAdditionRequestIEs *node_3_1 = NULL;
	StreamingProtobufs__CGConfigInfo *node_3_2 = NULL;
	StreamingProtobufs__CGConfigInfoCriticalExtensionsChoice1 *node_3_3 = NULL;
	StreamingProtobufs__CGConfigInfoIEs *node_3_4 = NULL;
	StreamingProtobufs__MeasResultList2NR *node_3_5 = NULL;
	StreamingProtobufs__MeasResult2NR *node_3_6 = NULL;
	gs_uint32_t i_3_6;
	StreamingProtobufs__MeasResultNR *node_3_7 = NULL;
	StreamingProtobufs__MeasResult *node_3_8 = NULL;
	StreamingProtobufs__CellResults *node_3_9 = NULL;
	StreamingProtobufs__MeasQuantityResults *node_3_10 = NULL;
// ------------------------------------------
// ---  Variables for .proto sgnb_addition_request.json, path sgnb_addition_req.json
	struct _sgnb_addreq_for_ue_sn_serv_csi_rs *sgnb_addreq_for_ue_sn_serv_csi_rs = NULL;
	StreamingProtobufs__SgNBAdditionRequest *node_4_0 = NULL;
	StreamingProtobufs__SgNBAdditionRequestIEs *node_4_1 = NULL;
	StreamingProtobufs__CGConfigInfo *node_4_2 = NULL;
	StreamingProtobufs__CGConfigInfoCriticalExtensionsChoice1 *node_4_3 = NULL;
	StreamingProtobufs__CGConfigInfoIEs *node_4_4 = NULL;
	StreamingProtobufs__MeasResultList2NR *node_4_5 = NULL;
	StreamingProtobufs__MeasResult2NR *node_4_6 = NULL;
	gs_uint32_t i_4_6;
	StreamingProtobufs__MeasResultNR *node_4_7 = NULL;
	StreamingProtobufs__MeasResult *node_4_8 = NULL;
	StreamingProtobufs__CellResults *node_4_9 = NULL;
	StreamingProtobufs__MeasQuantityResults *node_4_10 = NULL;
// ------------------------------------------
// ---  Variables for .proto sgnb_addition_request.json, path sgnb_addition_req.json
	struct _sgnb_addreq_for_ue_mn_serv_ssb *sgnb_addreq_for_ue_mn_serv_ssb = NULL;
	StreamingProtobufs__SgNBAdditionRequest *node_5_0 = NULL;
	StreamingProtobufs__SgNBAdditionRequestIEs *node_5_1 = NULL;
	StreamingProtobufs__CGConfigInfo *node_5_2 = NULL;
	StreamingProtobufs__CGConfigInfoCriticalExtensionsChoice1 *node_5_3 = NULL;
	StreamingProtobufs__CGConfigInfoIEs *node_5_4 = NULL;
	StreamingProtobufs__MeasResultList2NR *node_5_5 = NULL;
	StreamingProtobufs__MeasResult2NR *node_5_6 = NULL;
	gs_uint32_t i_5_6;
	StreamingProtobufs__MeasResultNR *node_5_7 = NULL;
	StreamingProtobufs__MeasResult *node_5_8 = NULL;
	StreamingProtobufs__CellResults *node_5_9 = NULL;
	StreamingProtobufs__MeasQuantityResults *node_5_10 = NULL;
// ------------------------------------------
// ---  Variables for .proto sgnb_addition_request.json, path sgnb_addition_req.json
	struct _sgnb_addreq_for_ue_mn_serv_csi_rs *sgnb_addreq_for_ue_mn_serv_csi_rs = NULL;
	StreamingProtobufs__SgNBAdditionRequest *node_6_0 = NULL;
	StreamingProtobufs__SgNBAdditionRequestIEs *node_6_1 = NULL;
	StreamingProtobufs__CGConfigInfo *node_6_2 = NULL;
	StreamingProtobufs__CGConfigInfoCriticalExtensionsChoice1 *node_6_3 = NULL;
	StreamingProtobufs__CGConfigInfoIEs *node_6_4 = NULL;
	StreamingProtobufs__MeasResultList2NR *node_6_5 = NULL;
	StreamingProtobufs__MeasResult2NR *node_6_6 = NULL;
	gs_uint32_t i_6_6;
	StreamingProtobufs__MeasResultNR *node_6_7 = NULL;
	StreamingProtobufs__MeasResult *node_6_8 = NULL;
	StreamingProtobufs__CellResults *node_6_9 = NULL;
	StreamingProtobufs__MeasQuantityResults *node_6_10 = NULL;
// ------------------------------------------
// ---  Variables for .proto sgnb_addition_request.json, path sgnb_addition_req.json
	struct _sgnb_addreq_for_ue_sn_neigh_ssb *sgnb_addreq_for_ue_sn_neigh_ssb = NULL;
	StreamingProtobufs__SgNBAdditionRequest *node_7_0 = NULL;
	StreamingProtobufs__SgNBAdditionRequestIEs *node_7_1 = NULL;
	StreamingProtobufs__CGConfigInfo *node_7_2 = NULL;
	StreamingProtobufs__CGConfigInfoCriticalExtensionsChoice1 *node_7_3 = NULL;
	StreamingProtobufs__CGConfigInfoIEs *node_7_4 = NULL;
	StreamingProtobufs__MeasResultList2NR *node_7_5 = NULL;
	StreamingProtobufs__MeasResult2NR *node_7_6 = NULL;
	gs_uint32_t i_7_6;
	StreamingProtobufs__MeasResultListNR *node_7_7 = NULL;
	StreamingProtobufs__MeasResultNR *node_7_8 = NULL;
	gs_uint32_t i_7_8;
	StreamingProtobufs__MeasResult *node_7_9 = NULL;
	StreamingProtobufs__CellResults *node_7_10 = NULL;
	StreamingProtobufs__MeasQuantityResults *node_7_11 = NULL;
// ------------------------------------------
// ---  Variables for .proto sgnb_addition_request.json, path sgnb_addition_req.json
	struct _sgnb_addreq_for_ue_sn_neigh_csi_rs *sgnb_addreq_for_ue_sn_neigh_csi_rs = NULL;
	StreamingProtobufs__SgNBAdditionRequest *node_8_0 = NULL;
	StreamingProtobufs__SgNBAdditionRequestIEs *node_8_1 = NULL;
	StreamingProtobufs__CGConfigInfo *node_8_2 = NULL;
	StreamingProtobufs__CGConfigInfoCriticalExtensionsChoice1 *node_8_3 = NULL;
	StreamingProtobufs__CGConfigInfoIEs *node_8_4 = NULL;
	StreamingProtobufs__MeasResultList2NR *node_8_5 = NULL;
	StreamingProtobufs__MeasResult2NR *node_8_6 = NULL;
	gs_uint32_t i_8_6;
	StreamingProtobufs__MeasResultListNR *node_8_7 = NULL;
	StreamingProtobufs__MeasResultNR *node_8_8 = NULL;
	gs_uint32_t i_8_8;
	StreamingProtobufs__MeasResult *node_8_9 = NULL;
	StreamingProtobufs__CellResults *node_8_10 = NULL;
	StreamingProtobufs__MeasQuantityResults *node_8_11 = NULL;
// ------------------------------------------
// ---  Variables for .proto sgnb_addition_request.json, path sgnb_addition_req.json
	struct _sgnb_addreq_for_ue_mn_neigh_ssb *sgnb_addreq_for_ue_mn_neigh_ssb = NULL;
	StreamingProtobufs__SgNBAdditionRequest *node_9_0 = NULL;
	StreamingProtobufs__SgNBAdditionRequestIEs *node_9_1 = NULL;
	StreamingProtobufs__CGConfigInfo *node_9_2 = NULL;
	StreamingProtobufs__CGConfigInfoCriticalExtensionsChoice1 *node_9_3 = NULL;
	StreamingProtobufs__CGConfigInfoIEs *node_9_4 = NULL;
	StreamingProtobufs__MeasResultList2NR *node_9_5 = NULL;
	StreamingProtobufs__MeasResult2NR *node_9_6 = NULL;
	gs_uint32_t i_9_6;
	StreamingProtobufs__MeasResultListNR *node_9_7 = NULL;
	StreamingProtobufs__MeasResultNR *node_9_8 = NULL;
	gs_uint32_t i_9_8;
	StreamingProtobufs__MeasResult *node_9_9 = NULL;
	StreamingProtobufs__CellResults *node_9_10 = NULL;
	StreamingProtobufs__MeasQuantityResults *node_9_11 = NULL;
// ------------------------------------------
// ---  Variables for .proto sgnb_addition_request.json, path sgnb_addition_req.json
	struct _sgnb_addreq_for_ue_mn_neigh_csi_rs *sgnb_addreq_for_ue_mn_neigh_csi_rs = NULL;
	StreamingProtobufs__SgNBAdditionRequest *node_10_0 = NULL;
	StreamingProtobufs__SgNBAdditionRequestIEs *node_10_1 = NULL;
	StreamingProtobufs__CGConfigInfo *node_10_2 = NULL;
	StreamingProtobufs__CGConfigInfoCriticalExtensionsChoice1 *node_10_3 = NULL;
	StreamingProtobufs__CGConfigInfoIEs *node_10_4 = NULL;
	StreamingProtobufs__MeasResultList2NR *node_10_5 = NULL;
	StreamingProtobufs__MeasResult2NR *node_10_6 = NULL;
	gs_uint32_t i_10_6;
	StreamingProtobufs__MeasResultListNR *node_10_7 = NULL;
	StreamingProtobufs__MeasResultNR *node_10_8 = NULL;
	gs_uint32_t i_10_8;
	StreamingProtobufs__MeasResult *node_10_9 = NULL;
	StreamingProtobufs__CellResults *node_10_10 = NULL;
	StreamingProtobufs__MeasQuantityResults *node_10_11 = NULL;

// --------------------------------------------------
// ---  Specialized processing for .proto sgnb_addition_request.json, path sgnb_addition_req.json

	sgnb_addreq_gtp_teid = (struct _sgnb_addreq_gtp_teid *)(cur_packet.record.packed.values);
	cur_packet.schema = 10001;

	hdr = streaming_protobufs__x2_apstreaming__unpack(NULL, buflen, buffer);
	if(hdr==NULL) return -1;

	node_0_0 = hdr->sgnbadditionrequest;
	if(node_0_0==NULL) return -2;
	if(hdr->header==NULL) return -3;

	ts_lo = hdr->header->timestamp & 0xffffffff;
	ts_hi = hdr->header->timestamp >> 32;
	sgnb_addreq_gtp_teid->timestamp_ms = (ts_hi - 2208988800) * 1000 + ((ts_lo * 1000) >> 32);
	if(hdr->header->gnbid==NULL)
		sgnb_addreq_gtp_teid->gnb_id = empty_string;
	else
		sgnb_addreq_gtp_teid->gnb_id = hdr->header->gnbid->value;

	node_0_1 = node_0_0->protocolies;
	if(node_0_0->protocolies){
		sgnb_addreq_gtp_teid->id_MeNB_UE_X2AP_ID = node_0_1->id_menb_ue_x2ap_id;
		node_0_2 = node_0_1->id_e_rabs_tobeadded_sgnbaddreqlist;
		if(node_0_1->id_e_rabs_tobeadded_sgnbaddreqlist){
			for(i_0_3=0;i_0_3<node_0_2->n_items; i_0_3++){
				node_0_3 = node_0_2->items[i_0_3];
				node_0_4 = node_0_3->id_e_rabs_tobeadded_sgnbaddreq_item;
				if(node_0_3->id_e_rabs_tobeadded_sgnbaddreq_item){
					node_0_5 = node_0_4->sgnbpdcppresent;
					if(node_0_4->sgnbpdcppresent){
						node_0_6 = node_0_5->s1_ul_gtptunnelendpoint;
						if(node_0_5->s1_ul_gtptunnelendpoint){
							sgnb_addreq_gtp_teid->gTP_TEID = node_0_6->gtp_teid;
							sgnb_addreq_gtp_teid->transportLayerAddress = node_0_6->transportlayeraddress;
							rts_fta_process_packet(&cur_packet);
						}
					}
				}
			}
		}
	}
// --------------------------------------------------
// ---  Specialized processing for .proto sgnb_addition_request.json, path sgnb_addition_req.json

	sgnb_addreq_for_ue = (struct _sgnb_addreq_for_ue *)(cur_packet.record.packed.values);
	cur_packet.schema = 401;
	node_1_0 = node_0_0;
	ts_lo = hdr->header->timestamp & 0xffffffff;
	ts_hi = hdr->header->timestamp >> 32;
	sgnb_addreq_for_ue->timestamp_ms = (ts_hi - 2208988800) * 1000 + ((ts_lo * 1000) >> 32);
	if(hdr->header->gnbid==NULL)
		sgnb_addreq_for_ue->gnb_id = empty_string;
	else
		sgnb_addreq_for_ue->gnb_id = hdr->header->gnbid->value;

	node_1_1 = node_1_0->protocolies;
	if(node_1_0->protocolies){
		sgnb_addreq_for_ue->id_MeNB_UE_X2AP_ID = node_1_1->id_menb_ue_x2ap_id;
		if(node_1_1->id_sgnbueaggregatemaximumbitrate){
			sgnb_addreq_for_ue->uEaggregateMaximumBitRateDownlink = node_1_1->id_sgnbueaggregatemaximumbitrate->ueaggregatemaximumbitratedownlink;
			sgnb_addreq_for_ue->uEaggregateMaximumBitRateDownlink_exists = 1;
		}else{
			sgnb_addreq_for_ue->uEaggregateMaximumBitRateDownlink_exists = 0;
		}
		if(node_1_1->id_menb_ue_x2ap_id_extension){
			sgnb_addreq_for_ue->id_MeNB_UE_X2AP_ID_Extension = node_1_1->id_menb_ue_x2ap_id_extension->value;
		}else{
			sgnb_addreq_for_ue->id_MeNB_UE_X2AP_ID_Extension = 0;
		}
		node_1_2 = node_1_1->id_menbcell_id;
		if(node_1_1->id_menbcell_id){
			sgnb_addreq_for_ue->eUTRANcellIdentifier = node_1_2->eutrancellidentifier;
			sgnb_addreq_for_ue->pLMN_Identity = node_1_2->plmn_identity;
			rts_fta_process_packet(&cur_packet);
		}
	}
// --------------------------------------------------
// ---  Specialized processing for .proto sgnb_addition_request.json, path sgnb_addition_req.json

	sgnb_addreq_for_ue_bearers = (struct _sgnb_addreq_for_ue_bearers *)(cur_packet.record.packed.values);
	cur_packet.schema = 402;
	node_2_0 = node_1_0;
	ts_lo = hdr->header->timestamp & 0xffffffff;
	ts_hi = hdr->header->timestamp >> 32;
	sgnb_addreq_for_ue_bearers->timestamp_ms = (ts_hi - 2208988800) * 1000 + ((ts_lo * 1000) >> 32);
	if(hdr->header->gnbid==NULL)
		sgnb_addreq_for_ue_bearers->gnb_id = empty_string;
	else
		sgnb_addreq_for_ue_bearers->gnb_id = hdr->header->gnbid->value;

	node_2_1 = node_2_0->protocolies;
	if(node_2_0->protocolies){
		sgnb_addreq_for_ue_bearers->id_MeNB_UE_X2AP_ID = node_2_1->id_menb_ue_x2ap_id;
		node_2_2 = node_2_1->id_e_rabs_tobeadded_sgnbaddreqlist;
		if(node_2_1->id_e_rabs_tobeadded_sgnbaddreqlist){
			for(i_2_3=0;i_2_3<node_2_2->n_items; i_2_3++){
				node_2_3 = node_2_2->items[i_2_3];
				node_2_4 = node_2_3->id_e_rabs_tobeadded_sgnbaddreq_item;
				if(node_2_3->id_e_rabs_tobeadded_sgnbaddreq_item){
					if(node_2_4->sgnbpdcppresent && node_2_4->sgnbpdcppresent->max_mcg_admit_e_rab_level_qos_parameters){
						sgnb_addreq_for_ue_bearers->MCG_eRAB_MaximumBitrateDL = node_2_4->sgnbpdcppresent->max_mcg_admit_e_rab_level_qos_parameters->e_rab_maximumbitratedl;
					}else{
						sgnb_addreq_for_ue_bearers->MCG_eRAB_MaximumBitrateDL = 0;
					}
					if(node_2_4->en_dc_resourceconfiguration){
						sgnb_addreq_for_ue_bearers->pDCPatSgNB = node_2_4->en_dc_resourceconfiguration->pdcpatsgnb;
					}else{
						sgnb_addreq_for_ue_bearers->pDCPatSgNB = -1;
					}
					sgnb_addreq_for_ue_bearers->drb_ID = node_2_4->drb_id;
					if(node_2_4->sgnbpdcppresent && node_2_4->sgnbpdcppresent->full_e_rab_level_qos_parameters && node_2_4->sgnbpdcppresent->full_e_rab_level_qos_parameters->allocationandretentionpriority){
						sgnb_addreq_for_ue_bearers->priorityLevel = node_2_4->sgnbpdcppresent->full_e_rab_level_qos_parameters->allocationandretentionpriority->prioritylevel;
					}else{
						sgnb_addreq_for_ue_bearers->priorityLevel = 0;
					}
					if(node_2_4->sgnbpdcppresent && node_2_4->sgnbpdcppresent->menb_dl_gtp_teidatmcg){
						sgnb_addreq_for_ue_bearers->gTP_TEID = node_2_4->sgnbpdcppresent->menb_dl_gtp_teidatmcg->gtp_teid;
					}else{
						sgnb_addreq_for_ue_bearers->gTP_TEID.data = "";
						sgnb_addreq_for_ue_bearers->gTP_TEID.len = 1;
					}
					if(node_2_4->sgnbpdcppresent && node_2_4->sgnbpdcppresent->full_e_rab_level_qos_parameters && node_2_4->sgnbpdcppresent->full_e_rab_level_qos_parameters->allocationandretentionpriority && node_2_4->sgnbpdcppresent->full_e_rab_level_qos_parameters->allocationandretentionpriority->pre_emptioncapability){
						sgnb_addreq_for_ue_bearers->pre_emptionCapability = node_2_4->sgnbpdcppresent->full_e_rab_level_qos_parameters->allocationandretentionpriority->pre_emptioncapability->value;
					}else{
						sgnb_addreq_for_ue_bearers->pre_emptionCapability = -1;
					}
					if(node_2_4->sgnbpdcppresent && node_2_4->sgnbpdcppresent->max_mcg_admit_e_rab_level_qos_parameters){
						sgnb_addreq_for_ue_bearers->MCG_eRAB_GuaranteedBitrateUL = node_2_4->sgnbpdcppresent->max_mcg_admit_e_rab_level_qos_parameters->e_rab_guaranteedbitrateul;
					}else{
						sgnb_addreq_for_ue_bearers->MCG_eRAB_GuaranteedBitrateUL = 0;
					}
					if(node_2_4->en_dc_resourceconfiguration){
						sgnb_addreq_for_ue_bearers->mCGresources = node_2_4->en_dc_resourceconfiguration->mcgresources;
					}else{
						sgnb_addreq_for_ue_bearers->mCGresources = -1;
					}
					if(node_2_4->sgnbpdcppresent && node_2_4->sgnbpdcppresent->menb_dl_gtp_teidatmcg){
						sgnb_addreq_for_ue_bearers->transportLayerAddress = node_2_4->sgnbpdcppresent->menb_dl_gtp_teidatmcg->transportlayeraddress;
					}else{
						sgnb_addreq_for_ue_bearers->transportLayerAddress.data = "";
						sgnb_addreq_for_ue_bearers->transportLayerAddress.len = 1;
					}
					if(node_2_4->sgnbpdcppresent && node_2_4->sgnbpdcppresent->full_e_rab_level_qos_parameters && node_2_4->sgnbpdcppresent->full_e_rab_level_qos_parameters->gbrqosinformation){
						sgnb_addreq_for_ue_bearers->full_eRAB_GuaranteedBitrateUL = node_2_4->sgnbpdcppresent->full_e_rab_level_qos_parameters->gbrqosinformation->e_rab_guaranteedbitrateul;
					}else{
						sgnb_addreq_for_ue_bearers->full_eRAB_GuaranteedBitrateUL = 0;
					}
					if(node_2_4->en_dc_resourceconfiguration){
						sgnb_addreq_for_ue_bearers->sCGresources = node_2_4->en_dc_resourceconfiguration->scgresources;
					}else{
						sgnb_addreq_for_ue_bearers->sCGresources = -1;
					}
					if(node_2_4->sgnbpdcppresent && node_2_4->sgnbpdcppresent->max_mcg_admit_e_rab_level_qos_parameters){
						sgnb_addreq_for_ue_bearers->MCG_eRAB_MaximumBitrateUL = node_2_4->sgnbpdcppresent->max_mcg_admit_e_rab_level_qos_parameters->e_rab_maximumbitrateul;
					}else{
						sgnb_addreq_for_ue_bearers->MCG_eRAB_MaximumBitrateUL = 0;
					}
					if(node_2_4->sgnbpdcppresent && node_2_4->sgnbpdcppresent->full_e_rab_level_qos_parameters && node_2_4->sgnbpdcppresent->full_e_rab_level_qos_parameters->gbrqosinformation){
						sgnb_addreq_for_ue_bearers->full_eRAB_MaximumBitrateUL = node_2_4->sgnbpdcppresent->full_e_rab_level_qos_parameters->gbrqosinformation->e_rab_maximumbitrateul;
					}else{
						sgnb_addreq_for_ue_bearers->full_eRAB_MaximumBitrateUL = 0;
					}
					if(node_2_4->sgnbpdcppresent && node_2_4->sgnbpdcppresent->full_e_rab_level_qos_parameters && node_2_4->sgnbpdcppresent->full_e_rab_level_qos_parameters->allocationandretentionpriority && node_2_4->sgnbpdcppresent->full_e_rab_level_qos_parameters->allocationandretentionpriority->pre_emptionvulnerability){
						sgnb_addreq_for_ue_bearers->pre_emptionVulnerability = node_2_4->sgnbpdcppresent->full_e_rab_level_qos_parameters->allocationandretentionpriority->pre_emptionvulnerability->value;
					}else{
						sgnb_addreq_for_ue_bearers->pre_emptionVulnerability = -1;
					}
					sgnb_addreq_for_ue_bearers->e_RAB_ID = node_2_4->e_rab_id;
					if(node_2_4->sgnbpdcppresent && node_2_4->sgnbpdcppresent->max_mcg_admit_e_rab_level_qos_parameters){
						sgnb_addreq_for_ue_bearers->MCG_eRAB_GuaranteedBitrateDL = node_2_4->sgnbpdcppresent->max_mcg_admit_e_rab_level_qos_parameters->e_rab_guaranteedbitratedl;
					}else{
						sgnb_addreq_for_ue_bearers->MCG_eRAB_GuaranteedBitrateDL = 0;
					}
					if(node_2_4->sgnbpdcppresent && node_2_4->sgnbpdcppresent->full_e_rab_level_qos_parameters){
						sgnb_addreq_for_ue_bearers->qCI = node_2_4->sgnbpdcppresent->full_e_rab_level_qos_parameters->qci;
					}else{
						sgnb_addreq_for_ue_bearers->qCI = 0;
					}
					if(node_2_4->sgnbpdcppresent && node_2_4->sgnbpdcppresent->full_e_rab_level_qos_parameters && node_2_4->sgnbpdcppresent->full_e_rab_level_qos_parameters->gbrqosinformation){
						sgnb_addreq_for_ue_bearers->full_eRAB_MaximumBitrateDL = node_2_4->sgnbpdcppresent->full_e_rab_level_qos_parameters->gbrqosinformation->e_rab_maximumbitratedl;
					}else{
						sgnb_addreq_for_ue_bearers->full_eRAB_MaximumBitrateDL = 0;
					}
					if(node_2_4->sgnbpdcppresent && node_2_4->sgnbpdcppresent->full_e_rab_level_qos_parameters && node_2_4->sgnbpdcppresent->full_e_rab_level_qos_parameters->gbrqosinformation){
						sgnb_addreq_for_ue_bearers->full_eRAB_GuaranteedBitrateDL = node_2_4->sgnbpdcppresent->full_e_rab_level_qos_parameters->gbrqosinformation->e_rab_guaranteedbitratedl;
					}else{
						sgnb_addreq_for_ue_bearers->full_eRAB_GuaranteedBitrateDL = 0;
					}
					rts_fta_process_packet(&cur_packet);
				}
			}
		}
	}
// --------------------------------------------------
// ---  Specialized processing for .proto sgnb_addition_request.json, path sgnb_addition_req.json

	sgnb_addreq_for_ue_sn_serv_ssb = (struct _sgnb_addreq_for_ue_sn_serv_ssb *)(cur_packet.record.packed.values);
	cur_packet.schema = 403;
	node_3_0 = node_2_0;
	ts_lo = hdr->header->timestamp & 0xffffffff;
	ts_hi = hdr->header->timestamp >> 32;
	sgnb_addreq_for_ue_sn_serv_ssb->timestamp_ms = (ts_hi - 2208988800) * 1000 + ((ts_lo * 1000) >> 32);
	if(hdr->header->gnbid==NULL)
		sgnb_addreq_for_ue_sn_serv_ssb->gnb_id = empty_string;
	else
		sgnb_addreq_for_ue_sn_serv_ssb->gnb_id = hdr->header->gnbid->value;

	node_3_1 = node_3_0->protocolies;
	if(node_3_0->protocolies){
		sgnb_addreq_for_ue_sn_serv_ssb->id_MeNB_UE_X2AP_ID = node_3_1->id_menb_ue_x2ap_id;
		if(node_3_1->id_menb_ue_x2ap_id_extension){
			sgnb_addreq_for_ue_sn_serv_ssb->id_MeNB_UE_X2AP_ID_Extension = node_3_1->id_menb_ue_x2ap_id_extension->value;
		}else{
			sgnb_addreq_for_ue_sn_serv_ssb->id_MeNB_UE_X2AP_ID_Extension = 0;
		}
		node_3_2 = node_3_1->id_menbtosgnbcontainer;
		if(node_3_1->id_menbtosgnbcontainer){
			node_3_3 = node_3_2->criticalextensionschoice1;
			if(node_3_2->criticalextensionschoice1){
				node_3_4 = node_3_3->protocolies;
				if(node_3_3->protocolies){
					node_3_5 = node_3_4->candidatecellinfolistsn;
					if(node_3_4->candidatecellinfolistsn){
						for(i_3_6=0;i_3_6<node_3_5->n_items; i_3_6++){
							node_3_6 = node_3_5->items[i_3_6];
							node_3_7 = node_3_6->measresultservingcell;
							if(node_3_6->measresultservingcell){
								if(node_3_7->physcellid){
									sgnb_addreq_for_ue_sn_serv_ssb->physCellId = node_3_7->physcellid->value;
									sgnb_addreq_for_ue_sn_serv_ssb->physCellId_exists = 1;
								}else{
									sgnb_addreq_for_ue_sn_serv_ssb->physCellId_exists = 0;
								}
								node_3_8 = node_3_7->measresult;
								if(node_3_7->measresult){
									node_3_9 = node_3_8->cellresults;
									if(node_3_8->cellresults){
										node_3_10 = node_3_9->resultsssb_cell;
										if(node_3_9->resultsssb_cell){
											if(node_3_10->rsrq){
												sgnb_addreq_for_ue_sn_serv_ssb->rsrq = node_3_10->rsrq->value;
											}else{
												sgnb_addreq_for_ue_sn_serv_ssb->rsrq = 128;
											}
											if(node_3_10->rsrp){
												sgnb_addreq_for_ue_sn_serv_ssb->rsrp = node_3_10->rsrp->value;
											}else{
												sgnb_addreq_for_ue_sn_serv_ssb->rsrp = 128;
											}
											if(node_3_10->sinr){
												sgnb_addreq_for_ue_sn_serv_ssb->sinr = node_3_10->sinr->value;
											}else{
												sgnb_addreq_for_ue_sn_serv_ssb->sinr = 128;
											}
											rts_fta_process_packet(&cur_packet);
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
// --------------------------------------------------
// ---  Specialized processing for .proto sgnb_addition_request.json, path sgnb_addition_req.json

	sgnb_addreq_for_ue_sn_serv_csi_rs = (struct _sgnb_addreq_for_ue_sn_serv_csi_rs *)(cur_packet.record.packed.values);
	cur_packet.schema = 404;
	node_4_0 = node_3_0;
	ts_lo = hdr->header->timestamp & 0xffffffff;
	ts_hi = hdr->header->timestamp >> 32;
	sgnb_addreq_for_ue_sn_serv_csi_rs->timestamp_ms = (ts_hi - 2208988800) * 1000 + ((ts_lo * 1000) >> 32);
	if(hdr->header->gnbid==NULL)
		sgnb_addreq_for_ue_sn_serv_csi_rs->gnb_id = empty_string;
	else
		sgnb_addreq_for_ue_sn_serv_csi_rs->gnb_id = hdr->header->gnbid->value;

	node_4_1 = node_4_0->protocolies;
	if(node_4_0->protocolies){
		sgnb_addreq_for_ue_sn_serv_csi_rs->id_MeNB_UE_X2AP_ID = node_4_1->id_menb_ue_x2ap_id;
		if(node_4_1->id_menb_ue_x2ap_id_extension){
			sgnb_addreq_for_ue_sn_serv_csi_rs->id_MeNB_UE_X2AP_ID_Extension = node_4_1->id_menb_ue_x2ap_id_extension->value;
		}else{
			sgnb_addreq_for_ue_sn_serv_csi_rs->id_MeNB_UE_X2AP_ID_Extension = 0;
		}
		node_4_2 = node_4_1->id_menbtosgnbcontainer;
		if(node_4_1->id_menbtosgnbcontainer){
			node_4_3 = node_4_2->criticalextensionschoice1;
			if(node_4_2->criticalextensionschoice1){
				node_4_4 = node_4_3->protocolies;
				if(node_4_3->protocolies){
					node_4_5 = node_4_4->candidatecellinfolistsn;
					if(node_4_4->candidatecellinfolistsn){
						for(i_4_6=0;i_4_6<node_4_5->n_items; i_4_6++){
							node_4_6 = node_4_5->items[i_4_6];
							node_4_7 = node_4_6->measresultservingcell;
							if(node_4_6->measresultservingcell){
								if(node_4_7->physcellid){
									sgnb_addreq_for_ue_sn_serv_csi_rs->physCellId = node_4_7->physcellid->value;
									sgnb_addreq_for_ue_sn_serv_csi_rs->physCellId_exists = 1;
								}else{
									sgnb_addreq_for_ue_sn_serv_csi_rs->physCellId_exists = 0;
								}
								node_4_8 = node_4_7->measresult;
								if(node_4_7->measresult){
									node_4_9 = node_4_8->cellresults;
									if(node_4_8->cellresults){
										node_4_10 = node_4_9->resultscsi_rs_cell;
										if(node_4_9->resultscsi_rs_cell){
											if(node_4_10->rsrq){
												sgnb_addreq_for_ue_sn_serv_csi_rs->rsrq = node_4_10->rsrq->value;
											}else{
												sgnb_addreq_for_ue_sn_serv_csi_rs->rsrq = 128;
											}
											if(node_4_10->rsrp){
												sgnb_addreq_for_ue_sn_serv_csi_rs->rsrp = node_4_10->rsrp->value;
											}else{
												sgnb_addreq_for_ue_sn_serv_csi_rs->rsrp = 128;
											}
											if(node_4_10->sinr){
												sgnb_addreq_for_ue_sn_serv_csi_rs->sinr = node_4_10->sinr->value;
											}else{
												sgnb_addreq_for_ue_sn_serv_csi_rs->sinr = 128;
											}
											rts_fta_process_packet(&cur_packet);
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
// --------------------------------------------------
// ---  Specialized processing for .proto sgnb_addition_request.json, path sgnb_addition_req.json

	sgnb_addreq_for_ue_mn_serv_ssb = (struct _sgnb_addreq_for_ue_mn_serv_ssb *)(cur_packet.record.packed.values);
	cur_packet.schema = 405;
	node_5_0 = node_4_0;
	ts_lo = hdr->header->timestamp & 0xffffffff;
	ts_hi = hdr->header->timestamp >> 32;
	sgnb_addreq_for_ue_mn_serv_ssb->timestamp_ms = (ts_hi - 2208988800) * 1000 + ((ts_lo * 1000) >> 32);
	if(hdr->header->gnbid==NULL)
		sgnb_addreq_for_ue_mn_serv_ssb->gnb_id = empty_string;
	else
		sgnb_addreq_for_ue_mn_serv_ssb->gnb_id = hdr->header->gnbid->value;

	node_5_1 = node_5_0->protocolies;
	if(node_5_0->protocolies){
		sgnb_addreq_for_ue_mn_serv_ssb->id_MeNB_UE_X2AP_ID = node_5_1->id_menb_ue_x2ap_id;
		if(node_5_1->id_menb_ue_x2ap_id_extension){
			sgnb_addreq_for_ue_mn_serv_ssb->id_MeNB_UE_X2AP_ID_Extension = node_5_1->id_menb_ue_x2ap_id_extension->value;
		}else{
			sgnb_addreq_for_ue_mn_serv_ssb->id_MeNB_UE_X2AP_ID_Extension = 0;
		}
		node_5_2 = node_5_1->id_menbtosgnbcontainer;
		if(node_5_1->id_menbtosgnbcontainer){
			node_5_3 = node_5_2->criticalextensionschoice1;
			if(node_5_2->criticalextensionschoice1){
				node_5_4 = node_5_3->protocolies;
				if(node_5_3->protocolies){
					node_5_5 = node_5_4->candidatecellinfolistmn;
					if(node_5_4->candidatecellinfolistmn){
						for(i_5_6=0;i_5_6<node_5_5->n_items; i_5_6++){
							node_5_6 = node_5_5->items[i_5_6];
							node_5_7 = node_5_6->measresultservingcell;
							if(node_5_6->measresultservingcell){
								if(node_5_7->physcellid){
									sgnb_addreq_for_ue_mn_serv_ssb->physCellId = node_5_7->physcellid->value;
									sgnb_addreq_for_ue_mn_serv_ssb->physCellId_exists = 1;
								}else{
									sgnb_addreq_for_ue_mn_serv_ssb->physCellId_exists = 0;
								}
								node_5_8 = node_5_7->measresult;
								if(node_5_7->measresult){
									node_5_9 = node_5_8->cellresults;
									if(node_5_8->cellresults){
										node_5_10 = node_5_9->resultsssb_cell;
										if(node_5_9->resultsssb_cell){
											if(node_5_10->rsrq){
												sgnb_addreq_for_ue_mn_serv_ssb->rsrq = node_5_10->rsrq->value;
											}else{
												sgnb_addreq_for_ue_mn_serv_ssb->rsrq = 128;
											}
											if(node_5_10->rsrp){
												sgnb_addreq_for_ue_mn_serv_ssb->rsrp = node_5_10->rsrp->value;
											}else{
												sgnb_addreq_for_ue_mn_serv_ssb->rsrp = 128;
											}
											if(node_5_10->sinr){
												sgnb_addreq_for_ue_mn_serv_ssb->sinr = node_5_10->sinr->value;
											}else{
												sgnb_addreq_for_ue_mn_serv_ssb->sinr = 128;
											}
											rts_fta_process_packet(&cur_packet);
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
// --------------------------------------------------
// ---  Specialized processing for .proto sgnb_addition_request.json, path sgnb_addition_req.json

	sgnb_addreq_for_ue_mn_serv_csi_rs = (struct _sgnb_addreq_for_ue_mn_serv_csi_rs *)(cur_packet.record.packed.values);
	cur_packet.schema = 406;
	node_6_0 = node_5_0;
	ts_lo = hdr->header->timestamp & 0xffffffff;
	ts_hi = hdr->header->timestamp >> 32;
	sgnb_addreq_for_ue_mn_serv_csi_rs->timestamp_ms = (ts_hi - 2208988800) * 1000 + ((ts_lo * 1000) >> 32);
	if(hdr->header->gnbid==NULL)
		sgnb_addreq_for_ue_mn_serv_csi_rs->gnb_id = empty_string;
	else
		sgnb_addreq_for_ue_mn_serv_csi_rs->gnb_id = hdr->header->gnbid->value;

	node_6_1 = node_6_0->protocolies;
	if(node_6_0->protocolies){
		sgnb_addreq_for_ue_mn_serv_csi_rs->id_MeNB_UE_X2AP_ID = node_6_1->id_menb_ue_x2ap_id;
		if(node_6_1->id_menb_ue_x2ap_id_extension){
			sgnb_addreq_for_ue_mn_serv_csi_rs->id_MeNB_UE_X2AP_ID_Extension = node_6_1->id_menb_ue_x2ap_id_extension->value;
		}else{
			sgnb_addreq_for_ue_mn_serv_csi_rs->id_MeNB_UE_X2AP_ID_Extension = 0;
		}
		node_6_2 = node_6_1->id_menbtosgnbcontainer;
		if(node_6_1->id_menbtosgnbcontainer){
			node_6_3 = node_6_2->criticalextensionschoice1;
			if(node_6_2->criticalextensionschoice1){
				node_6_4 = node_6_3->protocolies;
				if(node_6_3->protocolies){
					node_6_5 = node_6_4->candidatecellinfolistmn;
					if(node_6_4->candidatecellinfolistmn){
						for(i_6_6=0;i_6_6<node_6_5->n_items; i_6_6++){
							node_6_6 = node_6_5->items[i_6_6];
							node_6_7 = node_6_6->measresultservingcell;
							if(node_6_6->measresultservingcell){
								if(node_6_7->physcellid){
									sgnb_addreq_for_ue_mn_serv_csi_rs->physCellId = node_6_7->physcellid->value;
									sgnb_addreq_for_ue_mn_serv_csi_rs->physCellId_exists = 1;
								}else{
									sgnb_addreq_for_ue_mn_serv_csi_rs->physCellId_exists = 0;
								}
								node_6_8 = node_6_7->measresult;
								if(node_6_7->measresult){
									node_6_9 = node_6_8->cellresults;
									if(node_6_8->cellresults){
										node_6_10 = node_6_9->resultscsi_rs_cell;
										if(node_6_9->resultscsi_rs_cell){
											if(node_6_10->rsrq){
												sgnb_addreq_for_ue_mn_serv_csi_rs->rsrq = node_6_10->rsrq->value;
											}else{
												sgnb_addreq_for_ue_mn_serv_csi_rs->rsrq = 128;
											}
											if(node_6_10->rsrp){
												sgnb_addreq_for_ue_mn_serv_csi_rs->rsrp = node_6_10->rsrp->value;
											}else{
												sgnb_addreq_for_ue_mn_serv_csi_rs->rsrp = 128;
											}
											if(node_6_10->sinr){
												sgnb_addreq_for_ue_mn_serv_csi_rs->sinr = node_6_10->sinr->value;
											}else{
												sgnb_addreq_for_ue_mn_serv_csi_rs->sinr = 128;
											}
											rts_fta_process_packet(&cur_packet);
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
// --------------------------------------------------
// ---  Specialized processing for .proto sgnb_addition_request.json, path sgnb_addition_req.json

	sgnb_addreq_for_ue_sn_neigh_ssb = (struct _sgnb_addreq_for_ue_sn_neigh_ssb *)(cur_packet.record.packed.values);
	cur_packet.schema = 408;
	node_7_0 = node_6_0;
	ts_lo = hdr->header->timestamp & 0xffffffff;
	ts_hi = hdr->header->timestamp >> 32;
	sgnb_addreq_for_ue_sn_neigh_ssb->timestamp_ms = (ts_hi - 2208988800) * 1000 + ((ts_lo * 1000) >> 32);
	if(hdr->header->gnbid==NULL)
		sgnb_addreq_for_ue_sn_neigh_ssb->gnb_id = empty_string;
	else
		sgnb_addreq_for_ue_sn_neigh_ssb->gnb_id = hdr->header->gnbid->value;

	node_7_1 = node_7_0->protocolies;
	if(node_7_0->protocolies){
		sgnb_addreq_for_ue_sn_neigh_ssb->id_MeNB_UE_X2AP_ID = node_7_1->id_menb_ue_x2ap_id;
		if(node_7_1->id_menb_ue_x2ap_id_extension){
			sgnb_addreq_for_ue_sn_neigh_ssb->id_MeNB_UE_X2AP_ID_Extension = node_7_1->id_menb_ue_x2ap_id_extension->value;
		}else{
			sgnb_addreq_for_ue_sn_neigh_ssb->id_MeNB_UE_X2AP_ID_Extension = 0;
		}
		node_7_2 = node_7_1->id_menbtosgnbcontainer;
		if(node_7_1->id_menbtosgnbcontainer){
			node_7_3 = node_7_2->criticalextensionschoice1;
			if(node_7_2->criticalextensionschoice1){
				node_7_4 = node_7_3->protocolies;
				if(node_7_3->protocolies){
					node_7_5 = node_7_4->candidatecellinfolistsn;
					if(node_7_4->candidatecellinfolistsn){
						for(i_7_6=0;i_7_6<node_7_5->n_items; i_7_6++){
							node_7_6 = node_7_5->items[i_7_6];
							node_7_7 = node_7_6->measresultneighcelllistnr;
							if(node_7_6->measresultneighcelllistnr){
								for(i_7_8=0;i_7_8<node_7_7->n_items; i_7_8++){
									node_7_8 = node_7_7->items[i_7_8];
									if(node_7_8->physcellid){
										sgnb_addreq_for_ue_sn_neigh_ssb->physCellId = node_7_8->physcellid->value;
										sgnb_addreq_for_ue_sn_neigh_ssb->physCellId_exists = 1;
									}else{
										sgnb_addreq_for_ue_sn_neigh_ssb->physCellId_exists = 0;
									}
									node_7_9 = node_7_8->measresult;
									if(node_7_8->measresult){
										node_7_10 = node_7_9->cellresults;
										if(node_7_9->cellresults){
											node_7_11 = node_7_10->resultsssb_cell;
											if(node_7_10->resultsssb_cell){
												if(node_7_11->rsrq){
													sgnb_addreq_for_ue_sn_neigh_ssb->rsrq = node_7_11->rsrq->value;
												}else{
													sgnb_addreq_for_ue_sn_neigh_ssb->rsrq = 128;
												}
												if(node_7_11->rsrp){
													sgnb_addreq_for_ue_sn_neigh_ssb->rsrp = node_7_11->rsrp->value;
												}else{
													sgnb_addreq_for_ue_sn_neigh_ssb->rsrp = 128;
												}
												if(node_7_11->sinr){
													sgnb_addreq_for_ue_sn_neigh_ssb->sinr = node_7_11->sinr->value;
												}else{
													sgnb_addreq_for_ue_sn_neigh_ssb->sinr = 128;
												}
												rts_fta_process_packet(&cur_packet);
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
// --------------------------------------------------
// ---  Specialized processing for .proto sgnb_addition_request.json, path sgnb_addition_req.json

	sgnb_addreq_for_ue_sn_neigh_csi_rs = (struct _sgnb_addreq_for_ue_sn_neigh_csi_rs *)(cur_packet.record.packed.values);
	cur_packet.schema = 409;
	node_8_0 = node_7_0;
	ts_lo = hdr->header->timestamp & 0xffffffff;
	ts_hi = hdr->header->timestamp >> 32;
	sgnb_addreq_for_ue_sn_neigh_csi_rs->timestamp_ms = (ts_hi - 2208988800) * 1000 + ((ts_lo * 1000) >> 32);
	if(hdr->header->gnbid==NULL)
		sgnb_addreq_for_ue_sn_neigh_csi_rs->gnb_id = empty_string;
	else
		sgnb_addreq_for_ue_sn_neigh_csi_rs->gnb_id = hdr->header->gnbid->value;

	node_8_1 = node_8_0->protocolies;
	if(node_8_0->protocolies){
		sgnb_addreq_for_ue_sn_neigh_csi_rs->id_MeNB_UE_X2AP_ID = node_8_1->id_menb_ue_x2ap_id;
		if(node_8_1->id_menb_ue_x2ap_id_extension){
			sgnb_addreq_for_ue_sn_neigh_csi_rs->id_MeNB_UE_X2AP_ID_Extension = node_8_1->id_menb_ue_x2ap_id_extension->value;
		}else{
			sgnb_addreq_for_ue_sn_neigh_csi_rs->id_MeNB_UE_X2AP_ID_Extension = 0;
		}
		node_8_2 = node_8_1->id_menbtosgnbcontainer;
		if(node_8_1->id_menbtosgnbcontainer){
			node_8_3 = node_8_2->criticalextensionschoice1;
			if(node_8_2->criticalextensionschoice1){
				node_8_4 = node_8_3->protocolies;
				if(node_8_3->protocolies){
					node_8_5 = node_8_4->candidatecellinfolistsn;
					if(node_8_4->candidatecellinfolistsn){
						for(i_8_6=0;i_8_6<node_8_5->n_items; i_8_6++){
							node_8_6 = node_8_5->items[i_8_6];
							node_8_7 = node_8_6->measresultneighcelllistnr;
							if(node_8_6->measresultneighcelllistnr){
								for(i_8_8=0;i_8_8<node_8_7->n_items; i_8_8++){
									node_8_8 = node_8_7->items[i_8_8];
									if(node_8_8->physcellid){
										sgnb_addreq_for_ue_sn_neigh_csi_rs->physCellId = node_8_8->physcellid->value;
										sgnb_addreq_for_ue_sn_neigh_csi_rs->physCellId_exists = 1;
									}else{
										sgnb_addreq_for_ue_sn_neigh_csi_rs->physCellId_exists = 0;
									}
									node_8_9 = node_8_8->measresult;
									if(node_8_8->measresult){
										node_8_10 = node_8_9->cellresults;
										if(node_8_9->cellresults){
											node_8_11 = node_8_10->resultscsi_rs_cell;
											if(node_8_10->resultscsi_rs_cell){
												if(node_8_11->rsrq){
													sgnb_addreq_for_ue_sn_neigh_csi_rs->rsrq = node_8_11->rsrq->value;
												}else{
													sgnb_addreq_for_ue_sn_neigh_csi_rs->rsrq = 128;
												}
												if(node_8_11->rsrp){
													sgnb_addreq_for_ue_sn_neigh_csi_rs->rsrp = node_8_11->rsrp->value;
												}else{
													sgnb_addreq_for_ue_sn_neigh_csi_rs->rsrp = 128;
												}
												if(node_8_11->sinr){
													sgnb_addreq_for_ue_sn_neigh_csi_rs->sinr = node_8_11->sinr->value;
												}else{
													sgnb_addreq_for_ue_sn_neigh_csi_rs->sinr = 128;
												}
												rts_fta_process_packet(&cur_packet);
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
// --------------------------------------------------
// ---  Specialized processing for .proto sgnb_addition_request.json, path sgnb_addition_req.json

	sgnb_addreq_for_ue_mn_neigh_ssb = (struct _sgnb_addreq_for_ue_mn_neigh_ssb *)(cur_packet.record.packed.values);
	cur_packet.schema = 410;
	node_9_0 = node_8_0;
	ts_lo = hdr->header->timestamp & 0xffffffff;
	ts_hi = hdr->header->timestamp >> 32;
	sgnb_addreq_for_ue_mn_neigh_ssb->timestamp_ms = (ts_hi - 2208988800) * 1000 + ((ts_lo * 1000) >> 32);
	if(hdr->header->gnbid==NULL)
		sgnb_addreq_for_ue_mn_neigh_ssb->gnb_id = empty_string;
	else
		sgnb_addreq_for_ue_mn_neigh_ssb->gnb_id = hdr->header->gnbid->value;

	node_9_1 = node_9_0->protocolies;
	if(node_9_0->protocolies){
		sgnb_addreq_for_ue_mn_neigh_ssb->id_MeNB_UE_X2AP_ID = node_9_1->id_menb_ue_x2ap_id;
		if(node_9_1->id_menb_ue_x2ap_id_extension){
			sgnb_addreq_for_ue_mn_neigh_ssb->id_MeNB_UE_X2AP_ID_Extension = node_9_1->id_menb_ue_x2ap_id_extension->value;
		}else{
			sgnb_addreq_for_ue_mn_neigh_ssb->id_MeNB_UE_X2AP_ID_Extension = 0;
		}
		node_9_2 = node_9_1->id_menbtosgnbcontainer;
		if(node_9_1->id_menbtosgnbcontainer){
			node_9_3 = node_9_2->criticalextensionschoice1;
			if(node_9_2->criticalextensionschoice1){
				node_9_4 = node_9_3->protocolies;
				if(node_9_3->protocolies){
					node_9_5 = node_9_4->candidatecellinfolistmn;
					if(node_9_4->candidatecellinfolistmn){
						for(i_9_6=0;i_9_6<node_9_5->n_items; i_9_6++){
							node_9_6 = node_9_5->items[i_9_6];
							node_9_7 = node_9_6->measresultneighcelllistnr;
							if(node_9_6->measresultneighcelllistnr){
								for(i_9_8=0;i_9_8<node_9_7->n_items; i_9_8++){
									node_9_8 = node_9_7->items[i_9_8];
									if(node_9_8->physcellid){
										sgnb_addreq_for_ue_mn_neigh_ssb->physCellId = node_9_8->physcellid->value;
										sgnb_addreq_for_ue_mn_neigh_ssb->physCellId_exists = 1;
									}else{
										sgnb_addreq_for_ue_mn_neigh_ssb->physCellId_exists = 0;
									}
									node_9_9 = node_9_8->measresult;
									if(node_9_8->measresult){
										node_9_10 = node_9_9->cellresults;
										if(node_9_9->cellresults){
											node_9_11 = node_9_10->resultsssb_cell;
											if(node_9_10->resultsssb_cell){
												if(node_9_11->rsrq){
													sgnb_addreq_for_ue_mn_neigh_ssb->rsrq = node_9_11->rsrq->value;
												}else{
													sgnb_addreq_for_ue_mn_neigh_ssb->rsrq = 128;
												}
												if(node_9_11->rsrp){
													sgnb_addreq_for_ue_mn_neigh_ssb->rsrp = node_9_11->rsrp->value;
												}else{
													sgnb_addreq_for_ue_mn_neigh_ssb->rsrp = 128;
												}
												if(node_9_11->sinr){
													sgnb_addreq_for_ue_mn_neigh_ssb->sinr = node_9_11->sinr->value;
												}else{
													sgnb_addreq_for_ue_mn_neigh_ssb->sinr = 128;
												}
												rts_fta_process_packet(&cur_packet);
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
// --------------------------------------------------
// ---  Specialized processing for .proto sgnb_addition_request.json, path sgnb_addition_req.json

	sgnb_addreq_for_ue_mn_neigh_csi_rs = (struct _sgnb_addreq_for_ue_mn_neigh_csi_rs *)(cur_packet.record.packed.values);
	cur_packet.schema = 411;
	node_10_0 = node_9_0;
	ts_lo = hdr->header->timestamp & 0xffffffff;
	ts_hi = hdr->header->timestamp >> 32;
	sgnb_addreq_for_ue_mn_neigh_csi_rs->timestamp_ms = (ts_hi - 2208988800) * 1000 + ((ts_lo * 1000) >> 32);
	if(hdr->header->gnbid==NULL)
		sgnb_addreq_for_ue_mn_neigh_csi_rs->gnb_id = empty_string;
	else
		sgnb_addreq_for_ue_mn_neigh_csi_rs->gnb_id = hdr->header->gnbid->value;

	node_10_1 = node_10_0->protocolies;
	if(node_10_0->protocolies){
		sgnb_addreq_for_ue_mn_neigh_csi_rs->id_MeNB_UE_X2AP_ID = node_10_1->id_menb_ue_x2ap_id;
		if(node_10_1->id_menb_ue_x2ap_id_extension){
			sgnb_addreq_for_ue_mn_neigh_csi_rs->id_MeNB_UE_X2AP_ID_Extension = node_10_1->id_menb_ue_x2ap_id_extension->value;
		}else{
			sgnb_addreq_for_ue_mn_neigh_csi_rs->id_MeNB_UE_X2AP_ID_Extension = 0;
		}
		node_10_2 = node_10_1->id_menbtosgnbcontainer;
		if(node_10_1->id_menbtosgnbcontainer){
			node_10_3 = node_10_2->criticalextensionschoice1;
			if(node_10_2->criticalextensionschoice1){
				node_10_4 = node_10_3->protocolies;
				if(node_10_3->protocolies){
					node_10_5 = node_10_4->candidatecellinfolistmn;
					if(node_10_4->candidatecellinfolistmn){
						for(i_10_6=0;i_10_6<node_10_5->n_items; i_10_6++){
							node_10_6 = node_10_5->items[i_10_6];
							node_10_7 = node_10_6->measresultneighcelllistnr;
							if(node_10_6->measresultneighcelllistnr){
								for(i_10_8=0;i_10_8<node_10_7->n_items; i_10_8++){
									node_10_8 = node_10_7->items[i_10_8];
									if(node_10_8->physcellid){
										sgnb_addreq_for_ue_mn_neigh_csi_rs->physCellId = node_10_8->physcellid->value;
										sgnb_addreq_for_ue_mn_neigh_csi_rs->physCellId_exists = 1;
									}else{
										sgnb_addreq_for_ue_mn_neigh_csi_rs->physCellId_exists = 0;
									}
									node_10_9 = node_10_8->measresult;
									if(node_10_8->measresult){
										node_10_10 = node_10_9->cellresults;
										if(node_10_9->cellresults){
											node_10_11 = node_10_10->resultscsi_rs_cell;
											if(node_10_10->resultscsi_rs_cell){
												if(node_10_11->rsrq){
													sgnb_addreq_for_ue_mn_neigh_csi_rs->rsrq = node_10_11->rsrq->value;
												}else{
													sgnb_addreq_for_ue_mn_neigh_csi_rs->rsrq = 128;
												}
												if(node_10_11->rsrp){
													sgnb_addreq_for_ue_mn_neigh_csi_rs->rsrp = node_10_11->rsrp->value;
												}else{
													sgnb_addreq_for_ue_mn_neigh_csi_rs->rsrp = 128;
												}
												if(node_10_11->sinr){
													sgnb_addreq_for_ue_mn_neigh_csi_rs->sinr = node_10_11->sinr->value;
												}else{
													sgnb_addreq_for_ue_mn_neigh_csi_rs->sinr = 128;
												}
												rts_fta_process_packet(&cur_packet);
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	streaming_protobufs__sg_nbaddition_request__free_unpacked(node_0_0,NULL);
	return 0;
}

gs_uint32_t process_buffer_SGNBMODCONF(gs_uint8_t * buffer, gs_uint32_t buflen){
	char *empty_string = "";
unsigned long long int ts_lo, ts_hi;
	StreamingProtobufs__X2APStreaming *hdr = NULL;
// ------------------------------------------
// ---  Variables for .proto sgnb_modification_confirm.json, path sgnb_mod_confirm.json
	struct _sgnb_mod_conf *sgnb_mod_conf = NULL;
	StreamingProtobufs__SgNBModificationConfirm *node_0_0 = NULL;
	StreamingProtobufs__SgNBModificationConfirmIEs *node_0_1 = NULL;

// --------------------------------------------------
// ---  Specialized processing for .proto sgnb_modification_confirm.json, path sgnb_mod_confirm.json

	sgnb_mod_conf = (struct _sgnb_mod_conf *)(cur_packet.record.packed.values);
	cur_packet.schema = 1301;

	hdr = streaming_protobufs__x2_apstreaming__unpack(NULL, buflen, buffer);
	if(hdr==NULL) return -1;

	node_0_0 = hdr->sgnbmodificationconfirm;
	if(node_0_0==NULL) return -2;
	if(hdr->header==NULL) return -3;

	ts_lo = hdr->header->timestamp & 0xffffffff;
	ts_hi = hdr->header->timestamp >> 32;
	sgnb_mod_conf->timestamp_ms = (ts_hi - 2208988800) * 1000 + ((ts_lo * 1000) >> 32);
	if(hdr->header->gnbid==NULL)
		sgnb_mod_conf->gnb_id = empty_string;
	else
		sgnb_mod_conf->gnb_id = hdr->header->gnbid->value;

	node_0_1 = node_0_0->protocolies;
	if(node_0_0->protocolies){
		sgnb_mod_conf->id_MeNB_UE_X2AP_ID = node_0_1->id_menb_ue_x2ap_id;
		sgnb_mod_conf->id_SgNB_UE_X2AP_ID = node_0_1->id_sgnb_ue_x2ap_id;
		rts_fta_process_packet(&cur_packet);
	}
	streaming_protobufs__sg_nbmodification_confirm__free_unpacked(node_0_0,NULL);
	return 0;
}

gs_uint32_t process_buffer_SGNBMODREQ(gs_uint8_t * buffer, gs_uint32_t buflen){
	char *empty_string = "";
unsigned long long int ts_lo, ts_hi;
	StreamingProtobufs__X2APStreaming *hdr = NULL;
// ------------------------------------------
// ---  Variables for .proto sgnb_modification_request.json, path sgnb_mod_req.json
	struct _sgnb_mod_req *sgnb_mod_req = NULL;
	StreamingProtobufs__SgNBModificationRequest *node_0_0 = NULL;
	StreamingProtobufs__SgNBModificationRequestIEs *node_0_1 = NULL;

// --------------------------------------------------
// ---  Specialized processing for .proto sgnb_modification_request.json, path sgnb_mod_req.json

	sgnb_mod_req = (struct _sgnb_mod_req *)(cur_packet.record.packed.values);
	cur_packet.schema = 1201;

	hdr = streaming_protobufs__x2_apstreaming__unpack(NULL, buflen, buffer);
	if(hdr==NULL) return -1;

	node_0_0 = hdr->sgnbmodificationrequest;
	if(node_0_0==NULL) return -2;
	if(hdr->header==NULL) return -3;

	ts_lo = hdr->header->timestamp & 0xffffffff;
	ts_hi = hdr->header->timestamp >> 32;
	sgnb_mod_req->timestamp_ms = (ts_hi - 2208988800) * 1000 + ((ts_lo * 1000) >> 32);
	if(hdr->header->gnbid==NULL)
		sgnb_mod_req->gnb_id = empty_string;
	else
		sgnb_mod_req->gnb_id = hdr->header->gnbid->value;

	node_0_1 = node_0_0->protocolies;
	if(node_0_0->protocolies){
		if(node_0_1->id_cause && node_0_1->id_cause->protocol){
			sgnb_mod_req->cause_protocol = node_0_1->id_cause->protocol->value;
		}else{
			sgnb_mod_req->cause_protocol = -1;
		}
		sgnb_mod_req->id_MeNB_UE_X2AP_ID = node_0_1->id_menb_ue_x2ap_id;
		if(node_0_1->id_cause && node_0_1->id_cause->transport){
			sgnb_mod_req->cause_transport = node_0_1->id_cause->transport->value;
		}else{
			sgnb_mod_req->cause_transport = -1;
		}
		if(node_0_1->id_menbtosgnbcontainer && node_0_1->id_menbtosgnbcontainer->criticalextensionschoice1 && node_0_1->id_menbtosgnbcontainer->criticalextensionschoice1->protocolies && node_0_1->id_menbtosgnbcontainer->criticalextensionschoice1->protocolies->scgfailureinfo){
			sgnb_mod_req->failureType = node_0_1->id_menbtosgnbcontainer->criticalextensionschoice1->protocolies->scgfailureinfo->failuretype;
		}else{
			sgnb_mod_req->failureType = -1;
		}
		if(node_0_1->id_cause && node_0_1->id_cause->radionetwork){
			sgnb_mod_req->cause_radio_network = node_0_1->id_cause->radionetwork->value;
		}else{
			sgnb_mod_req->cause_radio_network = -1;
		}
		sgnb_mod_req->id_SgNB_UE_X2AP_ID = node_0_1->id_sgnb_ue_x2ap_id;
		if(node_0_1->id_cause && node_0_1->id_cause->misc){
			sgnb_mod_req->cause_misc = node_0_1->id_cause->misc->value;
		}else{
			sgnb_mod_req->cause_misc = -1;
		}
		rts_fta_process_packet(&cur_packet);
	}
	streaming_protobufs__sg_nbmodification_request__free_unpacked(node_0_0,NULL);
	return 0;
}

gs_uint32_t process_buffer_SGNBMODREQACK(gs_uint8_t * buffer, gs_uint32_t buflen){
	char *empty_string = "";
unsigned long long int ts_lo, ts_hi;
	StreamingProtobufs__X2APStreaming *hdr = NULL;
// ------------------------------------------
// ---  Variables for .proto sgnb_modification_request_acknowledge.json, path sgnb_mod_req_ack.json
	struct _sgnb_mod_req_ack *sgnb_mod_req_ack = NULL;
	StreamingProtobufs__SgNBModificationRequestAcknowledge *node_0_0 = NULL;
	StreamingProtobufs__SgNBModificationRequestAcknowledgeIEs *node_0_1 = NULL;

// --------------------------------------------------
// ---  Specialized processing for .proto sgnb_modification_request_acknowledge.json, path sgnb_mod_req_ack.json

	sgnb_mod_req_ack = (struct _sgnb_mod_req_ack *)(cur_packet.record.packed.values);
	cur_packet.schema = 1701;

	hdr = streaming_protobufs__x2_apstreaming__unpack(NULL, buflen, buffer);
	if(hdr==NULL) return -1;

	node_0_0 = hdr->sgnbmodificationrequestacknowledge;
	if(node_0_0==NULL) return -2;
	if(hdr->header==NULL) return -3;

	ts_lo = hdr->header->timestamp & 0xffffffff;
	ts_hi = hdr->header->timestamp >> 32;
	sgnb_mod_req_ack->timestamp_ms = (ts_hi - 2208988800) * 1000 + ((ts_lo * 1000) >> 32);
	if(hdr->header->gnbid==NULL)
		sgnb_mod_req_ack->gnb_id = empty_string;
	else
		sgnb_mod_req_ack->gnb_id = hdr->header->gnbid->value;

	node_0_1 = node_0_0->protocolies;
	if(node_0_0->protocolies){
		sgnb_mod_req_ack->id_MeNB_UE_X2AP_ID = node_0_1->id_menb_ue_x2ap_id;
		sgnb_mod_req_ack->id_SgNB_UE_X2AP_ID = node_0_1->id_sgnb_ue_x2ap_id;
		rts_fta_process_packet(&cur_packet);
	}
	streaming_protobufs__sg_nbmodification_request_acknowledge__free_unpacked(node_0_0,NULL);
	return 0;
}

gs_uint32_t process_buffer_SGNBMODREQREJECT(gs_uint8_t * buffer, gs_uint32_t buflen){
	char *empty_string = "";
unsigned long long int ts_lo, ts_hi;
	StreamingProtobufs__X2APStreaming *hdr = NULL;
// ------------------------------------------
// ---  Variables for .proto sgnb_modification_request_reject.json, path sgnb_mod_req_reject.json
	struct _sgnb_mod_req_reject *sgnb_mod_req_reject = NULL;
	StreamingProtobufs__SgNBModificationRequestReject *node_0_0 = NULL;
	StreamingProtobufs__SgNBModificationRequestRejectIEs *node_0_1 = NULL;

// --------------------------------------------------
// ---  Specialized processing for .proto sgnb_modification_request_reject.json, path sgnb_mod_req_reject.json

	sgnb_mod_req_reject = (struct _sgnb_mod_req_reject *)(cur_packet.record.packed.values);
	cur_packet.schema = 1801;

	hdr = streaming_protobufs__x2_apstreaming__unpack(NULL, buflen, buffer);
	if(hdr==NULL) return -1;

	node_0_0 = hdr->sgnbmodificationrequestreject;
	if(node_0_0==NULL) return -2;
	if(hdr->header==NULL) return -3;

	ts_lo = hdr->header->timestamp & 0xffffffff;
	ts_hi = hdr->header->timestamp >> 32;
	sgnb_mod_req_reject->timestamp_ms = (ts_hi - 2208988800) * 1000 + ((ts_lo * 1000) >> 32);
	if(hdr->header->gnbid==NULL)
		sgnb_mod_req_reject->gnb_id = empty_string;
	else
		sgnb_mod_req_reject->gnb_id = hdr->header->gnbid->value;

	node_0_1 = node_0_0->protocolies;
	if(node_0_0->protocolies){
		if(node_0_1->id_cause && node_0_1->id_cause->protocol){
			sgnb_mod_req_reject->cause_protocol = node_0_1->id_cause->protocol->value;
		}else{
			sgnb_mod_req_reject->cause_protocol = -1;
		}
		sgnb_mod_req_reject->id_MeNB_UE_X2AP_ID = node_0_1->id_menb_ue_x2ap_id;
		if(node_0_1->id_cause && node_0_1->id_cause->transport){
			sgnb_mod_req_reject->cause_transport = node_0_1->id_cause->transport->value;
		}else{
			sgnb_mod_req_reject->cause_transport = -1;
		}
		if(node_0_1->id_cause && node_0_1->id_cause->radionetwork){
			sgnb_mod_req_reject->cause_radio_network = node_0_1->id_cause->radionetwork->value;
		}else{
			sgnb_mod_req_reject->cause_radio_network = -1;
		}
		sgnb_mod_req_reject->id_SgNB_UE_X2AP_ID = node_0_1->id_sgnb_ue_x2ap_id;
		if(node_0_1->id_cause && node_0_1->id_cause->misc){
			sgnb_mod_req_reject->cause_misc = node_0_1->id_cause->misc->value;
		}else{
			sgnb_mod_req_reject->cause_misc = -1;
		}
		rts_fta_process_packet(&cur_packet);
	}
	streaming_protobufs__sg_nbmodification_request_reject__free_unpacked(node_0_0,NULL);
	return 0;
}

gs_uint32_t process_buffer_SGNBMODREQUIRED(gs_uint8_t * buffer, gs_uint32_t buflen){
	char *empty_string = "";
unsigned long long int ts_lo, ts_hi;
	StreamingProtobufs__X2APStreaming *hdr = NULL;
// ------------------------------------------
// ---  Variables for .proto sgnb_modification_required.json, path sgnb_mod_rqd.json
	struct _sgnb_mod_required *sgnb_mod_required = NULL;
	StreamingProtobufs__SgNBModificationRequired *node_0_0 = NULL;
	StreamingProtobufs__SgNBModificationRequiredIEs *node_0_1 = NULL;

// --------------------------------------------------
// ---  Specialized processing for .proto sgnb_modification_required.json, path sgnb_mod_rqd.json

	sgnb_mod_required = (struct _sgnb_mod_required *)(cur_packet.record.packed.values);
	cur_packet.schema = 1901;

	hdr = streaming_protobufs__x2_apstreaming__unpack(NULL, buflen, buffer);
	if(hdr==NULL) return -1;

	node_0_0 = hdr->sgnbmodificationrequired;
	if(node_0_0==NULL) return -2;
	if(hdr->header==NULL) return -3;

	ts_lo = hdr->header->timestamp & 0xffffffff;
	ts_hi = hdr->header->timestamp >> 32;
	sgnb_mod_required->timestamp_ms = (ts_hi - 2208988800) * 1000 + ((ts_lo * 1000) >> 32);
	if(hdr->header->gnbid==NULL)
		sgnb_mod_required->gnb_id = empty_string;
	else
		sgnb_mod_required->gnb_id = hdr->header->gnbid->value;

	node_0_1 = node_0_0->protocolies;
	if(node_0_0->protocolies){
		if(node_0_1->id_cause && node_0_1->id_cause->protocol){
			sgnb_mod_required->cause_protocol = node_0_1->id_cause->protocol->value;
		}else{
			sgnb_mod_required->cause_protocol = -1;
		}
		sgnb_mod_required->id_MeNB_UE_X2AP_ID = node_0_1->id_menb_ue_x2ap_id;
		if(node_0_1->id_cause && node_0_1->id_cause->transport){
			sgnb_mod_required->cause_transport = node_0_1->id_cause->transport->value;
		}else{
			sgnb_mod_required->cause_transport = -1;
		}
		if(node_0_1->id_cause && node_0_1->id_cause->radionetwork){
			sgnb_mod_required->cause_radio_network = node_0_1->id_cause->radionetwork->value;
		}else{
			sgnb_mod_required->cause_radio_network = -1;
		}
		sgnb_mod_required->id_SgNB_UE_X2AP_ID = node_0_1->id_sgnb_ue_x2ap_id;
		if(node_0_1->id_cause && node_0_1->id_cause->misc){
			sgnb_mod_required->cause_misc = node_0_1->id_cause->misc->value;
		}else{
			sgnb_mod_required->cause_misc = -1;
		}
		rts_fta_process_packet(&cur_packet);
	}
	streaming_protobufs__sg_nbmodification_required__free_unpacked(node_0_0,NULL);
	return 0;
}

gs_uint32_t process_buffer_SGNBMODREFUSE(gs_uint8_t * buffer, gs_uint32_t buflen){
	char *empty_string = "";
unsigned long long int ts_lo, ts_hi;
	StreamingProtobufs__X2APStreaming *hdr = NULL;
// ------------------------------------------
// ---  Variables for .proto sgnb_modification_refuse.json, path sgnb_modification_refuse.json
	struct _sgnb_mod_refuse *sgnb_mod_refuse = NULL;
	StreamingProtobufs__SgNBModificationRefuse *node_0_0 = NULL;
	StreamingProtobufs__SgNBModificationRefuseIEs *node_0_1 = NULL;
	StreamingProtobufs__Cause *node_0_2 = NULL;

// --------------------------------------------------
// ---  Specialized processing for .proto sgnb_modification_refuse.json, path sgnb_modification_refuse.json

	sgnb_mod_refuse = (struct _sgnb_mod_refuse *)(cur_packet.record.packed.values);
	cur_packet.schema = 1401;

	hdr = streaming_protobufs__x2_apstreaming__unpack(NULL, buflen, buffer);
	if(hdr==NULL) return -1;

	node_0_0 = hdr->sgnbmodificationrefuse;
	if(node_0_0==NULL) return -2;
	if(hdr->header==NULL) return -3;

	ts_lo = hdr->header->timestamp & 0xffffffff;
	ts_hi = hdr->header->timestamp >> 32;
	sgnb_mod_refuse->timestamp_ms = (ts_hi - 2208988800) * 1000 + ((ts_lo * 1000) >> 32);
	if(hdr->header->gnbid==NULL)
		sgnb_mod_refuse->gnb_id = empty_string;
	else
		sgnb_mod_refuse->gnb_id = hdr->header->gnbid->value;

	node_0_1 = node_0_0->protocolies;
	if(node_0_0->protocolies){
		sgnb_mod_refuse->id_MeNB_UE_X2AP_ID = node_0_1->id_menb_ue_x2ap_id;
		sgnb_mod_refuse->id_SgNB_UE_X2AP_ID = node_0_1->id_sgnb_ue_x2ap_id;
		node_0_2 = node_0_1->id_cause;
		if(node_0_1->id_cause){
			if(node_0_2->radionetwork){
				sgnb_mod_refuse->cause_radio_network = node_0_2->radionetwork->value;
			}else{
				sgnb_mod_refuse->cause_radio_network = -1;
			}
			if(node_0_2->transport){
				sgnb_mod_refuse->cause_transport = node_0_2->transport->value;
			}else{
				sgnb_mod_refuse->cause_transport = -1;
			}
			if(node_0_2->protocol){
				sgnb_mod_refuse->cause_protocol = node_0_2->protocol->value;
			}else{
				sgnb_mod_refuse->cause_protocol = -1;
			}
			if(node_0_2->misc){
				sgnb_mod_refuse->cause_misc = node_0_2->misc->value;
			}else{
				sgnb_mod_refuse->cause_misc = -1;
			}
			rts_fta_process_packet(&cur_packet);
		}
	}
	streaming_protobufs__sg_nbmodification_refuse__free_unpacked(node_0_0,NULL);
	return 0;
}

gs_uint32_t process_buffer_SNSTATUSXFER(gs_uint8_t * buffer, gs_uint32_t buflen){
	char *empty_string = "";
unsigned long long int ts_lo, ts_hi;
	StreamingProtobufs__X2APStreaming *hdr = NULL;
// ------------------------------------------
// ---  Variables for .proto sn_status_transfer.json, path snstatusxfer.json
	struct _sn_status_transfer *sn_status_transfer = NULL;
	StreamingProtobufs__SNStatusTransfer *node_0_0 = NULL;
	StreamingProtobufs__SNStatusTransferIEs *node_0_1 = NULL;
	StreamingProtobufs__ERABsSubjectToStatusTransferList *node_0_2 = NULL;
	StreamingProtobufs__ERABsSubjectToStatusTransferItemIEs *node_0_3 = NULL;
	gs_uint32_t i_0_3;
	StreamingProtobufs__ERABsSubjectToStatusTransferItem *node_0_4 = NULL;
	StreamingProtobufs__ERABsSubjectToStatusTransferItemExtIEs *node_0_5 = NULL;
	gs_uint32_t i_0_5;
	StreamingProtobufs__COUNTvaluePDCPSNlength18 *node_0_6 = NULL;

// --------------------------------------------------
// ---  Specialized processing for .proto sn_status_transfer.json, path snstatusxfer.json

	sn_status_transfer = (struct _sn_status_transfer *)(cur_packet.record.packed.values);
	cur_packet.schema = 1601;

	hdr = streaming_protobufs__x2_apstreaming__unpack(NULL, buflen, buffer);
	if(hdr==NULL) return -1;

	node_0_0 = hdr->snstatustransfer;
	if(node_0_0==NULL) return -2;
	if(hdr->header==NULL) return -3;

	ts_lo = hdr->header->timestamp & 0xffffffff;
	ts_hi = hdr->header->timestamp >> 32;
	sn_status_transfer->timestamp_ms = (ts_hi - 2208988800) * 1000 + ((ts_lo * 1000) >> 32);
	if(hdr->header->gnbid==NULL)
		sn_status_transfer->gnb_id = empty_string;
	else
		sn_status_transfer->gnb_id = hdr->header->gnbid->value;

	node_0_1 = node_0_0->protocolies;
	if(node_0_0->protocolies){
		sn_status_transfer->id_Old_eNB_UE_X2AP_ID = node_0_1->id_old_enb_ue_x2ap_id;
		if(node_0_1->id_sgnb_ue_x2ap_id){
			sn_status_transfer->id_SgNB_UE_X2AP_ID = node_0_1->id_sgnb_ue_x2ap_id->value;
		}else{
			sn_status_transfer->id_SgNB_UE_X2AP_ID = 0;
		}
		node_0_2 = node_0_1->id_e_rabs_subjecttostatustransfer_list;
		if(node_0_1->id_e_rabs_subjecttostatustransfer_list){
			for(i_0_3=0;i_0_3<node_0_2->n_items; i_0_3++){
				node_0_3 = node_0_2->items[i_0_3];
				node_0_4 = node_0_3->id_e_rabs_subjecttostatustransfer_item;
				if(node_0_3->id_e_rabs_subjecttostatustransfer_item){
					sn_status_transfer->e_RAB_ID = node_0_4->e_rab_id;
					for(i_0_5=0;i_0_5<node_0_4->n_ie_extensions; i_0_5++){
						node_0_5 = node_0_4->ie_extensions[i_0_5];
						node_0_6 = node_0_5->id_dlcountvaluepdcp_snlength18;
						if(node_0_5->id_dlcountvaluepdcp_snlength18){
							sn_status_transfer->pDCP_SNlength18 = node_0_6->pdcp_snlength18;
							rts_fta_process_packet(&cur_packet);
						}
					}
				}
			}
		}
	}
	streaming_protobufs__snstatus_transfer__free_unpacked(node_0_0,NULL);
	return 0;
}


int init_cur_packet(){
	cur_packet.ptype=PTYPE_STRUCT;
	cur_packet.record.packed.values = (void *)(malloc(10000));

	if(cur_packet.record.packed.values==NULL){
		print_error("could not malloc a data block for cur_packet in init_cur_packet.");
		exit(10);
	}
	return 0;
}
//----------------  END Specialized proto parsing -----------

static void dproto_replay_check_messages() {
    if (fta_start_service(0)<0) {
        print_error("Error:in processing the msg queue for a replay file");
        exit(9);
    }
}

//	Read length bytes from the current socket into buffer.
static gs_uint32_t gs_read_buffer(gs_uint8_t * buffer, gs_uint32_t length){
    gs_uint32_t used=0;
    gs_uint32_t cur;
    fd_set socket_rset;
    fd_set socket_eset;
    struct timeval socket_timeout;
    int retval;
    
    FD_ZERO(&socket_rset);
    FD_SET(socket_desc,&socket_rset);
    FD_ZERO(&socket_eset);
    FD_SET(socket_desc,&socket_eset);
    // timeout in one millisecon
    socket_timeout.tv_sec=0;
    socket_timeout.tv_usec=1000;
    
    if ((retval=select(socket_desc+1,&socket_rset,0,&socket_eset,&socket_timeout))<=0) {
        if (retval==0) {
            // caught a timeout
            return -1;
        }
        return -2;
    }
    
    while(used < length) {
        if ((cur=read(socket_desc,&(buffer[used]),length-used))<=0) {
            print_error("ERROR:could not read data from PROTO stream");
            return -2;
        }
        used+=cur;
    }
	return 0;
}

//	query gshub and use that info to open a socket
static void init_socket() {
	endpoint gshub;
	endpoint srcinfo;
 	struct sockaddr_in server;
	
	if (get_hub(&gshub)!=0) {
		print_error("ERROR:could not find gshub for data source");
		exit(0);
	}
    
	if (get_streamsource(gshub,name,&srcinfo,1) !=0) {
		print_error("ERROR:could not find data source for stream\n");
		exit(0);
	}
    
	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        print_error("ERROR:could not create socket for data stream");
		exit(0);
    }
	server.sin_addr.s_addr = srcinfo.ip;
    server.sin_family = AF_INET;
    server.sin_port = srcinfo.port;
    
	if (connect(socket_desc , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
		print_error("ERROR: could not open connection to data source");
		exit(0);
	}
    
}

int read_fifo(struct pollfd* pfd, char* buffer, size_t len, time_t timeout) {
    int i, bytes_read = 0;
    while (bytes_read < len) {      
        if (poll(pfd, 1, timeout)) {         
            if (pfd->revents & POLLIN) {
				while ((i = read(pfd->fd,buffer+bytes_read,len-bytes_read))==-1 && errno==EINTR);
                if (i <= 0) {
                    break;		// writer closed fifo or error
                } else {
                    bytes_read += i;
                } 
            } else
                break;			// writer closed fifo
        }
        if (!bytes_read)
            return -1;			// timeout
    }
    return bytes_read;  
}

//	proceed to the next file
static void next_file() {
	int open_flag = O_RDONLY;
	if (fifo)
		open_flag |= O_NONBLOCK;

	struct stat s;
	if (verbose) {
		fprintf(stderr,"Opening %s\n",name);
	}
	while (lstat(name,&s)!=0) {
		if (errno!=ENOENT) {
			print_error("dproto::lstat unexpected return value");
			exit(10);
		}
		dproto_replay_check_messages();
		usleep(10000);
	}
	if  (fd > 0) {
		close(fd);
	}
    if ((fd=open(name,open_flag)) <= 0) {
        print_error("dproto::open failed ");
        exit(10);
    }
	// setup polling for this file descriptor
	pfd.fd = fd;
	pfd.events = POLLIN;

	if (singlefile==0) {
		unlink(name);
	}
}

//	Perform initialization when reading from a file
static gs_retval_t dproto_replay_init(gs_sp_t device) {
    gs_sp_t  verbosetmp;
    gs_sp_t  delaytmp;
    gs_sp_t  gshubtmp;
    gs_sp_t  tempdel;
    gs_sp_t  singlefiletmp;
    gs_sp_t  fifotmp;	
    
    if ((name=get_iface_properties(device,"filename"))==0) {
		print_error("dproto_init::No protobuf \"Filename\" defined");
		exit(0);
	}
    
    if ((verbosetmp=get_iface_properties(device,"verbose"))!=0) {
        if (strncmp(verbosetmp,"TRUE",4)==0) {
            verbose=1;
            fprintf(stderr,"VERBOSE ENABLED\n");
        } else {
            fprintf(stderr,"VERBOSE DISABLED\n");
        }
    }
    if ((singlefiletmp=get_iface_properties(device,"singlefile"))!=0) {
        if (strncmp(singlefiletmp,"TRUE",4)==0) {
            singlefile=1;
            if (verbose)
                fprintf(stderr,"SINGLEFILE ENABLED\n");
        } else {
            if (verbose)
                fprintf(stderr,"SINGLEFILE DISABLED\n");
        }
    }
    if ((fifotmp=get_iface_properties(device,"fifo"))!=0) {
        if (strncmp(fifotmp,"TRUE",4)==0) {
            fifo=1;
            if (verbose)
                fprintf(stderr,"FIFO ENABLED\n");
        } else {
            if (verbose)
                fprintf(stderr,"FIFO DISABLED\n");
        }
    }	
    
    if ((delaytmp=get_iface_properties(device,"startupdelay"))!=0) {
        if (verbose) {
            fprintf(stderr,"Startup delay of %u seconds\n",atoi(get_iface_properties(device,"startupdelay")));
        }
        startupdelay=atoi(get_iface_properties(device,"startupdelay"));
    }
    if ((gshubtmp=get_iface_properties(device,"gshub"))!=0) {
        if (verbose) {
            fprintf(stderr,"PROTO format using gshub\n");
        }
        gshub=1;
    }
	init_cur_packet();
    
    return 0;
}

//	Read one message from a socket
static gs_retval_t dproto_read_socket()
{
	gs_uint32_t i;
	gs_uint32_t p;
	gs_uint32_t x;
	gs_uint32_t pkg_len;
	gs_int32_t r;
	gs_retval_t ret=0;
	gs_uint32_t done;
	char *pkg_len_s;
	char *timestamp_s;
    
	if((ret=gs_read_buffer(line,28))<0) { return ret;}
	pkg_len_s = line+4;
	pkg_len = atoi(pkg_len_s);

	if((ret=gs_read_buffer(line,pkg_len))<0) { return ret;}

	cur_packet.systemTime=time(0);
	ret = process_buffer(line, pkg_len);
	if(ret < 0){
            fprintf(stderr,"proto rejected by device %s, err=%d\n",this_device, ret);
        }

    return 0;
}
    
// read one message from a file
static gs_retval_t dproto_read_tuple(){
    gs_uint32_t retlen=0;
    gs_uint32_t done=0;
    gs_uint32_t pkg_len=0;
    gs_uint32_t eof=0;
	char *pkg_len_s;
	char *timestamp_s;
	gs_retval_t ret;

    if (fd==-1) next_file();

	retlen = read_fifo(&pfd, line, 28, 10);	// use 10ms timeout

	if(retlen==0){
		eof=1;
	}else if(retlen==-1) {		
		return -1;		// -1 indicates a timeout
	}else if(retlen == 28) {
		pkg_len_s = line+4;
		timestamp_s = line+12;
		pkg_len = atoi(pkg_len_s);

		if(pkg_len >= MAXLINE){
// TODO be more graceful here, but a large pkg_len likely indicates
// 		a garbaged file.
			print_error("Error in dproto_read_tuple, message too long.");
			fprintf(stderr,"Error in dproto_read_tuple, message length is %d, max is %d\n",pkg_len, MAXLINE);
			exit(10);
		}

		// once we received header we will wait for the main message indefinetly
		retlen = read_fifo(&pfd, line, pkg_len, -1);
		if(retlen<pkg_len){
			print_error("Error in dproto_read_tuple, line too short.");
			fprintf(stderr,"Error, read %d bytes, expecting %d\n",retlen, pkg_len);
			eof=1;
		}

		if(eof==0){
			cur_packet.systemTime=time(0);
			ret = process_buffer(line, pkg_len);
			if(ret < 0){
            	fprintf(stderr,"proto rejected by device %s, err=%d\n",this_device, ret);
        	}
		}
	}
	if(eof){
  	    if (singlefile==1) {
           	if(verbose) {
               	fprintf(stderr,"SINGLEFILE PROCESSING DONE!\n");
           	}
           	if (verbose) {
               	fprintf(stderr,"RTS SAYS BYe\n");
           	}
           	return -2;
       	} else {
           	next_file();
       	}
	}

	return 0;
}
    
//	Main loop for processing records from a file or socket    
static gs_retval_t dproto_process_file(){
    unsigned cnt=0;
    static unsigned totalcnt=0;

    gs_retval_t retval;
    for(cnt=0;cnt<50000;cnt++) {
        if (gshub!=0) {
            retval=dproto_read_socket();
        } else {
            retval=dproto_read_tuple();
        }
        if (retval==-1) return 0; // got a timeout so service message queue
        if (retval==-2) {
            // we signal that everything is done if we either see an EOF tuple OR the socket is closed by the peer
            if (verbose)
                fprintf(stderr,"Done processing waiting for things to shut down\n");
            rts_fta_done();
            // now just service message queue until we get killed or loose connectivity
            while (0==0) {
                fta_start_service(0); // service all waiting messages
                usleep(1000); // sleep a millisecond
            }
        } 
    }
    totalcnt=totalcnt+cnt;
    if (verbose) {
        fprintf(stderr,"Processed %u messages from %s\n",totalcnt, name);
    }
    return 0;
}
    
//	Entry for processing this interface
gs_retval_t main_dproto(gs_int32_t devicenum, gs_sp_t device, gs_int32_t mapcnt, gs_sp_t map[]) {
    gs_uint32_t cont;
    endpoint mygshub;
        
    dproto_replay_init(device); // will call init_cur_packet
    this_device = strdup(device); // save for error messages.
        
    /* initalize host_lib */
    if (verbose) {
        fprintf(stderr,"Init LFTAs for %s\n",device);
    }
        
    if (hostlib_init(LFTA,0,devicenum,mapcnt,map)<0) {
        fprintf(stderr,"%s::error:could not initiate host lib for clearinghouse\n", device);
        exit(7);
    }

//--------------------------------------------
//----  Generated dispatch code 
	if(strcmp(device,"CONRELEASE")==0){
		process_buffer = &process_buffer_CONRELEASE;
	}
	if(strcmp(device,"RATDATAUSAGE")==0){
		process_buffer = &process_buffer_RATDATAUSAGE;
	}
	if(strcmp(device,"RECONCOMPLETE")==0){
		process_buffer = &process_buffer_RECONCOMPLETE;
	}
	if(strcmp(device,"RELCONF")==0){
		process_buffer = &process_buffer_RELCONF;
	}
	if(strcmp(device,"RELREQ")==0){
		process_buffer = &process_buffer_RELREQ;
	}
	if(strcmp(device,"RELREQACK")==0){
		process_buffer = &process_buffer_RELREQACK;
	}
	if(strcmp(device,"SGNBRELEASERQD")==0){
		process_buffer = &process_buffer_SGNBRELEASERQD;
	}
	if(strcmp(device,"RRCXFER")==0){
		process_buffer = &process_buffer_RRCXFER;
	}
	if(strcmp(device,"ADDREQREJECT")==0){
		process_buffer = &process_buffer_ADDREQREJECT;
	}
	if(strcmp(device,"SGNB_ADDITION_REQ_ACK")==0){
		process_buffer = &process_buffer_SGNB_ADDITION_REQ_ACK;
	}
	if(strcmp(device,"SGNB_ADDITION_REQ")==0){
		process_buffer = &process_buffer_SGNB_ADDITION_REQ;
	}
	if(strcmp(device,"SGNBMODCONF")==0){
		process_buffer = &process_buffer_SGNBMODCONF;
	}
	if(strcmp(device,"SGNBMODREQ")==0){
		process_buffer = &process_buffer_SGNBMODREQ;
	}
	if(strcmp(device,"SGNBMODREQACK")==0){
		process_buffer = &process_buffer_SGNBMODREQACK;
	}
	if(strcmp(device,"SGNBMODREQREJECT")==0){
		process_buffer = &process_buffer_SGNBMODREQREJECT;
	}
	if(strcmp(device,"SGNBMODREQUIRED")==0){
		process_buffer = &process_buffer_SGNBMODREQUIRED;
	}
	if(strcmp(device,"SGNBMODREFUSE")==0){
		process_buffer = &process_buffer_SGNBMODREFUSE;
	}
	if(strcmp(device,"SNSTATUSXFER")==0){
		process_buffer = &process_buffer_SNSTATUSXFER;
	}
	if(process_buffer == NULL){
		fprintf(stderr,"Error, interface %s not recognized\n",device);
		exit(8);
	}
//--------------------------------------------

        
    fta_init(device); /*xxx probably should get error code back put Ted doesn't give me one*/
        
    cont=startupdelay+time(0);
        
    if (verbose) { fprintf(stderr,"Start startup delay"); }
        
    while (cont>time(NULL)) {
        if (fta_start_service(0)<0) {
            fprintf(stderr,"%s::error:in processing the msg queue\n", device);
            exit(9);
        }
        usleep(1000); /* sleep for one millisecond */
    }
        
    if (verbose) { fprintf(stderr,"... Done\n"); }
        
    // open the connection to the data source
    if (gshub!=0) { init_socket();}
        
    // wait to process till we get the signal from GSHUB
    if (get_hub(&mygshub)!=0) {
        print_error("ERROR:could not find gshub for data source");
        exit(0);
    }
    while(get_startprocessing(mygshub,get_instance_name(),0)!=0) {
        usleep(100);
        if (fta_start_service(0)<0) {
            fprintf(stderr,"%s::error:in processing the msg queue\n", device);
            exit(9);
        }
    }
        
   /* now we enter an endless loop to process data */
    if (verbose) {
        fprintf(stderr,"Start processing %s\n",device);
    }
        
    while (1==1) {
        if (dproto_process_file()<0) {
            fprintf(stderr,"%s::error:in processing packets\n", device);
            exit(8);
        }
        /* process all messages on the message queue*/
        if (fta_start_service(0)<0) {
            fprintf(stderr,"%s::error:in processing the msg queue\n", device);
            exit(9);
        }
    }
    return 0;
}
 