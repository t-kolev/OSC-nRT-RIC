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

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "tracelibcpp.hpp"
#include "config.hpp"

using namespace testing;
using namespace tracelibcpp;

class ConfMakerTest: public ::testing::Test
{
public:
    ConfMaker cm;
    ConfMakerTest():
        cm(ConfMaker("testservice"))
    {
    }
    ~ConfMakerTest()
    {
        unsetenv(JAEGER_SAMPLER_TYPE_ENV);
        unsetenv(TRACING_ENABLED_ENV);
        unsetenv(JAEGER_SAMPLER_PARAM_ENV);
        unsetenv(JAEGER_AGENT_ADDR_ENV);
        unsetenv(JAEGER_LOG_LEVEL_ENV);
    }
};

TEST_F(ConfMakerTest, TestEnvReadingNonExisingReturnsDefaultValue)
{
    EXPECT_THAT("foobar", StrEq(cm.getEnv("nonexistent", "foobar")));
}

TEST_F(ConfMakerTest, TestEnvReadingReturnsEnvironmentValValue)
{
    setenv("FOO", "BAR", 1);
    auto val = cm.getEnv("FOO", "foobar");
    unsetenv("FOO");
    EXPECT_THAT("BAR", StrEq(val));
}

TEST_F(ConfMakerTest, TestTracingIsDisabledByDefault)
{
    EXPECT_FALSE(cm.isTracingEnabled());
}

TEST_F(ConfMakerTest, TestTracingCanBeEnabledWithEnvValTrue)
{
    setenv(TRACING_ENABLED_ENV, "true", 1);
    EXPECT_TRUE(cm.isTracingEnabled());
    setenv(TRACING_ENABLED_ENV, "TRUE", 1);
    EXPECT_TRUE(cm.isTracingEnabled());
}

TEST_F(ConfMakerTest, TestTracingCanBeEnabledWithEnvValOne)
{
    setenv(TRACING_ENABLED_ENV, "1", 1);
    EXPECT_TRUE(cm.isTracingEnabled());
}

TEST_F(ConfMakerTest, TestTracingEnabledWithUnknownValuesResultsDisabled)
{
    setenv(TRACING_ENABLED_ENV, "0", 1);
    EXPECT_FALSE(cm.isTracingEnabled());
    setenv(TRACING_ENABLED_ENV, "off", 1);
    EXPECT_FALSE(cm.isTracingEnabled());
}

TEST_F(ConfMakerTest, TestThatByDefaultConstSamplerIsCreated)
{
    auto samplerconf = cm.getSamplerConfig();
    EXPECT_THAT("const", StrEq(samplerconf.type()));
    EXPECT_EQ(0.001, samplerconf.param());
}

TEST_F(ConfMakerTest, TestThatSamplerTypeCanBeDefined)
{
    setenv(JAEGER_SAMPLER_TYPE_ENV, "probabilistic", 1);
    auto samplerconf = cm.getSamplerConfig();
    EXPECT_THAT("probabilistic", StrEq(samplerconf.type()));
    EXPECT_EQ(0.001, samplerconf.param());
}

TEST_F(ConfMakerTest, TestThatSamplerParamCanBeDefined)
{
    setenv(JAEGER_SAMPLER_PARAM_ENV, "0.01", 1);
    setenv(JAEGER_SAMPLER_TYPE_ENV, "probabilistic", 1);
    auto samplerconf = cm.getSamplerConfig();
    EXPECT_EQ(0.01, samplerconf.param());
}

TEST_F(ConfMakerTest, TestThatReporterAgentAddrDefaultsToLocalhost)
{
    auto reporterConf = cm.getReporterConfig();
    EXPECT_THAT(reporterConf.localAgentHostPort(), StrEq("127.0.0.1:6831"));
}

TEST_F(ConfMakerTest, TestThatReporterAgentAddrCanBeDefined)
{
    setenv(JAEGER_AGENT_ADDR_ENV, "1.1.1.1:1111", 1);
    auto reporterConf = cm.getReporterConfig();
    EXPECT_THAT(reporterConf.localAgentHostPort(), StrEq("1.1.1.1:1111"));
}

TEST_F(ConfMakerTest, TestThatIfAgentPortIsNotGivenDefaultIsUsed)
{
    setenv(JAEGER_AGENT_ADDR_ENV, "1.1.1.1", 1);
    auto reporterConf = cm.getReporterConfig();
    EXPECT_THAT(reporterConf.localAgentHostPort(), StrEq("1.1.1.1:6831"));
}

TEST_F(ConfMakerTest, TestThatLoggingDefaultIsErr)
{
    EXPECT_EQ(LOG_ERR, cm.getLoggingLevel());
}

TEST_F(ConfMakerTest, TestThatLoggingLevelCanBeConfigured)
{
    setenv(JAEGER_LOG_LEVEL_ENV, "all", 1);
    EXPECT_EQ(LOG_ALL, cm.getLoggingLevel());

    setenv(JAEGER_LOG_LEVEL_ENV, "error", 1);
    EXPECT_EQ(LOG_ERR, cm.getLoggingLevel());

    setenv(JAEGER_LOG_LEVEL_ENV, "none", 1);
    EXPECT_EQ(LOG_NONE, cm.getLoggingLevel());
}

TEST_F(ConfMakerTest, TestThatByDefaultDisabledConfigIsCreated)
{
    auto conf = cm.getTraceConfig();
    EXPECT_TRUE(conf.disabled());
}

TEST_F(ConfMakerTest, TestThatIfTracerCreationFailsNoopTracerIsReturned)
{
    setenv(TRACING_ENABLED_ENV, "1", 1);
    setenv(JAEGER_AGENT_ADDR_ENV, "foobar.invalid", 1); // invalid address causes an exception
    auto tracer = createTracer("foo");
    EXPECT_THAT(tracer, NotNull());
}

TEST(CreateTest, TestThatTraceCreateReturnsANewInstance) {
    auto tracer = createTracer("foo");
    EXPECT_THAT(tracer, NotNull());
}

