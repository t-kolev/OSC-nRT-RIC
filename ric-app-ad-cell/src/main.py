
# ==================================================================================
#
#       Copyright (c) 2024 Samsung Electronics Co., Ltd. All Rights Reserved.
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#          http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.
#
# ==================================================================================
from ricxappframe.xapp_frame import Xapp

from .manager.InfluxDBManager import InfluxDBManager

from .utils import Util

log = Util.setup_logger()


def entry(self):
    log.debug('ADCellXapp.entry :: entry called')
    influxManager = InfluxDBManager()
    influxManager.query()


def launchXapp():
    log.debug('In main.py launchXapp method')
    xapp = Xapp(entrypoint=entry, rmr_port=4560, use_fake_sdl=False)
    log.debug("AD Cell xApp starting")
    xapp.run()


if __name__ == "__main__":
    launchXapp()
