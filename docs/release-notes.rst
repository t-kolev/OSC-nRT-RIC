.. This work is licensed under a Creative Commons Attribution 4.0 International License.
.. SPDX-License-Identifier: CC-BY-4.0
.. Copyright (C) 2019 AT&T


Release Notes
=============


This document provides the release notes for the Dawn Release of the Measurement Campaign xAPP.

.. contents::
   :depth: 3
   :local:


Version history
---------------

+--------------------+--------------------+--------------------+--------------------+
| **Date**           | **Ver.**           | **Author**         | **Comment**        |
|                    |                    |                    |                    |
+--------------------+--------------------+--------------------+--------------------+
| 2021-06-24         | 1.0.11             |   Vlad Shkapenyuk  | Dawn release       |
|                    |                    |                    |                    |
+--------------------+--------------------+--------------------+--------------------+
| 2020-05-01         | 1.0.5              |   Vlad Shkapenyuk  | Bronze release     |
|                    |                    |                    |                    |
+--------------------+--------------------+--------------------+--------------------+
| 2019-11-25         | 1.0.0              |   Vlad Shkapenyuk  | First draft        |
|                    |                    |                    |                    |
+--------------------+--------------------+--------------------+--------------------+



Summary
-------

The Dawn release of the MC xAPP supports calculation of a number of metrics and KPIs 
based on X2 messages received from UEEC.


Release Data
------------

+--------------------------------------+--------------------------------------+
| **Project**                          | RAN Intelligent Controller           |
|                                      |                                      |
+--------------------------------------+--------------------------------------+
| **Repo/commit-ID**                   | ric-app/mc                           |
|                                      |                                      |
+--------------------------------------+--------------------------------------+
| **Release designation**              | Dawn                                 |
|                                      |                                      |
+--------------------------------------+--------------------------------------+
| **Release date**                     | 2021-06-24                           |
|                                      |                                      |
+--------------------------------------+--------------------------------------+
| **Purpose of the delivery**          | open-source measurement campaign xApp|
|                                      |                                      |
|                                      |                                      |
+--------------------------------------+--------------------------------------+

Components
----------

- mc-core/* contains the source of core MC xApp container.
  + *mc-core/mc/queries/* contains a set of queries computing the output metrics and KPIs.
  + *mc-core/mc/data_gen/* contains a generator of X2 messages to run MC in standalone simulation mode without UEEC.
  
- *sidecars/* contains the source code the RMR listener container responsible for listeting to UEEC messages and forwarding them to mc-core.

- *docs/* contains the documentation.
  

Dependencies
------------
- MC xApp relies the GS-lite stream processing engine (com/gs-lite).
