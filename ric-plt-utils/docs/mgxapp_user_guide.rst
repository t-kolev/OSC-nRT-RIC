.. This work is licensed under a Creative Commons Attribution 4.0
.. International License.  SPDX-License-Identifier: CC-BY-4.0

.. CAUTION: this document is generated from source in mgxapp/doc/src/.
.. To make changes edit the source and recompile the document.
.. Do NOT make changes directly to .rst or ^.md files.


============
USER'S GUIDE
============
-------------------
RIC Metrics Gateway
-------------------


OVERVIEW
========

The *Metrics Gateway* is an RMR based RIC xAPP designed to
listen for ``RIC_METRICS`` messages forwarding the data from
these messages to some statistics consumer. Currently the
consumer is the VES collector.


INBOUND MESSAGES
================

Messages sent via RMR with the message type
``RIC_METRICS`` should be automatically routed to the Metrics
Gateway. The messages are expected to have a json payload
which contains the following information, illustrated in
figure 1, with the schema shown in figure 2:


       .. list-table::
         :widths: 15,80
         :header-rows: 0
         :class: borderless


         * - **Source**

           -

             The name of the application which is sending the status

             value(s). If omitted the source from the RMR message

             (hostname:port) is used.





             |



         * - **Generator**

           -

             The name of the application which generated the value. If

             omitted the *source* is used.





             |



         * - **Timestamp**

           -

             This is an integer value which is the number of

             milliseconds since the UNIX epoch (Jan 1, 1970) and is

             forwarded as the time that the value was captured. If

             omitted, the time that the message is received by the

             Metrics Gateway is used (not recommended).





             |



         * - **Data**

           -

             The data array is an array with one or more objects that

             define a specific statistic (measurement). Each object

             must specify an ``ID`` and ``value,`` and may optionally

             supply a type. (The type is not currently used, but might

             in the future be used to identify the value as a counter,

             meter value, percentage, or similar classification that is

             meaningful to the process that the Metrics Gateway is

             forwarding to.



::

    {
      "msg_source": "state_mon86",
      "value_source": "device89.port219",
      "timestamp": 515300400000,
      "data": [
        {
          "id": "voltage",
          "type": "value",
          "value":  110.0430,
        }
      ]
    }

Figure 1: Sample json payload for a single value.


If the xAPP is using a framework, it is likely that the
framework provides the API for creating and sending messages
to the Metrics Gateway.

::

  {
    "$schema": "http://json-schema.org/draft-04/schema#",
    "type": "object",
    "properties": {
      "msg_source": {
        "description": "the system or process sending the message",
        "default":  "the RMR source string (host:port)",
        "type": "string"
      },
      "vlaue_source": {
        "description": "the system or process which took the measurement",
        "default": "msg_source is assumed to be the generator if this is omitted",
        "type": "string"
      },
      "timestamp": {
        "description": "milliseconds past the UNIX epoch; e.g. 1594298319000 == 2020/07/09 08:38:39.000 UTC-4",
        "default":  "the message arrival time"
        "type": "integer"
      },
      "data": {
        "description": "one or more statistics measurements",
        "type": "array",
        "items": [
          {
            "type": "object",
            "properties": {
              "id": {
                "description": "measurement name",
                "type": "string"
              },
              "type": {
                "description": "future: measurement type such as counter, value, or similar",
                "type": "string"
              },
              "value": {
                "description": "actual value; treated as a double when forwarded",
                "type": "number"
              }
            },
            "required": [
              "id",
              "value"
            ]
          }
        ]
      }
    },
    "required": [
      "data"
    ]
  }

Figure 2: The schema for the json payload expected by the
Metrics Gateway.



FORWARDING EXPECTATIONS
=======================

The Metrics Gateway expects that the data which it forwards
is sent as a json payload to the URL defined via command line
paramter or as a field in the ``controls`` section of the
configuration file. Currently the Metrics Gateway assumes
that the target is the VES collction application and accepts
messages as defined by the specification(s) at the following
site:


::

  docs.onap.org/en/elalto/submodules/vnfrqts/requirements.git/docs/Chapter8/ves7_1spec.html




EXECUTION
=========

The Metrics Gateway is a single binary which can be executed
as a stand alone container. The binary is ``munchkin`` and is
installed by default in ``/usr/local/bin.``


Environment Variables
---------------------

The usual RMR environment variables will have the expected
effect if they are set when the process is invoked. Currently
the Metrics Gateway does not expect, or use, any environment
variables; all configuration is controlled by command line
options and/or the configuration file.


Command Line Options
--------------------

The process allows several options to be supplied on the
command line. The assumption is that none will be necessary
for the general execution case, but these options provide
flexibility for testing, and should the process need to be
colocated with another xAPP in a single container. The
following is a list of command line options which are
supported:


       .. list-table::
         :widths: 15,80
         :header-rows: 0
         :class: borderless


         * - **-d**

           -

             Places logging into *debug* mode.





             |



         * - **-c config-file**

           -

             Supplies the name of the configuration file. (Described in

             a later section.)





             |



         * - **-l filename**

           -

             Writes the standard error messages to the named file

             rather than to standard error. Implies human readable

             format (the RIC logging library makes no provision to

             redirect messages). (This option is a lower case 'L.')





             |



         * - **-t n**

           -

             Number of threads. This is passed to the framework and

             allows for multiple concurrent callback threads to be

             created. Currently it is not anticipated that this will be

             needed.





             |



         * - **-P port-name**

           -

             Supplies the port name that should be matched in the

             *messaging* section of the configuration file. This option

             is valid only when the ``-c`` option is supplied and

             **must** be placed on the command line **before** the

             ``-c`` option. When not supplied, the port name that will

             be lifted from the config is *rmr-data.*





             |



         * - **-r**

           -

             Enable human readable messages written by the Metrics

             Gateway. By default, the Metrics Gateway uses the RIC

             logging library which generates unfriendly json encrusted

             output; this turns that off.





             |



         * - **-T url**

           -

             The URL of the process that is the target of Metrics

             Gateway output.





             |



         * - **-v**

           -

             Verbose mode. The Metrics Gateway will be chatty to the

             standard error device.





             |



         * - **-V**

           -

             Verbose mode. The Metrics Gateway will be chatty to the

             standard error device but will write human readable

             messages and not json encrusted log messages.





             |



         * - **-w**

           -

             Wait for RMR route table. Normally the Metrics Gateway

             does not need to wait for an RMR route table to arrive

             before it can start processing. Should that need arise,

             this option will put the Metrics Gateway into a hold until

             the table is received and validated.




The Configuration File
----------------------

The xAPP descriptor can be supplied to the Metrics Gateway
and will be used as the source for configuration data.
Specifically the file is assumed to be valid json, and only
the ``controls`` and ``messaging`` sections are used. Figure
3 illustrates an example of these sections.

::


    "messaging": {
      "ports": [
        {
          "name": "rmr-data",
          "container": "mgxapp",
          "port": 4560,
          "rxMessages":
          [
            "RIC_METRICS"
          ],
          "description": "sgxapp listens on this port for RMR messages."
        },
        {
          "name": "rmr_route",
          "container": "mgxapp",
          "port": 4561,
          "description": "mgxapp listens on this port for RMR route messages"
        }
      ]
    },

    "controls": {
      "collector_url": "https://ves_collector:43086",
      "hr_logs":    false,
      "log_file": "/dev/stderr",
      "log_level": "warn",
      "wait4rt":    false
    },

Figure 3: Sections from a configuration file processed by the
Metrics Gateway



The Messaging Section
---------------------

The messaging section in the configuration file is assumed to
have an array of port objects. The Metrics Gateway will
examine each until it finds the expected port *name*
("rmr-data" by default) and will use the port associated with
the The command line flag ``-P`` can be used to supply an
alternate port name when necessary. All other fields in each
port object are ignored and are assumed to be used by other
container management functions.


The Controls Section
--------------------

The ``controls`` section is analogous to the command line
options and supplies most of the same information that can be
supplied from the command line. The following lists the
fields which the Metrics Gateway recognises from this
section.


       .. list-table::
         :widths: 15,80
         :header-rows: 0
         :class: borderless


         * - **collector_url**

           -

             Defines the URL that the Metrics Gateway will forward

             metrics to.





             |



         * - **log_file**

           -

             Supplies a destination for messages which are normally

             written to the standard error. This applies only if the

             human readable messages option is true as the RIC logging

             library makes no provision for capturing log messages in a

             named file.





             |



         * - **wait4rt**

           -

             Causes the Metrics Gateway to wait for an RMR route table

             to be received and installed before starting. Normally a

             route table is not needed by the Metrics Gateway, so

             processing can begin before any route table is received.





             |



         * - **hr_logs**

           -

             When set to ``true`` causes human readable messages to be

             written to standard error rather than the json encrusted

             messages generated by the RIC logging library. The default

             if omitted is ``false.``





             |



         * - **log_level**

           -

             Defines the log level which should be one of the following

             strings: ``crit, err, warn, info,`` or ``debug.`` If not

             supplied, the default is ``warn.``





Combining Options and Config File
---------------------------------

It is possible to provide the Metrics Gateway with a
configuration file and to override any values in the
configuration file with command line options. It is also
possible to set options on the command line which are treated
as defaults should the value not exist in the
``controls`` section of the configuration file. These are
both accomplished by carefully ordering the command line
options when starting the Metrics Gateway.

All options which appear on the command line **before** the
``-c`` are treated as default values. These values will be
used only if they are **not** defined in the configuration
file. Any options placed on the command line after the
``-c`` option are considered to be overrides to any
information in the configuration file. This is illustrated in
figure 4.


::

    munchkin -l /var/munchkin/log/msgs -c /var/munchkin/config.json -p 39282

Figure 4: Sample command line where port overrides the
configuration file.

In figure 4 The ``-l`` (lower case L) option provides the
default file for message that would normally be written to
standard error. If the config file contains the
``log_file`` field, then that value will be used instead of
the filename given on the command line. The port (-p) is
placed on the command line after the configuration file
option, and thus the indicated port will be used regardless
of what is in the configuration file.

