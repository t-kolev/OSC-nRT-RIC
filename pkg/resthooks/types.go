/*
==================================================================================
  Copyright (c) 2021 Samsung

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

   This source code is part of the near-RT RIC (RAN Intelligent Controller)
   platform project (RICP).
==================================================================================
*/
package resthooks

import (
	"gerrit.o-ran-sc.org/r/ric-plt/a1/pkg/rmr"
	"gerrit.o-ran-sc.org/r/ric-plt/xapp-frame/pkg/xapp"
)

type Resthook struct {
	db             iSdl
	iRmrSenderInst rmr.IRmrSender
}
type iSdl interface {
	GetAll(string) ([]string, error)
	SetIfNotExists(ns string, key string, data interface{}) (bool, error)
	Get(string, []string) (map[string]interface{}, error)
	SetIf(ns string, key string, oldData, newData interface{}) (bool, error)
	Set(ns string, pairs ...interface{}) error
	Remove(ns string, keys []string) error
}

type iRMRClient interface {
	SendMsg(params *xapp.RMRParams) bool
}
