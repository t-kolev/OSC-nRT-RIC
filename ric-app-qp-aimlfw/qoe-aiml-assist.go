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

package main

import (
	"gerrit.o-ran-sc.org/r/ric-app/qp-aimlfw/control"
	"gerrit.o-ran-sc.org/r/ric-plt/xapp-frame/pkg/xapp"
)

func main() {
	xapp.Logger.Info("Start qp-aiml-assist")
	c := control.NewControl()
	c.Run()
	defer afterMain()
}

func afterMain() {
	xapp.Logger.Info("Exited qp-aiml-assist")
}
