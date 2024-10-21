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
class Constants:
    A1_POLICY_QUERY = 20012
    AD_CELL_POLICY_ID = 2
    RIC_HEALTH_CHECK_REQ = 100
    RIC_HEALTH_CHECK_RESP = 101
    A1_POLICY_REQ = 20010
    A1_POLICY_RESP = 20011
    RIC_ALARM_UPDATE = 110
    ACTION_TYPE = "REPORT"
    SUBSCRIPTION_PATH = "http://service-{}-{}-http:{}"
    PLT_NAMESPACE = "ricplt"
    SUBSCRIPTION_SERVICE = "submgr"
    SUBSCRIPTION_PORT = "3800"
    SUBSCRIPTION_REQ = 12011

    MEAN = "mean"
    SD = "sd"
    COUNT = "count"
    CONFIG_DIR = '/tmp/src/configuration'
    MODEL_AD_CELL_FILE_NAME = "XGBOOST_NEW_Data_AdCell.pkl"
    KPIS_INDEXER = ['Short name']
    REPO_COUNT = 'REPO_COUNT'
    REPO_MEAN = 'REPO_MEAN'
    REPO_POW_SUM = 'REPO_POW_SUM'
    REPO_SD = 'REPO_SD'
    REPO_SUM = 'REPO_SUM'
