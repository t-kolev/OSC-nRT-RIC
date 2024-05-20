#!/usr/bin/env bash
# vim: ts=4 sw=4 noet:
#--------------------------------------------------------------------------------
#	Copyright (c) 2018-2019 AT&T Intellectual Property.
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#	   http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.
#--------------------------------------------------------------------------------



# ---------------------------------------------------------------------------
#	Mnemonic:	bld_dev_env.sh
#	Abstract:	This script is meant to be executed by a RUN command in
#				a docker file. It fetches NNG, builds/installs it, and then
#				does a wget of the desired RMR version's packages and installs
#				them. RMR is fetched from package cloud.
#
#	Date:		22 August 2019
#	Author:		E. Scott Daniels
# ---------------------------------------------------------------------------

rmr_ver=1.9.0
nng_ver=v1.1.1

while [[ $1 == -* ]]
do
	case $1 in 
		-n) nng_ver=$2; shift;;
		-r)	rmr_ver=$2; shift;;
	esac

	shift
done

if [[ $nng_ver != "v"* ]]
then
	nng_ver="v$nng_ver"
fi

set -e 							# from this point on crash on any error
mkdir -p /playpen/build/nng
cd /playpen/build/nng
git clone https://github.com/nanomsg/nng.git
cd nng
git checkout $nng_ver
mkdir .build
cd .build
echo "building nng (messages supressed unless there is an error)"

set +e
if ! cmake ..  >/tmp/cmake.nng.log 2>&1
then
	cat /tmp/cmake.nng.log
	echo ""
	echo "### ERROR ### NNG cmake configuration failed"
	exit 1
fi

if ! make install >/tmp/build.nng.log 2>&1
then
	cat /tmp/build.nng.log
	echo ""
	echo "### ERROR ### NNG build failed"
	exit 1
fi

set -e
echo "nng build finished ok"
cd /playpen
rm -fr /playpen/build/nng

echo "installing RMR packages version = $rmr_ver"
mkdir -p /playpen/build/pkgs
cd /playpen/build/pkgs

base_url=https://packagecloud.io/o-ran-sc/master/packages/debian/stretch/
base_url=https://packagecloud.io/o-ran-sc/staging/packages/debian/stretch/
pc_url=${base_url}rmr_${rmr_ver}_amd64.deb/download.deb
pc_dev_url=${base_url}rmr-dev_${rmr_ver}_amd64.deb/download.deb

wget -q -O rmr.deb $pc_url
wget -q -O rmr-dev.deb $pc_dev_url

ls -al
dpkg -i *.deb
cd /playpen
rm -fr /playpen/build/pkgs
echo "RMR package install finished"
