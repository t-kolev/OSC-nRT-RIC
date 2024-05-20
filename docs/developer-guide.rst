..
.. Copyright (c) 2019 AT&T Intellectual Property.
..
.. Copyright (c) 2019 Nokia.
..
..
.. Licensed under the Creative Commons Attribution 4.0 International
..
.. Public License (the "License"); you may not use this file except
..
.. in compliance with the License. You may obtain a copy of the License at
..
..
..     https://creativecommons.org/licenses/by/4.0/
..
..
.. Unless required by applicable law or agreed to in writing, documentation
..
.. distributed under the License is distributed on an "AS IS" BASIS,
..
.. WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
..
.. See the License for the specific language governing permissions and
..
.. limitations under the License.
..
..   This source code is part of the near-RT RIC (RAN Intelligent Controller)
..  platform project (RICP).
..

Developer Guide
===============
Tracing helper library
The library creates a configured tracer instance.

Tracelibgo Repo
---------------

.. code:: bash

 git clone "https://gerrit.o-ran-sc.org/r/ric-plt/tracelibgo"


Usage
-----
Create a tracer instance and set it as a global tracer:


.. code:: bash

 import (
        "github.com/opentracing/opentracing-go"
        "gerrit.o-ran-sc.org/ric-plt/tracelibgo/pkg/tracelibgo"
        ...
 )

 tracer, closer := tracelibgo.CreateTracer("my-service-name")
 defer closer.Close()
 opentracing.SetGlobalTracer(tracer)


Serialize span context to a byte array that can be sent to another component via some messaging. For example, using the RMR library. The serialization uses JSON format.

.. code:: bash

        carrier := make(map[string]string)
        opentracing.GlobalTracer().Inject(
                        span.Context(),
                        opentracing.TextMap,
                        opentracing.TextMapCarrier(carrier))
        b, err := json.Marshal(carrier) // b is a []byte and contains serilized span context

Extract a span context from byte array and create a new child span from it.The serialized span context is got, for example, from the RMR library.

.. code:: bash

        var carrier map[string]string
        err = json.Unmarshal(data, &carrier) // data is []byte containing serialized span context
        if err != nil {
                ...
        }
        context, err := opentracing.GlobalTracer().Extract(opentracing.TextMap, opentracing.TextMapCarrier(carrier))
        if err != nil {
                ...
        }
        span := opentracing.GlobalTracer().StartSpan("go test span", opentracing.ChildOf(context))

Configuration
-------------

The trace library currently supports only [Jaeger](https://www.jaegertracing.io/) [golang client](https://github.com/jaegertracing/jaeger-client-go) tracer implementation.
The configuration is done using environment variables:

+------------------------------+-------------------------------------+----------------+
|  **environment variable**    |        **values**                   | **default**    |
|                              |                                     |                |
+------------------------------+-------------------------------------+----------------+
| TRACING_ENABLED              | 1, true, 0, false                   | false          |
|                              |                                     |                |
+------------------------------+-------------------------------------+----------------+
| TRACING_JAEGER_SAMPLER_TYPE  | const, propabilistic, ratelimiting  | const          |
|                              |                                     |                |
+------------------------------+-------------------------------------+----------------+
| TRACING_JAEGER_SAMPLER_PARAM | float                               | 0.001          |
|                              |                                     |                |
+------------------------------+-------------------------------------+----------------+
| TRACING_JAEGER_AGENT_ADDR    | IP addr[:port]                      | 127.0.0.1:6831 |
|                              |                                     |                |
+------------------------------+-------------------------------------+----------------+
| TRACING_JAEGER_LOG_LEVEL     | all, error, none                    | none           |
|                              |                                     |                |
+------------------------------+-------------------------------------+----------------+

Meaning of the configuration variables is described in Jaeger web pages.
By default a no-op tracer is created.


Unit testing
------------

.. code:: bash

 GO111MODULE=on go mod download
 go test ./pkg/tracelibgo

