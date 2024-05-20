/*
 * Copyright (c) 2019 AT&T Intellectual Property.
 * Copyright (c) 2018-2019 Nokia.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * This source code is part of the near-RT RIC (RAN Intelligent Controller)
 * platform project (RICP).
 */
#ifndef _TRACELIB_CONFIG_HPP_
#define _TRACELIB_CONFIG_HPP_

#include <boost/algorithm/string.hpp>
#include <jaegertracing/Tracer.h>

#define TRACING_ENABLED_ENV      "TRACING_ENABLED"
#define JAEGER_SAMPLER_TYPE_ENV  "TRACING_JAEGER_SAMPLER_TYPE"
#define JAEGER_SAMPLER_PARAM_ENV "TRACING_JAEGER_SAMPLER_PARAM"
#define JAEGER_AGENT_ADDR_ENV    "TRACING_JAEGER_AGENT_ADDR"
#define JAEGER_LOG_LEVEL_ENV     "TRACING_JAEGER_LOG_LEVEL"

namespace tracelibcpp
{
    typedef enum {
        LOG_ALL,
        LOG_ERR,
        LOG_NONE
    } LogLevel;
    class ConfMaker {
    public:
        ConfMaker(std::string serviceName):
            name(serviceName) {}

        std::string getEnv(const char* envName, std::string defVal);

        bool isTracingEnabled(void);

        jaegertracing::Config makeNopTraceConfig(void);

        jaegertracing::samplers::Config getSamplerConfig(void);

        jaegertracing::reporters::Config getReporterConfig(void);

        LogLevel getLoggingLevel(void);

        std::unique_ptr<jaegertracing::logging::Logger> getLogger(void);

        jaegertracing::Config getTraceConfig(void);

    private:
        std::string name;
    };
}

#endif
