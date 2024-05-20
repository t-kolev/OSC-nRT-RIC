.. This work is licensed under a Creative Commons Attribution 4.0 International License.
.. SPDX-License-Identifier: CC-BY-4.0
.. Copyright (C) 2019 AT&T


RIC Measurement Campaign (MC) Overview
=======================================

MC xApp
5G EN-DC Measurement and KPI Computation xApp
Developed by Rajarajan Sivaraj, Theodore Johnson, Vladislav Shkapenyuk
Additional Credits: Matti Hiltunen, Gueyoung Jung, Scott Daniels
AT&T Labs Research
---------------------------------

MC xApp calculates a number of metrics and KPIs based on X2 messages formatted as protobufs as defined in ric-plt/streaming-protobufs.

In the MC xApp, we compute 5G KPIs at customized fine-grained granularities in the following manner:
- On a per-UE per-bearer basis
- On a per-UE basis
- On a UE-group basis
- On an NR cell-wide basis
- On an NR gNB-wide basis

We identify individual UEs across multiple EN-DC sessions by the s1-UL-GTPtunnelEndpoint IE in 3GPP X2AP: SgNB Addition Request message, as long as they remain connected to the same eNB.
We identify UE groups based on (i) QCI, (ii) E-RAB-ID, (iii) (QCI, ARP) value pair, etc.
We identify NR cell based on the NR-PCI or the Physical Cell Identity
We identify the NR gNB based on the gNB ID Information Element (IE) received in the RMR X2AP streaming header

The key 3GPP EN-DC X2AP procedures that we handle in the MC xApp include:
- SgNB Addition
- SgNB Release
- SgNB Modification
- RRC Transfer
- Secondary RAT Data Usage Report

The metrics are reported periodically as VES events and include metrics such as number of dual connected UEs, duration of dual connections, and signal strength metrics. For a full list of supported metrics see the readme.

Internally, MC xApp uses the gs-lite stream processing engine (com/gs-lite).

The protobufs are received as RMR messages from UEEC but if no such data producer is available, MC xApp can be run in a simulated mode where the protobuf content is generated locally by the UEEC simulator running in the same container.

