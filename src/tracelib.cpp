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

#include "tracelibcpp.hpp"
#include "config.hpp"

using namespace tracelibcpp;

std::string ConfMaker::getEnv(const char* envName, std::string defVal)
{
    const char *ev = getenv(envName);
    if (!ev)
        return defVal;
    return std::string(ev);
}

bool ConfMaker::isTracingEnabled()
{
    std::string envValue = getEnv(TRACING_ENABLED_ENV, "false");
    if (envValue == "1" || boost::iequals(envValue, "true"))
        return true;
    else
        return false;
}

jaegertracing::Config ConfMaker::makeNopTraceConfig()
{
    return jaegertracing::Config(true,
            jaegertracing::samplers::Config("const", 0));
}

jaegertracing::samplers::Config ConfMaker::getSamplerConfig()
{
    std::string samplerType = getEnv(JAEGER_SAMPLER_TYPE_ENV, "const");
    // Use value 0.001 as default param, same way as jaeger does it
    double param = atof(getEnv(JAEGER_SAMPLER_PARAM_ENV, "0.001").c_str());
    return jaegertracing::samplers::Config(samplerType, param);
}

jaegertracing::reporters::Config ConfMaker::getReporterConfig()
{
    std::string agentHostPort = getEnv(JAEGER_AGENT_ADDR_ENV, jaegertracing::reporters::Config::kDefaultLocalAgentHostPort);

    if (agentHostPort.find(':') == std::string::npos)
        agentHostPort += ":6831";

    return jaegertracing::reporters::Config(
        0, std::chrono::seconds(0),     // use jaeger defaults
        getLoggingLevel() == LOG_ALL,   // log spans
        agentHostPort
    );
}

LogLevel ConfMaker::getLoggingLevel()
{
    std::string logLevel = getEnv(JAEGER_LOG_LEVEL_ENV, "error");
    if (boost::iequals(logLevel, "all"))
        return LOG_ALL;
    else if (boost::iequals(logLevel, "error"))
        return LOG_ERR;
    else
        return LOG_NONE;
}

std::unique_ptr<jaegertracing::logging::Logger> ConfMaker::getLogger()
{
    switch (getLoggingLevel())
    {
        case LOG_ALL:
        case LOG_ERR:
            return jaegertracing::logging::consoleLogger();
            break;
        default:
            return jaegertracing::logging::nullLogger();
    }
}

jaegertracing::Config ConfMaker::getTraceConfig()
{
    if (!isTracingEnabled())
        return makeNopTraceConfig();
    auto sampler = getSamplerConfig();
    auto reporter = getReporterConfig();
    return jaegertracing::Config(false, sampler, reporter);
}

std::shared_ptr<opentracing::Tracer> tracelibcpp::createTracer(std::string serviceName)
{
    auto cm = ConfMaker(serviceName);
    auto config = cm.getTraceConfig();
    try {
        return jaegertracing::Tracer::make(serviceName, config, cm.getLogger());
    } catch (std::exception& e)
    {
        if (cm.getLoggingLevel() != LOG_NONE)
            std::cerr << "Cannot create tracer: " << e.what() << std::endl;
        return jaegertracing::Tracer::make(serviceName, cm.makeNopTraceConfig());
    }
}


