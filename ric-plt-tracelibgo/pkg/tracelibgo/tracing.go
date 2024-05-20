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
 *
 */

// Package tracelibgo implements a function to create a configured tracer instance
package tracelibgo

import (
	"fmt"
	"io"
	"os"
	"strconv"
	"strings"

	"github.com/opentracing/opentracing-go"
	jaegercfg "github.com/uber/jaeger-client-go/config"
	jaegerlog "github.com/uber/jaeger-client-go/log"
)

const tracingEnabledEnv string = "TRACING_ENABLED"
const jaegerSamplerTypeEnv string = "TRACING_JAEGER_SAMPLER_TYPE"
const jaegerSamplerParamEnv = "TRACING_JAEGER_SAMPLER_PARAM"
const jaegerAgentAddrEnv = "TRACING_JAEGER_AGENT_ADDR"
const jaegerLogLevelEnv = "TRACING_JAEGER_LOG_LEVEL"

type confMaker struct {
	ServiceName string
}

type logLevel int

const (
	logAll  logLevel = iota
	logErr  logLevel = iota
	logNone logLevel = iota
)

func (cm *confMaker) GetEnv(envName string, defval string) (retval string) {
	retval = os.Getenv(envName)
	if retval == "" {
		retval = defval
	}
	return
}

func (cm *confMaker) IsTracingEnabled() bool {
	val := cm.GetEnv(tracingEnabledEnv, "false")
	if val == "1" || strings.ToLower(val) == "true" {
		return true
	}
	return false
}

func createDisabledTracer(name string) (opentracing.Tracer, io.Closer) {
	if name == "" {
		name = "dummy"
	}
	cfg := jaegercfg.Configuration{
		ServiceName: name,
		Disabled:    true,
	}
	tracer, closer, err := cfg.NewTracer()
	if err != nil {
		fmt.Fprintln(os.Stderr, "tracelibgo: trace creation error: ", err.Error())
	}
	return tracer, closer
}

func (cm *confMaker) getSamplerConfig() jaegercfg.SamplerConfig {
	samplerType := cm.GetEnv(jaegerSamplerTypeEnv, "const")
	param, err := strconv.ParseFloat(cm.GetEnv(jaegerSamplerParamEnv, "0.001"), 64)
	if err != nil {
		param = 0
	}
	return jaegercfg.SamplerConfig{Type: samplerType, Param: param}
}

func (cm *confMaker) getReporterConfig() jaegercfg.ReporterConfig {
	agentHostPort := cm.GetEnv(jaegerAgentAddrEnv, "127.0.0.1:6831")
	if !strings.Contains(agentHostPort, ":") {
		agentHostPort += ":6831"
	}
	return jaegercfg.ReporterConfig{LogSpans: cm.getLoggingLevel() == logAll, LocalAgentHostPort: agentHostPort}
}

func (cm *confMaker) getLoggingLevel() logLevel {
	level := strings.ToLower(cm.GetEnv(jaegerLogLevelEnv, "error"))
	switch level {
	case "error":
		return logErr
	case "all":
		return logAll
	default:
		return logNone
	}
}

// CreateTracer creates a tracer entry
func CreateTracer(name string) (opentracing.Tracer, io.Closer) {

	cm := confMaker{name}
	if !cm.IsTracingEnabled() {
		return createDisabledTracer(name)
	}
	sampler := cm.getSamplerConfig()
	reporter := cm.getReporterConfig()
	cfg := jaegercfg.Configuration{
		ServiceName: name,
		Disabled:    false,
		Sampler:     &sampler,
		Reporter:    &reporter,
	}
	var jaegerLoggerOpt jaegercfg.Option
	switch cm.getLoggingLevel() {
	case logAll, logErr:
		jaegerLoggerOpt = jaegercfg.Logger(jaegerlog.StdLogger)
	default:
		jaegerLoggerOpt = jaegercfg.Logger(nil)
	}
	tracer, closer, err := cfg.NewTracer(jaegerLoggerOpt)
	if err != nil {
		fmt.Fprintln(os.Stderr, "tracelibgo: cannot init tracer: "+err.Error())
		return createDisabledTracer(name)
	}
	return tracer, closer
}
