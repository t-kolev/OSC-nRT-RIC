# RIC tracing helper library

The library includes a function for creating a configured tracer instance.
It hides the underlaying tracer implementation from the application.

## Usage

Create a global tracer

```c
    #include <opentracing/tracer.h>
    #include <tracelibcpp/tracelibcpp.hpp>

    opentracing::Tracer::InitGlobal(tracelibcpp::createTracer("my-service-name"));
```
Span context propagation between different software components in RIC is using a
TextMap carrier and JSON format serialization.
The [opentracing C++](https://github.com/opentracing/opentracing-cpp) Readme gives examples
how span context **inject** and **extract** with textmap can be done.
Serialization to JSON can be done with any JSON library.

## Configuration

The trace library currently supports only [Jaeger](https://www.jaegertracing.io/) [C++ client](https://github.com/jaegertracing/jaeger-client-cpp) tracer implementation.
The configuration is done using environment variables:

| environment variable         | values                              | default        |
| ---------------------------- |------------------------------------ | -------------- |
| TRACING_ENABLED              | 1, true, 0, false                   | false          |
| TRACING_JAEGER_SAMPLER_TYPE  | const, propabilistic, ratelimiting  | const          |
| TRACING_JAEGER_SAMPLER_PARAM | float                               | 0.001          |
| TRACING_JAEGER_AGENT_ADDR    | IP addr + port                      | 127.0.0.1:6831 |
| TRACING_JAEGER_LOG_LEVEL     | all, error, none                    | none           |

Meaning of the configuration variables is described in Jaeger web pages.
By default a no-op tracer is created.

## Requires

cmake
gcc/c++
opentracing-cpp version 1.5.0

## Build
mkdir build
cd build
cmake ..
make

## Unit testing
To run unit tests the project needs to be configured with testing option
cmake -DWITH_TESTING=ON ..
make check
Or with output
CTEST_OUTPUT_ON_FAILURE=1 make check

## Coverage
Unit testing generates also coverage data. To get that in html format run commands, assuming
you are building in `build` dir under the tracelibcpp
```shell
lcov -c --no-external --base-directory $(dirname $PWD)  --directory . --output-file cov.info
genhtml cov.info
```

## Binary package support
Binary packages of the libary can be created with `make package` target, or with
the Dockerfile in the `ci` directory.

The Docker build executes unit tests and compiles binary packages which can then be
exported from the container by running it and giving the target directory as a command line
argument. The target directory must mounted to the container.

```shell
# Build the container
docker build -t tracelibcpp -f ci/Dockerfile .
# Export binary packages to /tmp
docker run -v /tmp:/tmp tracelibcpp /tmp
```

## License

See [LICENSES.txt](LICENSES.txt) file.
