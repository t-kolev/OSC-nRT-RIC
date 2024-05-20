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

package tracelibgo

import (
	"os"
	"testing"

	"github.com/stretchr/testify/assert"
	"github.com/stretchr/testify/suite"
)

type ConfMakerTestSuite struct {
	suite.Suite
	cm confMaker
}

func (suite *ConfMakerTestSuite) SetupTest() {
	suite.cm = confMaker{"foo"}
}

func (suite *ConfMakerTestSuite) TearDownTest() {
	os.Unsetenv(tracingEnabledEnv)
	os.Unsetenv(jaegerSamplerTypeEnv)
	os.Unsetenv(jaegerSamplerParamEnv)
	os.Unsetenv(jaegerAgentAddrEnv)
	os.Unsetenv(jaegerLogLevelEnv)
}

func (suite *ConfMakerTestSuite) TestTracingIsDisabledByDefault() {
	suite.False(suite.cm.IsTracingEnabled())
}

func (suite *ConfMakerTestSuite) TestTracingCanBeEnabledWithEnvVar() {
	os.Setenv(tracingEnabledEnv, "1")
	suite.True(suite.cm.IsTracingEnabled())
	os.Setenv(tracingEnabledEnv, "true")
	suite.True(suite.cm.IsTracingEnabled())
}

func (suite *ConfMakerTestSuite) TestTracingEnabledWithUnknownValueResultsDisabled() {
	os.Setenv(tracingEnabledEnv, "0")
	suite.False(suite.cm.IsTracingEnabled())
	os.Setenv(tracingEnabledEnv, "foo")
	suite.False(suite.cm.IsTracingEnabled())
}

func (suite *ConfMakerTestSuite) TestSamplerTypeDefaultIsConst() {
	suite.Equal("const", suite.cm.getSamplerConfig().Type)
}

func (suite *ConfMakerTestSuite) TestSamplerTypeParamDefault() {
	suite.Equal(0.001, suite.cm.getSamplerConfig().Param)
}

func (suite *ConfMakerTestSuite) TestSamplerTypeCanBeDefined() {
	os.Setenv(jaegerSamplerTypeEnv, "probabilistic")
	suite.Equal("probabilistic", suite.cm.getSamplerConfig().Type)
}

func (suite *ConfMakerTestSuite) TestIfSamplerParamIsInvalidZeroValueIsUsed() {
	os.Setenv(jaegerSamplerParamEnv, "foo")
	suite.Equal(0.0, suite.cm.getSamplerConfig().Param)
}

func (suite *ConfMakerTestSuite) TestAgentAddrCanBeDefined() {
	os.Setenv(jaegerAgentAddrEnv, "1.1.1.1:1111")
	suite.Equal("1.1.1.1:1111", suite.cm.getReporterConfig().LocalAgentHostPort)
}

func (suite *ConfMakerTestSuite) TestAgentAddressPortIsOptional() {
	os.Setenv(jaegerAgentAddrEnv, "1.1.1.1")
	suite.Equal("1.1.1.1:6831", suite.cm.getReporterConfig().LocalAgentHostPort)
}

func (suite *ConfMakerTestSuite) TestLoggingLevelDefaultIsErr() {
	suite.Equal(logErr, suite.cm.getLoggingLevel())
}

func (suite *ConfMakerTestSuite) TestLoggingLevelCanBeConfigured() {
	os.Setenv(jaegerLogLevelEnv, "error")
	suite.Equal(logErr, suite.cm.getLoggingLevel())
	os.Setenv(jaegerLogLevelEnv, "all")
	suite.Equal(logAll, suite.cm.getLoggingLevel())
	os.Setenv(jaegerLogLevelEnv, "none")
	suite.Equal(logNone, suite.cm.getLoggingLevel())
}

func (suite *ConfMakerTestSuite) TestConfiguredTracerCreate() {
	os.Setenv(tracingEnabledEnv, "1")
	os.Setenv(jaegerSamplerParamEnv, "const")
	os.Setenv(jaegerSamplerParamEnv, "1")
	os.Setenv(jaegerAgentAddrEnv, "127.0.0.1:6831")
	tracer, closer := CreateTracer("foo")
	suite.NotNil(tracer)
	suite.NotNil(closer)
}

func (suite *ConfMakerTestSuite) TestIfTracerCreationFailsDisabledTracerIsReturned() {
	os.Setenv(tracingEnabledEnv, "1")
	tracer, closer := CreateTracer("") // Empty name is an error
	suite.NotNil(tracer)
	suite.NotNil(closer)
}

func TestConfMakerSuite(t *testing.T) {
	suite.Run(t, new(ConfMakerTestSuite))
}

func TestDefaultTracerCreate(t *testing.T) {
	tracer, closer := CreateTracer("foo")
	assert.NotNil(t, tracer)
	assert.NotNil(t, closer)
	err := closer.Close()
	assert.Nil(t, err)
}
