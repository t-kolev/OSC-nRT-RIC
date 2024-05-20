.. This work is licensed under a Creative Commons Attribution 4.0 International License.
.. SPDX-License-Identifier: CC-BY-4.0
.. Copyright (C) 2019 AT&T


RIC Measurement Campaign (MC) supported KPIs
============================================

name: addreq_pdf_nr_cell
------------------------

description: histogram of neighboring cell RSRP, aggregated by gnb_id / cell id

keys: CELL_ID, GNB_ID

timestamp: TS

- ULLONG TS
- FLOAT measurementInterval
- UINT CELL_ID
- STRING GNB_ID
- INT cnt
- UINT rsrp_vbad
- UINT rsrp_bad
- UINT rsrp_medium
- UINT rsrp_good
- UINT rsrp_vgood

sources: sgnb_addreq_for_ue_mn_neigh_ssb

transitive sources: SGNB_ADDITION_REQ.sgnb_addreq_for_ue_mn_neigh_ssb

Interface map: SGNB_ADDITION_REQ (sgnb_addition_request)

name: addreq_pdf_nr_gnb
-----------------------

description: histogram of neighboring cell RSRP, aggregated by GNB, as computed from addition request events.

keys: GNB_ID

timestamp: TS

- ULLONG TS
- FLOAT measurementInterval
- STRING GNB_ID
- INT cnt
- UINT rsrp_vbad
- UINT rsrp_bad
- UINT rsrp_medium
- UINT rsrp_good
- UINT rsrp_vgood

sources: sgnb_addreq_for_ue_mn_neigh_ssb

transitive sources: SGNB_ADDITION_REQ.sgnb_addreq_for_ue_mn_neigh_ssb

Interface map: SGNB_ADDITION_REQ (sgnb_addition_request)

name: addreq_stats_nr_cell
--------------------------

description: statistics about neighboring cell RSRP aggregated by cell id

keys: CELL_ID, GNB_ID

timestamp: TS

- ULLONG TS
- FLOAT measurementInterval
- UINT CELL_ID
- STRING GNB_ID
- INT cnt
- INT min_rsrp
- INT pctl_05_rsrp
- INT median_rsrp
- INT pctl_95_rsrp
- FLOAT stddev_rsrp
- INT max_rsrp

sources: sgnb_addreq_for_ue_mn_neigh_ssb

transitive sources: SGNB_ADDITION_REQ.sgnb_addreq_for_ue_mn_neigh_ssb

Interface map: SGNB_ADDITION_REQ (sgnb_addition_request)

name: addreq_stats_nr_gnb
-------------------------

description: statistics about neighboring cell RSRP aggregated by GNB, as computed from addition request events.

keys: GNB_ID

timestamp: TS

- ULLONG TS
- FLOAT measurementInterval
- STRING GNB_ID
- INT cnt
- INT min_rsrp
- INT pctl_05_rsrp
- INT median_rsrp
- INT pctl_95_rsrp
- FLOAT stddev_rsrp
- INT max_rsrp

sources: sgnb_addreq_for_ue_mn_neigh_ssb

transitive sources: SGNB_ADDITION_REQ.sgnb_addreq_for_ue_mn_neigh_ssb

Interface map: SGNB_ADDITION_REQ (sgnb_addition_request)

name: addreq_success_stats
--------------------------

description: statistics on the time to successfully make a DC connection

keys: GNB_ID

timestamp: TS

- STRING GNB_ID
- ULLONG TS
- FLOAT measurementInterval
- FLOAT min_success_time
- FLOAT max_success_time
- FLOAT avg_success_time
- FLOAT pctl_05_success_time
- FLOAT pctl_95_success_time
- FLOAT stddev_success_time

sources: add_req_start, add_req_success

transitive sources: SGNB_ADDITION_REQ.sgnb_addreq_for_ue, RECONCOMPLETE.reconfig_success

Interface map: RECONCOMPLETE (sgnb_reconfiguration_complete), SGNB_ADDITION_REQ (sgnb_addition_request)

name: dc_release_debug
----------------------

timestamp: TS

- STRING name
- INT cnt
- ULLONG TS
- FLOAT measurementInterval

sources: dc_release

transitive sources: CONRELEASE.dc_release

Interface map: CONRELEASE (ue_context_release)

name: dl_sched_trace_stats
--------------------------

description: dl sched trace stats

keys: eutran_trace_id, eci

timestamp: TS

- ULLONG TS
- UINT eci
- ULLONG eutran_trace_id
- LLONG ue_num_schedTTIs
- FLOAT ue_num_schedTTIs_per_time
- FLOAT ue_avg_PRB_alloc_rate
- FLOAT ue_avg_PRB_alloc_rate_per_TTI
- LLONG ue_HARQ_pid_count
- FLOAT ue_HARQ_pid_count_per_time
- FLOAT ue_schedTTIs_MIMO_percent
- FLOAT ue_schedTTIs_TxDiversity_percent
- FLOAT ue_HARQ_retx_pid_count
- FLOAT ue_MAC_PDU_init_Tx_failed_percent
- FLOAT ue_MAC_PDU_last_Tx_failed_percent

sources: lte_dl_sched_trace

transitive sources: LTE_PCMD.lte_dl_sched_trace

name: drb_pdcp_pdu_stats
------------------------

description: drb pdcp pdu stats

keys: drb_Id, eutran_trace_id, eci

timestamp: TS

- ULLONG TS
- UINT eci
- ULLONG eutran_trace_id
- LLONG drb_Id
- LLONG ue_drb_pdcppdu_count
- FLOAT ue_drb_pdcppdu_count_per_time
- FLOAT ue_drb_pdcppdu_discard_rate

sources: lte_rb_thpt

transitive sources: LTE_PCMD.lte_rb_thpt

name: erab_stats
----------------

description: number of admitted bearers and the distribution of their qCI

keys: GNB_ID

timestamp: TS

- ULLONG TS
- STRING GNB_ID
- FLOAT measurementInterval
- INT total_erabs
- UINT qCI_1
- UINT qCI_2
- UINT qCI_3
- UINT qCI_4
- UINT qCI_5
- UINT qCI_6
- UINT qCI_7
- UINT qCI_8
- UINT qCI_9
- UINT qCI_other

sources: erab_stats_join

transitive sources: SGNB_ADDITION_REQ_ACK.eRABs_acked_for_admit_for_ue, RECONCOMPLETE.reconfig_success

Interface map: RECONCOMPLETE (sgnb_reconfiguration_complete), SGNB_ADDITION_REQ_ACK (sgnb_addition_request_acknowledge)

name: erab_stats_pci
--------------------

description: number of admitted bearers and the distribution of their qCI, by physical cell id

keys: physCellId, GNB_ID

timestamp: TS

- ULLONG TS
- STRING GNB_ID
- UINT physCellId
- FLOAT measurementInterval
- INT total_erabs
- UINT qCI_1
- UINT qCI_2
- UINT qCI_3
- UINT qCI_4
- UINT qCI_5
- UINT qCI_6
- UINT qCI_7
- UINT qCI_8
- UINT qCI_9
- UINT qCI_other

sources: erab_stats_join, gnb_ueid_cellid_map

transitive sources: SGNB_ADDITION_REQ_ACK.eRABs_acked_for_admit_for_ue, RECONCOMPLETE.reconfig_success, SGNB_ADDITION_REQ_ACK.add_req_ack_cellid

Interface map: RECONCOMPLETE (sgnb_reconfiguration_complete), SGNB_ADDITION_REQ_ACK (sgnb_addition_request_acknowledge)

name: handovers_gnb
-------------------

description: Number of handovers on a per-gtp_teid basis

keys: GTP_TEID, GNB_ID

timestamp: TS

- ULLONG TS
- FLOAT measurementInterval
- STRING GTP_TEID
- STRING GNB_ID
- INT total_addition_requests
- UINT n_handovers
- UINT n_ping_pong

sources: handovers_join

transitive sources: SGNB_ADDITION_REQ.sgnb_addreq_gtp_teid, RECONCOMPLETE.reconfig_success

Interface map: RECONCOMPLETE (sgnb_reconfiguration_complete), SGNB_ADDITION_REQ (sgnb_addition_request)

name: ho_counts_gtp_teid
------------------------

description: Number of handovers, by UE (gTP_TEID)

keys: gTP_TEID, GNB_ID

timestamp: TS

- ULLONG TS
- STRING GNB_ID
- STRING gTP_TEID
- FLOAT measurementInterval
- INT n_handovers

sources: sgnb_mod_req_ack, sgnb_mod_conf, gnb_ueid_teid_map

transitive sources: SGNBMODREQACK.sgnb_mod_req_ack, SGNBMODCONF.sgnb_mod_conf, SGNB_ADDITION_REQ.sgnb_addreq_gtp_teid, RECONCOMPLETE.reconfig_success

Interface map: RECONCOMPLETE (sgnb_reconfiguration_complete), SGNBMODREQACK (sgnb_modification_request_acknowledge), SGNB_ADDITION_REQ (sgnb_addition_request), SGNBMODCONF (sgnb_modification_confirm)

name: mc_connected_cnt
----------------------

description: Number of dual connected sessions

keys: GNB_ID

timestamp: TS

- ULLONG TS
- STRING GNB_ID
- FLOAT measurementInterval
- INT count_connected_ue

sources: dc_events

transitive sources: RECONCOMPLETE.reconfig_success, CONRELEASE.dc_release

Interface map: RECONCOMPLETE (sgnb_reconfiguration_complete), CONRELEASE (ue_context_release)

name: mc_connected_cnt_pci
--------------------------

description: Number of dual connected users by gnb and pci

keys: physCellId, GNB_ID

timestamp: TS

- ULLONG TS
- STRING GNB_ID
- UINT physCellId
- FLOAT measurementInterval
- INT count_connected_ue

sources: dc_events_pci

transitive sources: RECONCOMPLETE.reconfig_success, CONRELEASE.dc_release, SGNB_ADDITION_REQ_ACK.add_req_ack_cellid

Interface map: RECONCOMPLETE (sgnb_reconfiguration_complete), SGNB_ADDITION_REQ_ACK (sgnb_addition_request_acknowledge), CONRELEASE (ue_context_release)

name: mc_connection_stats
-------------------------

description: statistics about the length of dual connected sessions by gnb

keys: GNB_ID

timestamp: TS

- ULLONG TS
- STRING GNB_ID
- FLOAT measurementInterval
- FLOAT min_connected_time
- FLOAT max_connected_time
- FLOAT avg_connected_time
- FLOAT pctl_05_connected_time
- FLOAT pctl_95_connected_time
- FLOAT stddev_connected_time

sources: mc_disconnected_ues

transitive sources: RECONCOMPLETE.reconfig_success, CONRELEASE.dc_release

Interface map: RECONCOMPLETE (sgnb_reconfiguration_complete), CONRELEASE (ue_context_release)

name: mc_connection_stats_gtp_teid
----------------------------------

description: statistics about the length of dual connected sessions, by gtp_teid

keys: gTP_TEID, GNB_ID

timestamp: TS

- ULLONG TS
- STRING GNB_ID
- STRING gTP_TEID
- FLOAT measurementInterval
- FLOAT min_connected_time
- FLOAT max_connected_time
- FLOAT avg_connected_time
- FLOAT pctl_05_connected_time
- FLOAT pctl_95_connected_time
- FLOAT stddev_connected_time

sources: mc_disconnected_ues, gnb_ueid_teid_map

transitive sources: RECONCOMPLETE.reconfig_success, CONRELEASE.dc_release, SGNB_ADDITION_REQ.sgnb_addreq_gtp_teid

Interface map: RECONCOMPLETE (sgnb_reconfiguration_complete), SGNB_ADDITION_REQ (sgnb_addition_request), CONRELEASE (ue_context_release)

name: mc_connects_cnt
---------------------

description: number of DC connection requests, by GNB

keys: GNB_ID

timestamp: TS

- ULLONG TS
- STRING GNB_ID
- FLOAT measurementInterval
- INT count_ue_connects

sources: dc_events

transitive sources: RECONCOMPLETE.reconfig_success, CONRELEASE.dc_release

Interface map: RECONCOMPLETE (sgnb_reconfiguration_complete), CONRELEASE (ue_context_release)

name: mc_connects_cnt_gtp_teid
------------------------------

description: number of DC connection requests by UE

keys: gTP_TEID, GNB_ID

timestamp: TS

- ULLONG TS
- STRING GNB_ID
- STRING gTP_TEID
- FLOAT measurementInterval
- INT count_ue_connects

sources: dc_events, gnb_ueid_teid_map

transitive sources: RECONCOMPLETE.reconfig_success, CONRELEASE.dc_release, SGNB_ADDITION_REQ.sgnb_addreq_gtp_teid

Interface map: RECONCOMPLETE (sgnb_reconfiguration_complete), SGNB_ADDITION_REQ (sgnb_addition_request), CONRELEASE (ue_context_release)

name: mc_connects_cnt_pci
-------------------------

description: number of DC connection requests, by GNB and PCI

keys: physCellId, GNB_ID

timestamp: TS

- ULLONG TS
- STRING GNB_ID
- UINT physCellId
- FLOAT measurementInterval
- INT count_ue_connects

sources: dc_events_pci

transitive sources: RECONCOMPLETE.reconfig_success, CONRELEASE.dc_release, SGNB_ADDITION_REQ_ACK.add_req_ack_cellid

Interface map: RECONCOMPLETE (sgnb_reconfiguration_complete), SGNB_ADDITION_REQ_ACK (sgnb_addition_request_acknowledge), CONRELEASE (ue_context_release)

name: mc_disconnects_cnt
------------------------

description: number of DC connection releases

keys: GNB_ID

timestamp: TS

- ULLONG TS
- STRING GNB_ID
- FLOAT measurementInterval
- INT count_ue_disconnects

sources: dc_events

transitive sources: RECONCOMPLETE.reconfig_success, CONRELEASE.dc_release

Interface map: RECONCOMPLETE (sgnb_reconfiguration_complete), CONRELEASE (ue_context_release)

name: mc_unique_ue_cnt
----------------------

description: Number of distinct UEs making a DC request or release

keys: GNB_ID

timestamp: TS

- ULLONG TS
- STRING GNB_ID
- FLOAT measurementInterval
- INT count_unique_ue

sources: dc_events

transitive sources: RECONCOMPLETE.reconfig_success, CONRELEASE.dc_release

Interface map: RECONCOMPLETE (sgnb_reconfiguration_complete), CONRELEASE (ue_context_release)

name: mc_unique_ue_pci_cnt
--------------------------

description: Number of distinct UEs making a DC request or release by pci

keys: physCellId, GNB_ID

timestamp: TS

- ULLONG TS
- STRING GNB_ID
- UINT physCellId
- FLOAT measurementInterval
- INT count_unique_ue

sources: dc_events_pci

transitive sources: RECONCOMPLETE.reconfig_success, CONRELEASE.dc_release, SGNB_ADDITION_REQ_ACK.add_req_ack_cellid

Interface map: RECONCOMPLETE (sgnb_reconfiguration_complete), SGNB_ADDITION_REQ_ACK (sgnb_addition_request_acknowledge), CONRELEASE (ue_context_release)

name: mod_failure_cause_gtp_teid
--------------------------------

description: distribution of causes for a sgnb modification failure, by UE (gtp_teid)

keys: gTP_TEID, GNB_ID

timestamp: TS

- STRING GNB_ID
- STRING gTP_TEID
- ULLONG TS
- FLOAT measurementInterval
- INT total_reconfig_refuse
- UINT count_radio_network
- UINT count_transport
- UINT count_protocol
- UINT count_misc

sources: sgnb_mod_req_reject, mod_status_refuse_cause_base, gnb_ueid_teid_map, gnb_ueid_teid_map

transitive sources: SGNBMODREQREJECT.sgnb_mod_req_reject, SGNBMODREFUSE.sgnb_mod_refuse, SGNB_ADDITION_REQ.sgnb_addreq_gtp_teid, RECONCOMPLETE.reconfig_success

Interface map: RECONCOMPLETE (sgnb_reconfiguration_complete), SGNBMODREFUSE (sgnb_modification_refuse), SGNB_ADDITION_REQ (sgnb_addition_request), SGNBMODREQREJECT (sgnb_modification_request_reject)

name: mod_req_failure_distribution
----------------------------------

description: distribution of causes of a modification request failure

keys: GNB_ID

timestamp: TS

- ULLONG TS
- STRING GNB_ID
- FLOAT measurementInterval
- INT cnt
- UINT count_protobuf_unspecified
- UINT count_t310_Expiry
- UINT count_randomAccessProblem
- UINT count_rlc_MaxNumRetx
- UINT count_synchReconfigFailure_SCG
- UINT count_scg_reconfigFailure
- UINT count_srb3_IntegrityFailure

sources: base_mod_req_failure_distribution

transitive sources: SGNBMODREQ.sgnb_mod_req

Interface map: SGNBMODREQ (sgnb_modification_request)

name: mod_req_failure_distribution_gtp_teid
-------------------------------------------

description: distribution of causes of a modification request failure

keys: gTP_TEID, GNB_ID

timestamp: TS

- ULLONG TS
- STRING GNB_ID
- STRING gTP_TEID
- FLOAT measurementInterval
- INT cnt
- UINT count_protobuf_unspecified
- UINT count_t310_Expiry
- UINT count_randomAccessProblem
- UINT count_rlc_MaxNumRetx
- UINT count_synchReconfigFailure_SCG
- UINT count_scg_reconfigFailure
- UINT count_srb3_IntegrityFailure

sources: base_mod_req_failure_distribution, gnb_ueid_teid_map

transitive sources: SGNBMODREQ.sgnb_mod_req, SGNB_ADDITION_REQ.sgnb_addreq_gtp_teid, RECONCOMPLETE.reconfig_success

Interface map: SGNBMODREQ (sgnb_modification_request), RECONCOMPLETE (sgnb_reconfiguration_complete), SGNB_ADDITION_REQ (sgnb_addition_request)

name: mod_req_failure_distribution_pci
--------------------------------------

description: distribution of causes of a modification request failure

keys: physCellId, GNB_ID

timestamp: TS

- ULLONG TS
- STRING GNB_ID
- UINT physCellId
- FLOAT measurementInterval
- INT cnt
- UINT count_protobuf_unspecified
- UINT count_t310_Expiry
- UINT count_randomAccessProblem
- UINT count_rlc_MaxNumRetx
- UINT count_synchReconfigFailure_SCG
- UINT count_scg_reconfigFailure
- UINT count_srb3_IntegrityFailure

sources: base_mod_req_failure_distribution, gnb_ueid_cellid_map

transitive sources: SGNBMODREQ.sgnb_mod_req, SGNB_ADDITION_REQ_ACK.add_req_ack_cellid

Interface map: SGNBMODREQ (sgnb_modification_request), SGNB_ADDITION_REQ_ACK (sgnb_addition_request_acknowledge)

name: mod_status_refuse_cause
-----------------------------

description: distribution of causes for a sgnb modification refusal (base)

keys: GNB_ID

timestamp: TS

- STRING GNB_ID
- ULLONG TS
- FLOAT measurementInterval
- INT total_reconfig_refuse
- UINT count_radio_network
- UINT count_transport
- UINT count_protocol
- UINT count_misc

sources: mod_status_refuse_cause_base

transitive sources: SGNBMODREFUSE.sgnb_mod_refuse

Interface map: SGNBMODREFUSE (sgnb_modification_refuse)

name: mod_status_refuse_cause_pci
---------------------------------

description: distribution of causes for a sgnb modification refusal (base)

keys: physCellId, GNB_ID

timestamp: TS

- STRING GNB_ID
- ULLONG TS
- UINT physCellId
- FLOAT measurementInterval
- INT total_reconfig_refuse
- UINT count_radio_network
- UINT count_transport
- UINT count_protocol
- UINT count_misc

sources: mod_status_refuse_cause_base, gnb_ueid_cellid_map

transitive sources: SGNBMODREFUSE.sgnb_mod_refuse, SGNB_ADDITION_REQ_ACK.add_req_ack_cellid

Interface map: SGNB_ADDITION_REQ_ACK (sgnb_addition_request_acknowledge), SGNBMODREFUSE (sgnb_modification_refuse)

name: reconfig_all_debug
------------------------

timestamp: TS

- STRING name
- INT cnt
- ULLONG TS
- FLOAT measurementInterval

sources: reconfig_all

transitive sources: RECONCOMPLETE.reconfig_all

Interface map: RECONCOMPLETE (sgnb_reconfiguration_complete)

name: reconfig_reject_debug
---------------------------

timestamp: TS

- STRING name
- INT cnt
- ULLONG TS
- FLOAT measurementInterval

sources: reconfig_reject

transitive sources: RECONCOMPLETE.reconfig_reject

Interface map: RECONCOMPLETE (sgnb_reconfiguration_complete)

name: reconfig_status_reject_cause
----------------------------------

description: distribution of causes for DC rejection

keys: GNB_ID

timestamp: TB

- STRING GNB_ID
- ULLONG TB
- ULLONG TS
- FLOAT measurementInterval
- INT total_reconfig_reject
- UINT count_radio_network
- UINT count_transport
- UINT count_protocol
- UINT count_misc

sources: sgnb_add_req_reject

transitive sources: ADDREQREJECT.sgnb_add_req_reject

Interface map: ADDREQREJECT (sgnb_addition_request_reject)

name: reconfig_status_reject_cause_gtp_teid
-------------------------------------------

description: distribution of causes for DC rejection on a per-ue (gtp-teid) basis

keys: gTP_TEID, GNB_ID

timestamp: TB

- STRING GNB_ID
- STRING gTP_TEID
- ULLONG TB
- ULLONG TS
- FLOAT measurementInterval
- INT total_reconfig_reject
- UINT count_radio_network
- UINT count_transport
- UINT count_protocol
- UINT count_misc

sources: sgnb_add_req_reject, gnb_ueid_teid_map

transitive sources: ADDREQREJECT.sgnb_add_req_reject, SGNB_ADDITION_REQ.sgnb_addreq_gtp_teid, RECONCOMPLETE.reconfig_success

Interface map: RECONCOMPLETE (sgnb_reconfiguration_complete), ADDREQREJECT (sgnb_addition_request_reject), SGNB_ADDITION_REQ (sgnb_addition_request)

name: reconfig_status_reject_cause_pci
--------------------------------------

description: distribution of causes for DC rejection

keys: physCellId, GNB_ID

timestamp: TB

- STRING GNB_ID
- ULLONG TB
- ULLONG TS
- UINT physCellId
- FLOAT measurementInterval
- INT total_reconfig_reject
- UINT count_radio_network
- UINT count_transport
- UINT count_protocol
- UINT count_misc

sources: sgnb_add_req_reject, gnb_ueid_cellid_map

transitive sources: ADDREQREJECT.sgnb_add_req_reject, SGNB_ADDITION_REQ_ACK.add_req_ack_cellid

Interface map: ADDREQREJECT (sgnb_addition_request_reject), SGNB_ADDITION_REQ_ACK (sgnb_addition_request_acknowledge)

name: reconfig_status_success_rate
----------------------------------

description: fraction of DC connect requests which are successful

keys: GNB_ID

timestamp: TS

- STRING GNB_ID
- ULLONG TS
- FLOAT measurementInterval
- INT total_reconfiguration_requests
- UINT successful_reconfiguration_requests
- FLOAT success_rate
- FLOAT failure_rate

sources: reconfig_status_merge

transitive sources: SGNB_ADDITION_REQ.sgnb_addreq_for_ue, RECONCOMPLETE.reconfig_success

Interface map: RECONCOMPLETE (sgnb_reconfiguration_complete), SGNB_ADDITION_REQ (sgnb_addition_request)

name: reconfig_status_success_rate_gtp_teid
-------------------------------------------

description: fraction of DC connect requests which are successful, on a per-user (gtp_teid) basis.

keys: gTP_TEID, GNB_ID

timestamp: TS

- STRING GNB_ID
- STRING gTP_TEID
- ULLONG TS
- FLOAT measurementInterval
- INT total_reconfiguration_requests
- UINT successful_reconfiguration_requests
- FLOAT success_rate
- FLOAT failure_rate

sources: reconfig_status_merge, gnb_ueid_teid_map

transitive sources: SGNB_ADDITION_REQ.sgnb_addreq_for_ue, RECONCOMPLETE.reconfig_success, SGNB_ADDITION_REQ.sgnb_addreq_gtp_teid

Interface map: RECONCOMPLETE (sgnb_reconfiguration_complete), SGNB_ADDITION_REQ (sgnb_addition_request)

name: reconfig_status_success_rate_pci
--------------------------------------

description: fraction of DC connect requests which are successful, on a per-user (gtp_teid) basis.

keys: physCellId, GNB_ID

timestamp: TS

- STRING GNB_ID
- UINT physCellId
- ULLONG TS
- FLOAT measurementInterval
- INT total_reconfiguration_requests
- UINT successful_reconfiguration_requests
- FLOAT success_rate
- FLOAT failure_rate

sources: reconfig_status_merge, gnb_ueid_cellid_map

transitive sources: SGNB_ADDITION_REQ.sgnb_addreq_for_ue, RECONCOMPLETE.reconfig_success, SGNB_ADDITION_REQ_ACK.add_req_ack_cellid

Interface map: RECONCOMPLETE (sgnb_reconfiguration_complete), SGNB_ADDITION_REQ (sgnb_addition_request), SGNB_ADDITION_REQ_ACK (sgnb_addition_request_acknowledge)

name: reconfig_success_debug
----------------------------

timestamp: TS

- STRING name
- INT cnt
- ULLONG TS
- FLOAT measurementInterval

sources: reconfig_success

transitive sources: RECONCOMPLETE.reconfig_success

Interface map: RECONCOMPLETE (sgnb_reconfiguration_complete)

name: release_cause
-------------------

description: distribution of the causes of a DC release

keys: GNB_ID

timestamp: TS

- STRING GNB_ID
- ULLONG TS
- FLOAT measurementInterval
- INT total_reconfig_refuse
- UINT count_radio_network
- UINT count_transport
- UINT count_protocol
- UINT count_misc

sources: reconfig_cause_merge

transitive sources: RELREQ.release_req, SGNBRELEASERQD.SgNB_release_rqd

Interface map: RELREQ (sgnb_release_request), SGNBRELEASERQD (sgnb_release_required)

name: release_cause_gtp_ueid
----------------------------

description: distribution of the causes of a DC release by UE (gtp_teid)

keys: gTP_TEID, GNB_ID

timestamp: TS

- STRING GNB_ID
- STRING gTP_TEID
- ULLONG TS
- FLOAT measurementInterval
- INT total_reconfig_refuse
- UINT count_radio_network
- UINT count_transport
- UINT count_protocol
- UINT count_misc

sources: reconfig_cause_merge, gnb_ueid_teid_map

transitive sources: RELREQ.release_req, SGNBRELEASERQD.SgNB_release_rqd, SGNB_ADDITION_REQ.sgnb_addreq_gtp_teid, RECONCOMPLETE.reconfig_success

Interface map: RELREQ (sgnb_release_request), SGNBRELEASERQD (sgnb_release_required), SGNB_ADDITION_REQ (sgnb_addition_request), RECONCOMPLETE (sgnb_reconfiguration_complete)

name: release_req_success_stats
-------------------------------

description: statistics on the time to delease a DC connection

keys: GNB_ID

timestamp: TS

- STRING GNB_ID
- ULLONG TS
- FLOAT measurementInterval
- FLOAT min_success_time
- FLOAT max_success_time
- FLOAT avg_success_time
- FLOAT pctl_05_success_time
- FLOAT pctl_95_success_time
- FLOAT stddev_success_time

sources: release_req_start, release_req_success

transitive sources: RELREQ.release_req, CONRELEASE.dc_release

Interface map: RELREQ (sgnb_release_request), CONRELEASE (ue_context_release)

name: requests_per_gtp_teid
---------------------------

description: Number of sgnb addition requests requests per gTP_TEID

keys: gTP_TEID, GNB_ID

timestamp: TS

- ULLONG TS
- FLOAT measurementInterval
- STRING GNB_ID
- STRING gTP_TEID
- INT n_requests

sources: sgnb_addreq_gtp_teid

transitive sources: SGNB_ADDITION_REQ.sgnb_addreq_gtp_teid

Interface map: SGNB_ADDITION_REQ (sgnb_addition_request)

name: requests_per_gtp_teid_pci
-------------------------------

description: Number of sgnb addition requests requests per gTP_TEID

keys: gTP_TEID, physCellId, GNB_ID

timestamp: TS

- ULLONG TS
- FLOAT measurementInterval
- STRING GNB_ID
- STRING gTP_TEID
- UINT physCellId
- INT n_requests

sources: sgnb_addreq_gtp_teid, gnb_ueid_cellid_map

transitive sources: SGNB_ADDITION_REQ.sgnb_addreq_gtp_teid, SGNB_ADDITION_REQ_ACK.add_req_ack_cellid

Interface map: SGNB_ADDITION_REQ (sgnb_addition_request), SGNB_ADDITION_REQ_ACK (sgnb_addition_request_acknowledge)

name: rrcx_pdf_neighbor_beam_cell
---------------------------------

description: distribution of the beam ssb rsrp of neighboring cells, aggregated by gnb_id / cell id, computed from rrc transfer

keys: CELL_ID, GNB_ID

timestamp: TS

- ULLONG TS
- FLOAT measurementInterval
- UINT CELL_ID
- STRING GNB_ID
- INT cnt
- UINT rsrp_vbad
- UINT rsrp_bad
- UINT rsrp_medium
- UINT rsrp_good
- UINT rsrp_vgood

sources: neighbor_beam_ssb

transitive sources: RRCXFER.neighbor_beam_ssb

Interface map: RRCXFER (rrctransfer)

name: rrcx_pdf_neighbor_beam_gnb
--------------------------------

description: distribution of the beam ssb rsrp of neighboring cells, aggregated by gNB, computed from rrc transfer

keys: GNB_ID

timestamp: TS

- ULLONG TS
- FLOAT measurementInterval
- STRING GNB_ID
- INT cnt
- UINT rsrp_vbad
- UINT rsrp_bad
- UINT rsrp_medium
- UINT rsrp_good
- UINT rsrp_vgood

sources: neighbor_beam_ssb

transitive sources: RRCXFER.neighbor_beam_ssb

Interface map: RRCXFER (rrctransfer)

name: rrcx_pdf_neighbor_beam_gtp_teid
-------------------------------------

description: distribution of the beam ssb rsrp of neighboring cells aggregated by ue (gtp_teid), computed from rrc transfer

keys: gTP_TEID, GNB_ID

timestamp: TS

- ULLONG TS
- FLOAT measurementInterval
- STRING GNB_ID
- STRING gTP_TEID
- INT cnt
- UINT rsrp_vbad
- UINT rsrp_bad
- UINT rsrp_medium
- UINT rsrp_good
- UINT rsrp_vgood

sources: neighbor_beam_ssb, gnb_ueid_teid_map

transitive sources: RRCXFER.neighbor_beam_ssb, SGNB_ADDITION_REQ.sgnb_addreq_gtp_teid, RECONCOMPLETE.reconfig_success

Interface map: RECONCOMPLETE (sgnb_reconfiguration_complete), SGNB_ADDITION_REQ (sgnb_addition_request), RRCXFER (rrctransfer)

name: rrcx_pdf_neighbor_cell
----------------------------

description: distribution of the  ssb rsrp of the neighboring cells by cell id, computed from rrc transfer

keys: CELL_ID, GNB_ID

timestamp: TS

- ULLONG TS
- FLOAT measurementInterval
- UINT CELL_ID
- STRING GNB_ID
- INT cnt
- UINT rsrp_vbad
- UINT rsrp_bad
- UINT rsrp_medium
- UINT rsrp_good
- UINT rsrp_vgood

sources: nr_neighbor

transitive sources: RRCXFER.nr_neighbor

Interface map: RRCXFER (rrctransfer)

name: rrcx_pdf_neighbor_gnb
---------------------------

description: distribution of the  ssb rsrp of neighbor cells aggregated by gNB, computed from rrc transfer

keys: GNB_ID

timestamp: TS

- ULLONG TS
- FLOAT measurementInterval
- STRING GNB_ID
- INT cnt
- UINT rsrp_vbad
- UINT rsrp_bad
- UINT rsrp_medium
- UINT rsrp_good
- UINT rsrp_vgood

sources: nr_neighbor

transitive sources: RRCXFER.nr_neighbor

Interface map: RRCXFER (rrctransfer)

name: rrcx_pdf_neighbor_gtp_teid
--------------------------------

description: distribution of the  ssb rsrp of neighbor cells aggregated by ue (gtp_teid), computed from rrc transfer

keys: gTP_TEID, GNB_ID

timestamp: TS

- ULLONG TS
- FLOAT measurementInterval
- STRING GNB_ID
- STRING gTP_TEID
- INT cnt
- UINT rsrp_vbad
- UINT rsrp_bad
- UINT rsrp_medium
- UINT rsrp_good
- UINT rsrp_vgood

sources: nr_neighbor, gnb_ueid_teid_map

transitive sources: RRCXFER.nr_neighbor, SGNB_ADDITION_REQ.sgnb_addreq_gtp_teid, RECONCOMPLETE.reconfig_success

Interface map: RECONCOMPLETE (sgnb_reconfiguration_complete), SGNB_ADDITION_REQ (sgnb_addition_request), RRCXFER (rrctransfer)

name: rrcx_pdf_serv_beam_cell
-----------------------------

description: distribution of the beam ssb rsrp of serving cells, aggregated by gnb_id / cell id, computed from rrc transfer

keys: CELL_ID, GNB_ID

timestamp: TS

- ULLONG TS
- FLOAT measurementInterval
- UINT CELL_ID
- STRING GNB_ID
- INT cnt
- UINT rsrp_vbad
- UINT rsrp_bad
- UINT rsrp_medium
- UINT rsrp_good
- UINT rsrp_vgood

sources: serv_cell_beam_ssb

transitive sources: RRCXFER.serv_cell_beam_ssb

Interface map: RRCXFER (rrctransfer)

name: rrcx_pdf_serv_beam_gnb
----------------------------

description: distribution of the beam ssb rsrp of serving cells, aggregated by gnb_id, computed from rrc transfer

keys: GNB_ID

timestamp: TS

- ULLONG TS
- FLOAT measurementInterval
- STRING GNB_ID
- INT cnt
- UINT rsrp_vbad
- UINT rsrp_bad
- UINT rsrp_medium
- UINT rsrp_good
- UINT rsrp_vgood

sources: serv_cell_beam_ssb

transitive sources: RRCXFER.serv_cell_beam_ssb

Interface map: RRCXFER (rrctransfer)

name: rrcx_pdf_serv_beam_gtp_teid
---------------------------------

description: distribution of the  ssb beam rsrp of serving cells aggregated by ue (gtp_teid), computed from rrc transfer

keys: gTP_TEID, GNB_ID

timestamp: TS

- ULLONG TS
- FLOAT measurementInterval
- STRING GNB_ID
- STRING gTP_TEID
- INT cnt
- UINT rsrp_vbad
- UINT rsrp_bad
- UINT rsrp_medium
- UINT rsrp_good
- UINT rsrp_vgood

sources: serv_cell_beam_ssb, gnb_ueid_teid_map

transitive sources: RRCXFER.serv_cell_beam_ssb, SGNB_ADDITION_REQ.sgnb_addreq_gtp_teid, RECONCOMPLETE.reconfig_success

Interface map: RECONCOMPLETE (sgnb_reconfiguration_complete), SGNB_ADDITION_REQ (sgnb_addition_request), RRCXFER (rrctransfer)

name: rrcx_pdf_serv_cell
------------------------

description: distribution of the  ssb rsrp of serving cell aggregated by cell id, computed from rrc transfer

keys: CELL_ID, GNB_ID

timestamp: TS

- ULLONG TS
- FLOAT measurementInterval
- UINT CELL_ID
- STRING GNB_ID
- INT cnt
- UINT rsrp_vbad
- UINT rsrp_bad
- UINT rsrp_medium
- UINT rsrp_good
- UINT rsrp_vgood

sources: serv_nr_cell

transitive sources: RRCXFER.serv_nr_cell

Interface map: RRCXFER (rrctransfer)

name: rrcx_pdf_serv_gnb
-----------------------

description: distribution of the  ssb rsrp of serving cells aggregated by gnb id, computed from rrc transfer

keys: GNB_ID

timestamp: TS

- ULLONG TS
- FLOAT measurementInterval
- STRING GNB_ID
- INT cnt
- UINT rsrp_vbad
- UINT rsrp_bad
- UINT rsrp_medium
- UINT rsrp_good
- UINT rsrp_vgood

sources: serv_nr_cell

transitive sources: RRCXFER.serv_nr_cell

Interface map: RRCXFER (rrctransfer)

name: rrcx_pdf_serv_gtp_teid
----------------------------

description: distribution of the  ssb rsrp of serving cells aggregated by ue (gtp_teid), computed from rrc transfer

keys: gTP_TEID, GNB_ID

timestamp: TS

- ULLONG TS
- FLOAT measurementInterval
- STRING GNB_ID
- STRING gTP_TEID
- INT cnt
- UINT rsrp_vbad
- UINT rsrp_bad
- UINT rsrp_medium
- UINT rsrp_good
- UINT rsrp_vgood

sources: serv_nr_cell, gnb_ueid_teid_map

transitive sources: RRCXFER.serv_nr_cell, SGNB_ADDITION_REQ.sgnb_addreq_gtp_teid, RECONCOMPLETE.reconfig_success

Interface map: RECONCOMPLETE (sgnb_reconfiguration_complete), SGNB_ADDITION_REQ (sgnb_addition_request), RRCXFER (rrctransfer)

name: rrcx_stats_neighbor_beam_cell
-----------------------------------

description: statistics on ssb RSRP on the beams of neighboring cells, aggregated by gbn_id / cell ID, computed using rrc transfer

keys: CELL_ID, GNB_ID

timestamp: TS

- ULLONG TS
- FLOAT measurementInterval
- UINT CELL_ID
- STRING GNB_ID
- INT cnt
- INT min_rsrp
- INT pctl_05_rsrp
- INT median_rsrp
- INT pctl_95_rsrp
- FLOAT stddev_rsrp
- INT max_rsrp

sources: neighbor_beam_ssb

transitive sources: RRCXFER.neighbor_beam_ssb

Interface map: RRCXFER (rrctransfer)

name: rrcx_stats_neighbor_beam_gnb
----------------------------------

description: statistics on ssb RSRP on the beams of nrighboring cells, aggregated by gNB, computed using rrc transfer

keys: GNB_ID

timestamp: TS

- ULLONG TS
- FLOAT measurementInterval
- STRING GNB_ID
- INT cnt
- INT min_rsrp
- INT pctl_05_rsrp
- INT median_rsrp
- INT pctl_95_rsrp
- FLOAT stddev_rsrp
- INT max_rsrp

sources: neighbor_beam_ssb

transitive sources: RRCXFER.neighbor_beam_ssb

Interface map: RRCXFER (rrctransfer)

name: rrcx_stats_neighbor_cell
------------------------------

description: statistics on the ssb rsrp of the neighbor cells, aggregated by gnb_id / cell id, computed using rrc transfer

keys: CELL_ID, GNB_ID

timestamp: TS

- ULLONG TS
- FLOAT measurementInterval
- UINT CELL_ID
- STRING GNB_ID
- INT cnt
- INT min_rsrp
- INT pctl_05_rsrp
- INT median_rsrp
- INT pctl_95_rsrp
- FLOAT stddev_rsrp
- INT max_rsrp

sources: nr_neighbor

transitive sources: RRCXFER.nr_neighbor

Interface map: RRCXFER (rrctransfer)

name: rrcx_stats_neighbor_gnb
-----------------------------

description: statistics on the ssb rsrp of the neighbor cells, aggregated by gNB, computed using rrc transfer

keys: GNB_ID

timestamp: TS

- ULLONG TS
- FLOAT measurementInterval
- STRING GNB_ID
- INT cnt
- INT min_rsrp
- INT pctl_05_rsrp
- INT median_rsrp
- INT pctl_95_rsrp
- FLOAT stddev_rsrp
- INT max_rsrp

sources: nr_neighbor

transitive sources: RRCXFER.nr_neighbor

Interface map: RRCXFER (rrctransfer)

name: rrcx_stats_serv_beam_cell
-------------------------------

description: statistics on ssb RSRP on the beams of serving cells, aggregated by gbn_id / cell ID, computed using rrc transfer

keys: CELL_ID, GNB_ID

timestamp: TS

- ULLONG TS
- FLOAT measurementInterval
- UINT CELL_ID
- STRING GNB_ID
- INT cnt
- INT min_rsrp
- INT pctl_05_rsrp
- INT median_rsrp
- INT pctl_95_rsrp
- FLOAT stddev_rsrp
- INT max_rsrp

sources: serv_cell_beam_ssb

transitive sources: RRCXFER.serv_cell_beam_ssb

Interface map: RRCXFER (rrctransfer)

name: rrcx_stats_serv_beam_gnb
------------------------------

description: statistics on ssb RSRP on the beams of serving cells, aggregated by gbn_id / cell ID, computed using rrc transfer

keys: GNB_ID

timestamp: TS

- ULLONG TS
- FLOAT measurementInterval
- STRING GNB_ID
- INT cnt
- INT min_rsrp
- INT pctl_05_rsrp
- INT median_rsrp
- INT pctl_95_rsrp
- FLOAT stddev_rsrp
- INT max_rsrp

sources: serv_cell_beam_ssb

transitive sources: RRCXFER.serv_cell_beam_ssb

Interface map: RRCXFER (rrctransfer)

name: rrcx_stats_serv_cell
--------------------------

description: statistics on the ssb rsrp of the serving cell, aggregated by gnb_id / cell id, computed using rrc transfer

keys: CELL_ID, GNB_ID

timestamp: TS

- ULLONG TS
- FLOAT measurementInterval
- UINT CELL_ID
- STRING GNB_ID
- INT cnt
- INT min_rsrp
- INT pctl_05_rsrp
- INT median_rsrp
- INT pctl_95_rsrp
- FLOAT stddev_rsrp
- INT max_rsrp

sources: serv_nr_cell

transitive sources: RRCXFER.serv_nr_cell

Interface map: RRCXFER (rrctransfer)

name: rrcx_stats_serv_gnb
-------------------------

description: statistics on the ssb srp of the serving cell, aggregated by gNB, computed using rrc transfer

keys: GNB_ID

timestamp: TS

- ULLONG TS
- FLOAT measurementInterval
- STRING GNB_ID
- INT cnt
- INT min_rsrp
- INT pctl_05_rsrp
- INT median_rsrp
- INT pctl_95_rsrp
- FLOAT stddev_rsrp
- INT max_rsrp

sources: serv_nr_cell

transitive sources: RRCXFER.serv_nr_cell

Interface map: RRCXFER (rrctransfer)

name: throughput_gnb
--------------------

description: throughput experienced by a GNB over a measurement interval.   *Active* throughput is throughput while actively downloading, *average* averages bytes transfered over the measurement interval

keys: GNB_ID, e_RAB_ID

timestamp: TS

- ULLONG TS
- LLONG e_RAB_ID
- STRING GNB_ID
- FLOAT measurementInterval
- LLONG active_throughput
- LLONG average_throughput
- LLONG min_throughput
- LLONG max_throughput
- UINT active_throughput_percentile_05
- UINT active_throughput_percentile_95

sources: rat_data_usage

transitive sources: RATDATAUSAGE.rat_data_usage

Interface map: RATDATAUSAGE (secondary_rat_data_usage_report)

name: throughput_gtp_teid
-------------------------

description: throughput experienced by UE, as determined by the gtp_teid, over a measurement interval.   *Active* throughput is throughput while actively downloading, *average* averages bytes transfered over the measurement interval

keys: gTP_TEID, GNB_ID

timestamp: TS

- ULLONG TS
- STRING gTP_TEID
- STRING GNB_ID
- FLOAT measurementInterval
- LLONG active_throughput
- LLONG average_throughput
- LLONG min_throughput
- LLONG max_throughput

sources: throughput_session_gtp_teid_join

transitive sources: RATDATAUSAGE.rat_data_usage, SGNB_ADDITION_REQ.sgnb_addreq_gtp_teid, RECONCOMPLETE.reconfig_success

Interface map: RECONCOMPLETE (sgnb_reconfiguration_complete), RATDATAUSAGE (secondary_rat_data_usage_report), SGNB_ADDITION_REQ (sgnb_addition_request)

name: throughput_gtp_teid_bearer
--------------------------------

description: throughput experienced by UE, as determined by the gtp_teid, for a bearer (eRAB_ID) over a measurement interval.   *Active* throughput is throughput while actively downloading, *average* averages bytes transfered over the measurement interval

keys: gTP_TEID, GNB_ID, e_RAB_ID

timestamp: TS

- ULLONG TS
- STRING gTP_TEID
- STRING GNB_ID
- LLONG e_RAB_ID
- FLOAT measurementInterval
- LLONG active_throughput
- LLONG average_throughput
- LLONG min_throughput
- LLONG max_throughput

sources: throughput_session_gtp_teid_join

transitive sources: RATDATAUSAGE.rat_data_usage, SGNB_ADDITION_REQ.sgnb_addreq_gtp_teid, RECONCOMPLETE.reconfig_success

Interface map: RECONCOMPLETE (sgnb_reconfiguration_complete), RATDATAUSAGE (secondary_rat_data_usage_report), SGNB_ADDITION_REQ (sgnb_addition_request)

name: throughput_meas_alt_crnti
-------------------------------

description: throughput experienced by a UE (alternative version)

keys: eutran_trace_id, eci

timestamp: TS

- ULLONG TS
- UINT eci
- ULLONG eutran_trace_id
- FLOAT ue_lte_tput

sources: lte_thpt_meas

transitive sources: LTE_PCMD.lte_thpt_meas

name: throughput_meas_crnti
---------------------------

description: throughput experienced by a UE

keys: eutran_trace_id, eci

timestamp: TS

- ULLONG TS
- UINT eci
- ULLONG eutran_trace_id
- FLOAT ue_lte_tput

sources: lte_thpt_meas

transitive sources: LTE_PCMD.lte_thpt_meas

name: throughput_pci
--------------------

description: throughput experienced by UE, as determined by the gtp_teid, over a measurement interval.   *Active* throughput is throughput while actively downloading, *average* averages bytes transfered over the measurement interval

keys: physCellId, GNB_ID

timestamp: TS

- ULLONG TS
- UINT physCellId
- STRING GNB_ID
- FLOAT measurementInterval
- LLONG active_throughput
- LLONG average_throughput
- LLONG min_throughput
- LLONG max_throughput

sources: prelim_throughput_gtp_teid, gnb_ueid_cellid_map

transitive sources: RATDATAUSAGE.rat_data_usage, SGNB_ADDITION_REQ_ACK.add_req_ack_cellid

Interface map: RATDATAUSAGE (secondary_rat_data_usage_report), SGNB_ADDITION_REQ_ACK (sgnb_addition_request_acknowledge)

name: throughput_rollup
-----------------------

description: statistics on the per-UE throughput

keys: GNB_ID, e_RAB_ID

timestamp: TS

- ULLONG TS
- LLONG e_RAB_ID
- STRING GNB_ID
- FLOAT measurementInterval
- INT count_ues
- UINT average_throughput_percentile_05
- UINT average_throughput_percentile_50
- FLOAT average_average_throughput
- UINT average_throughput_percentile_95
- UINT active_throughput_percentile_05
- UINT active_throughput_percentile_50
- FLOAT average_active_throughput
- UINT active_throughput_percentile_95

sources: throughput_session

transitive sources: RATDATAUSAGE.rat_data_usage

Interface map: RATDATAUSAGE (secondary_rat_data_usage_report)

name: throughput_session
------------------------

description: throughput experienced by UE session over a measurement interval.   *Active* throughput is throughput while actively downloading, *average* averages bytes transfered over the measurement interval

keys: UE_ID, GNB_ID, e_RAB_ID

timestamp: TS

- ULLONG TS
- LLONG e_RAB_ID
- LLONG UE_ID
- STRING GNB_ID
- FLOAT measurementInterval
- LLONG active_throughput
- LLONG average_throughput
- LLONG min_throughput
- LLONG max_throughput

sources: rat_data_usage

transitive sources: RATDATAUSAGE.rat_data_usage

Interface map: RATDATAUSAGE (secondary_rat_data_usage_report)

name: throughput_userclass
--------------------------

description: throughput experienced by UE, rolled up into user classes, over a measurement interval.  Class A (qci=9, arp=15) is class=1 and Class B  (qci=8, arp=15) is class=2.   *Active* throughput is throughput while actively downloading, *average* averages bytes transfered over the measurement interval

keys: GNB_ID, CLASS

timestamp: TS

- ULLONG TS
- STRING GNB_ID
- UINT CLASS
- FLOAT measurementInterval
- LLONG active_throughput
- LLONG average_throughput
- LLONG min_throughput
- LLONG max_throughput

sources: throughput_session_userclass_join

transitive sources: RATDATAUSAGE.rat_data_usage, SGNB_ADDITION_REQ.sgnb_addreq_for_ue_bearers, SGNB_ADDITION_REQ_ACK.eRABs_acked_for_admit_for_ue

Interface map: RATDATAUSAGE (secondary_rat_data_usage_report), SGNB_ADDITION_REQ (sgnb_addition_request), SGNB_ADDITION_REQ_ACK (sgnb_addition_request_acknowledge)

name: throughput_userclass_pci
------------------------------

description: throughput experienced by UE, rolled up into user classes, over a measurement interval.  Class A (qci=9, arp=15) is class=1 and Class B  (qci=8, arp=15) is class=2.   *Active* throughput is throughput while actively downloading, *average* averages bytes transfered over the measurement interval

keys: ARP, physCellId, GNB_ID, qCI

timestamp: TS

- ULLONG TS
- STRING GNB_ID
- UINT physCellId
- LLONG qCI
- LLONG ARP
- FLOAT measurementInterval
- LLONG active_throughput
- LLONG average_throughput
- LLONG min_throughput
- LLONG max_throughput

sources: throughput_session_userclass_join, gnb_ueid_cellid_map

transitive sources: RATDATAUSAGE.rat_data_usage, SGNB_ADDITION_REQ.sgnb_addreq_for_ue_bearers, SGNB_ADDITION_REQ_ACK.eRABs_acked_for_admit_for_ue, SGNB_ADDITION_REQ_ACK.add_req_ack_cellid

Interface map: RATDATAUSAGE (secondary_rat_data_usage_report), SGNB_ADDITION_REQ (sgnb_addition_request), SGNB_ADDITION_REQ_ACK (sgnb_addition_request_acknowledge)

name: ue_drb_count
------------------

description: ue drb count

keys: eutran_trace_id, eci

timestamp: TS

- ULLONG TS
- UINT eci
- ULLONG eutran_trace_id
- INT ue_drb_count

sources: drb_pdcp_pdu_stats

transitive sources: LTE_PCMD.lte_rb_thpt





