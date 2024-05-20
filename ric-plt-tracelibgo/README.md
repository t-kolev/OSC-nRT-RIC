# Tracing helper library

The library creates a configured tracer instance.


## Usage

Create a tracer instance and set it as a global tracer:

```go
import (
		"github.com/opentracing/opentracing-go"
        "gerrit.o-ran-sc.org/ric-plt/tracelibgo/pkg/tracelibgo"
        ...
)

tracer, closer := tracelibgo.CreateTracer("my-service-name")
defer closer.Close()
opentracing.SetGlobalTracer(tracer)
```

Serialize span context to a byte array that can be sent
to another component via some messaging. For example, using
the RMR library. The serialization uses JSON format.

```go
	carrier := make(map[string]string)
	opentracing.GlobalTracer().Inject(
			span.Context(),
			opentracing.TextMap,
			opentracing.TextMapCarrier(carrier))
	b, err := json.Marshal(carrier) // b is a []byte and contains serilized span context
```

Extract a span context from byte array and create a new child span from it.
The serialized span context is got, for example, from the RMR library.

```go
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
```

## Configuration

The trace library currently supports only [Jaeger](https://www.jaegertracing.io/) [golang client](https://github.com/jaegertracing/jaeger-client-go) tracer implementation.
The configuration is done using environment variables:

| environment variable         | values                              | default        |
| ---------------------------- |------------------------------------ | -------------- |
| TRACING_ENABLED              | 1, true, 0, false                   | false          |
| TRACING_JAEGER_SAMPLER_TYPE  | const, propabilistic, ratelimiting  | const          |
| TRACING_JAEGER_SAMPLER_PARAM | float                               | 0.001          |
| TRACING_JAEGER_AGENT_ADDR    | IP addr[:port]                      | 127.0.0.1:6831 |
| TRACING_JAEGER_LOG_LEVEL     | all, error, none                    | none           |

Meaning of the configuration variables is described in Jaeger web pages.
By default a no-op tracer is created.


## Unit testing

 GO111MODULE=on go mod download
 go test ./pkg/tracelibgo

## License

See [LICENSES.txt](LICENSES.txt) file.
