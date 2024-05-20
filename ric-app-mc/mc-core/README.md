----------------------------------
MC xApp
5G EN-DC Measurement and KPI Computation xApp
Developed by Rajarajan Sivaraj, Theodore Johnson, Vladislav Shkapenyuk
Additional Credits: Matti Hiltunen, Gueyoung Jung, Scott Daniels
AT&T Labs Research
----------------------------------

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

