/*
==================================================================================
      Copyright (c) 2022 Samsung Electronics Co., Ltd. All Rights Reserved.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

         http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

==================================================================================
*/
package mocks

import "gerrit.o-ran-sc.org/r/ric-plt/xapp-frame/pkg/xapp"

type FakeRMRClient struct{}

func NewFakeRMRClient() FakeRMRClient {
	return FakeRMRClient{}
}

func (frw FakeRMRClient) SendRts(msg *xapp.RMRParams) bool {
	return true
}

func (rw FakeRMRClient) GetRicMessageName(id int) string {
	return "ricMessageName"
}

func (frw FakeRMRClient) Send(msg *xapp.RMRParams, isRts bool) bool {
	return true
}
