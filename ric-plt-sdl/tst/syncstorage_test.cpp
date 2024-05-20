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

#include <type_traits>
#include <gtest/gtest.h>
#include <sdl/syncstorage.hpp>

using namespace shareddatalayer;

TEST(SyncStorageTest, IsNotCopyableAndIsNotMovable)
{
    EXPECT_FALSE(std::is_copy_constructible<SyncStorage>::value);
    EXPECT_FALSE(std::is_copy_assignable<SyncStorage>::value);
    EXPECT_FALSE(std::is_move_constructible<SyncStorage>::value);
    EXPECT_FALSE(std::is_move_assignable<SyncStorage>::value);
}

TEST(SyncStorageTest, HasVirtualDestructor)
{
    EXPECT_TRUE(std::has_virtual_destructor<SyncStorage>::value);
}

TEST(SyncStorageTest, IsAbstract)
{
    EXPECT_TRUE(std::is_abstract<SyncStorage>::value);
}

TEST(SyncStorageTest, SyncStorageCreateInstanceHasCorrectType)
{
    auto syncStorageInstance(shareddatalayer::SyncStorage::create());
    EXPECT_EQ(typeid(std::unique_ptr<SyncStorage>), typeid(syncStorageInstance));
}
