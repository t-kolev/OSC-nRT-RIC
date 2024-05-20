# Copyright (c) 2019 AT&T Intellectual Property.
# Copyright (c) 2018-2022 Nokia.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

#
# This source code is part of the near-RT RIC (RAN Intelligent Controller)
# platform project (RICP).
#

import queue
import time
from unittest.mock import Mock
import pytest
import ricsdl.backend
from ricsdl.configuration import _Configuration
from ricsdl.configuration import DbBackendType


@pytest.fixture()
def fake_dict_backend_fixture(request):
    request.cls.ns = 'some-ns'
    request.cls.dm = {'abc': b'1', 'bcd': b'2'}
    request.cls.new_dm = {'abc': b'3', 'bcd': b'2'}
    request.cls.dm2 = {'cdf': b'4'}
    request.cls.remove_dm = {'bcd': b'2'}
    request.cls.key = 'abc'
    request.cls.keys = ['abc', 'bcd']
    request.cls.key2 = ['cdf']
    request.cls.old_data = b'1'
    request.cls.new_data = b'3'
    request.cls.keypattern = r'*bc*'
    request.cls.group = 'some-group'
    request.cls.groupmember = b'm1'
    request.cls.groupmembers = set([b'm1', b'm2'])
    request.cls.new_groupmembers = set(b'm3')
    request.cls.all_groupmembers = request.cls.groupmembers | request.cls.new_groupmembers
    request.cls.channels = ['abs', 'gma']
    request.cls.channels_and_events = {'abs': ['cbn']}

    request.cls.configuration = Mock()
    mock_conf_params = _Configuration.Params(db_host=None,
                                             db_ports=None,
                                             db_sentinel_ports=None,
                                             db_sentinel_master_names=None,
                                             db_cluster_addrs=None,
                                             db_type=DbBackendType.FAKE_DICT)
    request.cls.configuration.get_params.return_value = mock_conf_params
    request.cls.db = ricsdl.backend.get_backend_instance(request.cls.configuration)


@pytest.mark.usefixtures('fake_dict_backend_fixture')
class TestFakeDictBackend:
    def test_is_connected_function_success(self):
        ret = self.db.is_connected()
        assert ret is True

    def test_set_function_success(self):
        self.db.set(self.ns, self.dm)
        self.db.set(self.ns, self.dm2)
        ret = self.db.get(self.ns, self.keys)
        assert ret == self.dm
        ret = self.db.get(self.ns, self.key2)
        assert ret == self.dm2

    def test_set_if_function_success(self):
        self.db.set(self.ns, self.dm)
        ret = self.db.set_if(self.ns, self.key, self.old_data, self.new_data)
        assert ret is True
        ret = self.db.get(self.ns, self.keys)
        assert ret == self.new_dm

    def test_set_if_function_returns_false_if_existing_key_value_not_expected(self):
        self.db.set_if(self.ns, self.key, self.old_data, self.new_data)
        self.db.set(self.ns, self.new_dm)
        ret = self.db.set_if(self.ns, self.key, self.old_data, self.new_data)
        assert ret is False

    def test_set_if_not_exists_function_success(self):
        ret = self.db.set_if_not_exists(self.ns, self.key, self.new_data)
        assert ret is True
        ret = self.db.get(self.ns, self.keys)
        assert ret == {self.key: self.new_data}

    def test_set_if_not_exists_function_returns_false_if_key_already_exists(self):
        self.db.set(self.ns, self.dm)
        ret = self.db.set_if_not_exists(self.ns, self.key, self.new_data)
        assert ret is False

    def test_find_keys_function_success(self):
        self.db.set(self.ns, self.dm)
        ret = self.db.find_keys(self.ns, self.keypattern)
        assert ret == self.keys

    def test_find_keys_function_returns_empty_list_when_no_matching_keys_found(self):
        ret = self.db.find_keys(self.ns, self.keypattern)
        assert ret == []

    def test_find_and_get_function_success(self):
        self.db.set(self.ns, self.dm)
        ret = self.db.find_and_get(self.ns, self.keypattern)
        assert ret == self.dm

    def test_find_and_get_function_returns_empty_dict_when_no_matching_keys_exist(self):
        ret = self.db.find_and_get(self.ns, self.keypattern)
        assert ret == dict()

    def test_remove_function_success(self):
        self.db.set(self.ns, self.dm)
        self.db.remove(self.ns, self.keys)
        ret = self.db.get(self.ns, self.keys)
        assert ret == dict()

    def test_remove_if_function_success(self):
        self.db.set(self.ns, self.dm)
        ret = self.db.remove_if(self.ns, self.key, self.old_data)
        assert ret is True
        ret = self.db.get(self.ns, self.keys)
        assert ret == self.remove_dm

    def test_remove_if_function_returns_false_if_data_does_not_match(self):
        ret = self.db.remove_if(self.ns, self.key, self.old_data)
        assert ret is False
        self.db.set(self.ns, self.dm)
        ret = self.db.remove_if(self.ns, self.key, self.new_data)
        assert ret is False

    def test_add_member_function_success(self):
        self.db.add_member(self.ns, self.group, self.groupmembers)
        ret = self.db.get_members(self.ns, self.group)
        assert ret == self.groupmembers

        self.db.add_member(self.ns, self.group, self.new_groupmembers)
        ret = self.db.get_members(self.ns, self.group)
        assert ret == self.all_groupmembers

    def test_remove_member_function_success(self):
        self.db.remove_member(self.ns, self.group, self.groupmembers)
        self.db.add_member(self.ns, self.group, self.groupmembers)
        self.db.remove_member(self.ns, self.group, self.groupmembers)
        ret = self.db.get_members(self.ns, self.group)
        assert ret == set()

    def test_remove_group_function_success(self):
        self.db.remove_group(self.ns, self.group)
        ret = self.db.get_members(self.ns, self.group)
        assert ret == set()

    def test_is_member_function_success(self):
        ret = self.db.is_member(self.ns, self.group, b'not member')
        assert ret is False
        self.db.add_member(self.ns, self.group, self.groupmembers)
        ret = self.db.is_member(self.ns, self.group, self.groupmember)
        assert ret is True
        ret = self.db.is_member(self.ns, self.group, b'not member')
        assert ret is False

    def test_group_size_function_success(self):
        ret = self.db.group_size(self.ns, self.group)
        assert ret == 0
        self.db.add_member(self.ns, self.group, self.groupmembers)
        ret = self.db.group_size(self.ns, self.group)
        assert ret == len(self.groupmembers)

    def test_fake_dict_backend_object_string_representation(self):
        assert str(self.db) == str({'DB type': 'FAKE DB'})

    def test_set_and_publish_function_success(self):
        self.db.set_and_publish(self.ns, self.channels_and_events, self.dm)
        ret = self.db.get(self.ns, self.keys)
        assert ret == self.dm
        assert self.db._queue.qsize() == 1

    def test_set_if_and_publish_success(self):
        self.db.set(self.ns, self.dm)
        ret = self.db.set_if_and_publish(self.ns, self.channels_and_events, self.key, self.old_data,
                                         self.new_data)
        assert ret is True
        ret = self.db.get(self.ns, self.keys)
        assert ret == self.new_dm
        assert self.db._queue.qsize() == 1

    def test_set_if_and_publish_returns_false_if_existing_key_value_not_expected(self):
        self.db.set_if_and_publish(self.ns, self.channels_and_events, self.key, self.old_data,
                                   self.new_data)
        self.db.set(self.ns, self.new_dm)
        ret = self.db.set_if(self.ns, self.key, self.old_data, self.new_data)
        assert ret is False
        assert self.db._queue.qsize() == 0

    def test_set_if_not_exists_and_publish_success(self):
        ret = self.db.set_if_not_exists_and_publish(self.ns, self.channels_and_events, self.key,
                                                    self.new_data)
        assert ret is True
        ret = self.db.get(self.ns, self.keys)
        assert ret == {self.key: self.new_data}
        assert self.db._queue.qsize() == 1

    def test_set_if_not_exists_and_publish_returns_false_if_key_already_exists(self):
        self.db.set(self.ns, self.dm)
        ret = self.db.set_if_not_exists_and_publish(self.ns, self.channels_and_events, self.key,
                                                    self.new_data)
        assert ret is False
        assert self.db._queue.qsize() == 0

    def test_remove_and_publish_function_success(self):
        self.db.set(self.ns, self.dm)
        self.db.remove_and_publish(self.ns, self.channels_and_events, self.keys)
        ret = self.db.get(self.ns, self.keys)
        assert ret == dict()
        assert self.db._queue.qsize() == 1

    def test_remove_if_and_publish_success(self):
        self.db.set(self.ns, self.dm)
        ret = self.db.remove_if_and_publish(self.ns, self.channels_and_events, self.key,
                                            self.old_data)
        assert ret is True
        ret = self.db.get(self.ns, self.keys)
        assert ret == self.remove_dm
        assert self.db._queue.qsize() == 1

    def test_remove_if_and_publish_returns_false_if_data_does_not_match(self):
        ret = self.db.remove_if_and_publish(self.ns, self.channels_and_events, self.key,
                                            self.old_data)
        assert ret is False
        self.db.set(self.ns, self.dm)
        ret = self.db.remove_if_and_publish(self.ns, self.channels_and_events, self.key,
                                            self.new_data)
        assert ret is False
        assert self.db._queue.qsize() == 0

    def test_remove_all_publish_success(self):
        self.db.set(self.ns, self.dm)
        self.db.remove_all_and_publish(self.ns, self.channels_and_events)
        ret = self.db.get(self.ns, self.keys)
        assert ret == dict()
        assert self.db._queue.qsize() == 1

    def test_subscribe_channel_success(self):
        cb = Mock()
        self.db.subscribe_channel(self.ns, cb, self.channels)
        for channel in self.channels:
            assert self.db._channel_cbs.get(channel, None)
        assert not self.db._listen_thread.is_alive()

    def test_subscribe_channel_event_loop_success(self):
        cb = Mock()
        self.db.start_event_listener()
        self.db.subscribe_channel(self.ns, cb, self.channels)
        for channel in self.channels:
            assert self.db._channel_cbs.get(channel, None)
        assert self.db._listen_thread.is_alive()

    def test_unsubscribe_channel_success(self):
        self.db.subscribe_channel(self.ns, Mock(), self.channels)
        self.db.unsubscribe_channel(self.ns, [self.channels[0]])
        assert self.db._channel_cbs.get(self.channels[0], None) is None
        assert self.db._channel_cbs.get(self.channels[1], None)

    def test_listen(self):
        cb = Mock()
        self.db.start_event_listener()
        self.db.subscribe_channel(self.ns, cb, self.channels)
        self.db._queue.put(("abs", "cbn"))
        time.sleep(0.5)
        assert self.db._queue.qsize() == 0

    def test_start_event_listener_success(self):
        self.db.start_event_listener()
        assert self.db._run_in_thread

    def test_start_event_listener_subscribe_first(self):
        self.db._listen_thread.start = Mock()
        mock_cb = Mock()
        self.db._channel_cbs = {'abs': mock_cb}
        self.db.subscribe_channel(self.ns, Mock(), self.channels)
        self.db.start_event_listener()
        self.db._listen_thread.start.assert_called_once()

    def test_start_event_listener_fail(self):
        self.db._listen_thread.is_alive = Mock()
        self.db._listen_thread.is_alive.return_value = True
        with pytest.raises(Exception):
            self.db.start_event_listener()

    def test_handle_events_success(self):
        self.db._queue = Mock()
        self.db._queue.get.return_value = ('abs', 'cbn')
        mock_cb = Mock()
        self.db._channel_cbs = {'abs': mock_cb}
        assert self.db.handle_events() == ('abs', 'cbn')
        mock_cb.assert_called_once_with('abs', 'cbn')

    def test_handle_events_success_no_notification(self):
        self.db._queue = Mock()
        self.db._queue.get.side_effect = queue.Empty
        assert self.db.handle_events() is None

    def test_handle_events_fail_already_started(self):
        self.db._listen_thread = Mock()
        self.db._listen_thread.is_alive.return_value = True
        with pytest.raises(Exception):
            self.db.handle_events()

    def test_handle_events_fail_already_set(self):
        self.db._run_in_thread = True
        with pytest.raises(Exception):
            self.db.handle_events()

@pytest.fixture()
def fake_dict_backend_lock_fixture(request):
    request.cls.ns = 'some-ns'
    request.cls.lockname = 'some-lock-name'
    request.cls.expiration = 10
    request.cls.retry_interval = 0.1
    request.cls.retry_timeout = 1

    request.cls.configuration = Mock()
    mock_conf_params = _Configuration.Params(db_host=None,
                                             db_ports=None,
                                             db_sentinel_ports=None,
                                             db_sentinel_master_names=None,
                                             db_cluster_addrs=None,
                                             db_type=DbBackendType.FAKE_DICT)
    request.cls.configuration.get_params.return_value = mock_conf_params
    request.cls.lock = ricsdl.backend.get_backend_lock_instance(request.cls.configuration,
                                                                request.cls.ns,
                                                                request.cls.lockname,
                                                                request.cls.expiration,
                                                                Mock())


@pytest.mark.usefixtures('fake_dict_backend_lock_fixture')
class TestFakeDictBackendLock:
    def test_acquire_function_success(self):
        ret = self.lock.acquire(self.retry_interval, self.retry_timeout)
        assert ret is True

    def test_acquire_function_returns_false_if_lock_is_not_acquired(self):
        self.lock.acquire(self.retry_interval, self.retry_timeout)
        ret = self.lock.acquire(self.retry_interval, self.retry_timeout)
        assert ret is False

    def test_release_function_success(self):
        self.lock.acquire(self.retry_interval, self.retry_timeout)
        ret = self.lock.acquire(self.retry_interval, self.retry_timeout)
        assert ret is False
        self.lock.release()
        ret = self.lock.acquire(self.retry_interval, self.retry_timeout)
        assert ret is True

    def test_get_validity_time_function_success(self):
        ret = self.lock.get_validity_time()
        assert ret == self.expiration

    def test_fake_dict_backend_lock_object_string_representation(self):
        expected_lock_info = {'lock DB type': 'FAKE DB',
                              'lock namespace': 'some-ns',
                              'lock name': 'some-lock-name',
                              'lock status': 'unlocked'}
        assert str(self.lock) == str(expected_lock_info)
