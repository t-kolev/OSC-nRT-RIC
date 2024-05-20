/*
   Copyright (c) 2018-2019 Nokia.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

/*
 * This source code is part of the near-RT RIC (RAN Intelligent Controller)
 * platform project (RICP).
*/

#include <gtest/gtest.h>
#include "private/system.hpp"

using namespace shareddatalayer;
using namespace testing;

TEST(SystemTest, HasVirtualDestructor)
{
    EXPECT_TRUE(std::has_virtual_destructor<System>::value);
}

TEST(SystemTest, IsNotCopyable)
{
    EXPECT_FALSE(std::is_copy_constructible<System>::value);
    EXPECT_FALSE(std::is_copy_assignable<System>::value);
}

TEST(SystemTest, CanCreateInstance)
{
    System& s1(System::getSystem());
    System& s2(System::getSystem());
    EXPECT_EQ(&s1, &s2);
}

TEST(SystemDeathTest, FailureWithCloseAbortsWithAbort)
{
    EXPECT_EXIT(System::getSystem().close(-1), KilledBySignal(SIGABRT), "");
}
