..
..  Copyright (c) 2019 AT&T Intellectual Property.
..  Copyright (c) 2019-2022 Nokia.
..
..  Licensed under the Creative Commons Attribution 4.0 International
..  Public License (the "License"); you may not use this file except
..  in compliance with the License. You may obtain a copy of the License at
..
..    https://creativecommons.org/licenses/by/4.0/
..
..  Unless required by applicable law or agreed to in writing, documentation
..  distributed under the License is distributed on an "AS IS" BASIS,
..  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
..
..  See the License for the specific language governing permissions and
..  limitations under the License.
..

Release-Notes
=============

This document provides the release notes of the dbaas.

.. contents::
   :depth: 3
   :local:



Version history
---------------

[0.6.4] - 2023-12-13

* Bump to Redis 6.2.14 

[0.6.3] - 2023-05-29

* Bump to Redis 6.2.12 to fix Local Denial of Service Vulnerability

[0.6.2] - 2022-07-14

* Bump to Redis 6.2.7 to fix security vulnerabilities reported in Redis base image 6.2.6

[0.6.1] - 2022-03-21

* Upgrade v0.10.0 SDLGO tag for sdlcli and make DBAAS 0.6.1 tag

[0.6.0] - 2022-01-20

* Bump Redis base image to 6.2.6, because version 5 is going soon out of maintenance

[0.5.7] - 2021-12-30

* Build a new DBAAS Docker image to have the latest version (0.9.3) of the sdlcli -tool

[0.5.6] - 2021-12-28

* Build a new DBAAS Docker image to have the latest version (0.9.2) of the sdlcli -tool

[0.5.5] - 2021-12-16

* Build a new DBAAS Docker image to take the latest version (0.9.0) of the sdlcli -tool in

[0.5.4] - 2021-10-26

* Add sdlcli -tool for troubleshooting SDL and Database problems

[0.5.3] - 2021-08-26

* Upgrade apk-tools to fix security vulnerabilities CVE-2021-36159 and CVE-2021-30139.

[0.5.2] - 2021-06-04

* Upgrade packages to fix possible security vulnerability reported in Alpine base image.

[0.5.1] - 2021-02-25

* Upgrade SSL version to 1.1.1j-r0 to fix possible SSL security vulnerability.
* Fix DBAAS testing application compile issue.

[0.5.0] - 2020-12-11

* Upgrade DBAAS container's base Redis image to redis:5.0.9-alpine3.11.
* Upgrade Redis module's base builder image to bldr-alpine3-go:2.0.0.
* Upgrade DBAAS unit test's base builder image to ubuntu:18.04.

[0.4.1] - 2020-06-17

* Upgrade base image to bldr-alpine3:12-a3.11 in Redis docker build

[0.4.0] - 2020-04-23

* Bump version to 0.4.0 to follow RIC versioning rules (4 is meaning RIC release R4). No functional changes.

[0.3.2] - 2020-04-22

* Upgrade base image to bldr-alpine3:10-a3.22-rmr3 in Redis docker build
* Fix redismodule resource leak

[0.3.1] - 2020-02-13

* Upgrade base image to alpine3-go:1-rmr1.13.1 in Redis docker build

[0.3.0] - 2020-01-23

* Enable unit tests and valgrind in CI.
* Update redismodule with new commands.
* Update documentation.

[0.2.2] - 2019-11-12

* Take Alpine (version 6-a3.9) linux base image into use in Redis docker image.
* Add mandatory documentation files.

[0.2.1] - 2019-09-17

* Add the curl tool to docker image to facilitate trouble-shooting.

[0.2.0] - 2019-09-03

* Take Redis 5.0 in use.

[0.1.0] - 2019-06-17

* Initial Implementation to provide all the needed elements to deploy database
  as a service docker image to kubernetes.
* Introduce new Redis modules: SETIE, SETNE, DELIE, DELNE, MSETPUB, MSETMPUB,
  SETXXPUB, SETNXPUB, SETIEPUB, SETNEPUB, DELPUB, DELMPUB, DELIEPUB, DELNEPUB,
  NGET, NDEL.
