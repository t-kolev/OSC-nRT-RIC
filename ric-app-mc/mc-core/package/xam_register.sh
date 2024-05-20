# vim: noet ts=4 :
#----------------------------------------------------------------------------------
#
#   Copyright (c) 2021 AT&T Intellectual Property.
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.
#
#---------------------------------------------------------------------------------

# --------------------------------------------------------------------------------------------------------------
#	Abstract:	This will send an application registration message to
#				the xAPP manager.
#
#				A note about the field descriptions given to j2src programme:
#					They define what we need from the config file. Input to j2src is
#
#						field:[field:...]:{*|var-name}
#
#					fields are considered to be an "object" if not the last. The last field may be
#					of  the form  name[]match=value@pull-name
#					where:
#						the named field is an array of objects and "match" is a field in each
#						object. Each element is tested to see if the match field's value is "value"
#						and when true, the "pull-name" field's value is used.  The description is
#						used in our list to pull the rmr-data port from the messageing section.
#
#				Notes about URLs and DNS names:
#					According to those in the know, the xAPP manager being a platform component will have
#					a fixed DNS name, and thus we expect that.
#
#					In the less than obvious world of kubernetes we cannot depend on hostname actually being
#					the DNS name for our stuff. So, we assume that if the DN name is NOT the host name, the
#					variable will be set to the DNS name as RMR also depends on this for things.
#					If it's not set, we'll default to hostname.
#
#	Date:		28 January 2021
#	Author:		E. Scott Daniels
# --------------------------------------------------------------------------------------------------------------

# quote the two parms passed in making json style output quoting less cumbersome inline.
function quote {
	echo "\"$1\": \"$2\""
}

# ------------------------------------------------------------------------------------------------
xam_url="http://service-ricplt-appmgr-http:8080/ric/v1"		# where we expect xAPP mgr to be waiting
svc_name="${RMR_SRC_ID:-$( hostname )}"						# the DNS name of this "host" for RMR

if [[ -d /playpen/bin ]]		# where python things live since python doesn't use PATH :(
then
	PATH=/playpen/bin:$PATH
	bin_dir=/playpen/bin
else
	PATH=$PATH:.
	bin_dir="."
fi

# suss out the descriptor file. The env name _should_ be a direct pointer
# to the file, but it might be just a pointer to the directory :(
#
df="${XAPP_DESCRIPTOR_PATH:-/opt/ric/config/config-file.json}"      # default is where helm/kubernetes seems to deposit it


if [[ -d $df ]]														# we got a bloody directory... must play hide and go seek
then
	echo "[INFO] df ($df) is a directory, sussing out a config file from its depths" >&2
	if [[ -e $df/config-file.json ]]
	then
		echo "[INFO] df found: $df/config-file.json" >&2
		df=$df/config-file.json
	else
		cf=$( ls $df/*.json|head -1 )
		if [[ -z $cf ]]
		then
			echo "[FAIL] no json file found in $df" >&2
			exit 1
		fi
		df=$df/$cf
		echo "[INFO] df is  a directory, using: $df" >&2
	fi
fi
echo "[INFO] descriptor file: $df" >&2

sf=/tmp/PID$$.cfg			# where shell config dump goes
touch $sf					# must exist
if [[ -s $df ]]				# pull config stuff into src file; python can't handle a nil/empty file :(
then
	echo "[INFO] xam_register: parsing descriptor: $df" >&2

											# CAUTION: these must be concatinated with a trailing space!
	config_fields="xapp_name:* "			# create space sep list of fields we need
	config_fields+="version:* "
	config_fields+="controls:app_man_url:xam_url "	# xapp manager url for registration if there
	config_fields+="messaging:ports[]name=rmr-data@port:*"

	python3 $bin_dir/j2src.py debug $df $config_fields >$sf
else
	echo "[WARN] descriptor file isn't there or is empty: $df" >&2
fi

echo "[INFO] sourcing info from config" >&2
cat $sf
echo "[INFO] end sourced data" >&2
. $sf
# -------------------------------------------------------------------------------

unregister=0				# -U to turn this into an unregister
forreal=""					# set when no-exec mode enabled

# commandline parms override what we find in the config, so parsed late
while [[ $1 == -* ]]
do
	case $1 in
		-N)	xapp_name=$2; shift;;
		-n)	forreal="echo no-execute mode, would run: ";;
		-u)	xam_url="$2"; shift;;				# mostly for testing, but in a pinch might be useful
		-U)	unregister=1;;
		-V) version=$2; shift;;

		*)	echo "[FAIL] unrecognised option: $1" >&2
			exit 1
			;;
	esac

	shift
done

if [[ $xam_url != "http"* ]]
then
	echo "[FAIL] url for xapp manager is not close to being valid: $xam_url" >&2
	exit 1
fi

if [[ -z $xapp_name || -z $version ]]
then
	echo "[FAIL] could not find xapp name and/or version in the config; not supplied on the command line" >&2
	exit 1
fi

if (( unregister ))
then
	echo "[INFO] sending unregister to xAPP mgr: $xam_url" >&2
	app_name=$(    quote appName "$xapp_name" )
	app_in_name=$( quote appInstanceName  "?????" )

	$forreal curl -X POST "${xam_url:-http://junk-not-supplied}/deregister" -H "accept: application/json" -H "Content-Type: application/json" \
		-d "{ $app_name, $app_in_name }"
	rv=$?

	rm -fr /tmp/PID$$.*							# clean things (more important in test env than container)
	exit $rv
fi

if [[  -s $df ]]
then
	config_junk=$( encode_json.py $df )			# squish the config and escape quotes etc
else
	echo "[FAIL] no descriptor file (config) found, or file had no size" >&2
	exit 1
fi


# these are klunky, but makes quoting junk for the curl command a bit less tedious
app_name=$(    quote appName "$xapp_name" )
config_path=$( quote configPath "" )
app_in_name=$( quote appInstanceName  "?????" )
http_endpt=$(  quote httpEndpoint "" )
rmr_endpt=$(   quote rmrEndpoint "$svc_name:$port" )
config=$(      quote config "$config_junk" )

echo "[INFO] sending register to xAPP mgr: $xam_url" >&2
$forreal curl -X POST "${xam_url:-http://junk-not-supplied}/register" -H "accept: application/json" -H "Content-Type: application/json" \
	-d "{ $app_name, $config_path, $app_in_name, $http_endpt, $rmr_endpt, $config }"
rv=$?	# use curl result as exit value not results of cleanup ops


# tidy the space before going
rm -fr /tmp/PID$$.*

exit $rv


