import configparser
import os
import time
import influxdb_client
import pandas as pd

from .DetectionExecutor import DetectionExecutor
from .TrainingBatchExecutor import TrainingBatchExecutor

from ..utils import Util
from ..utils.constants import Constants

config = configparser.ConfigParser()
config.read('/tmp/src/configuration/config.ini')

log = Util.setup_logger()

class InfluxDBManager:

    def getLatestData(self, measurement) -> pd.DataFrame:
        log.debug('InfluxDBManager.getLatestData :: getLatestData called')
        
        client = influxdb_client.InfluxDBClient(url = config.get('APP', 'INFLUX_URL'),
                                     token=config.get('APP', 'INFLUX_TOKEN'),
                                     org=config.get('APP', 'INFLUX_ORG'))
        query_api = client.query_api()

        INFLUXDB_BUCKET = config.get('APP', 'INFLUX_BUCKET')
        flux_query = f'''
        from(bucket: "{INFLUXDB_BUCKET}")
            |> range(start: -5s)  // Adjust the range as needed
            |> filter(fn: (r) => r["_measurement"] == "{measurement}")
            |> pivot(rowKey:["_time"], columnKey: ["_field"], valueColumn: "_value")
            |> sort(columns: ["_time"], desc: true)
            |> limit(n:1)
        '''

        try:
            df = query_api.query_data_frame(flux_query)
            return df
        except Exception as e:
            log.error(f"An error occurred while querying InfluxDB: {e}")
            return pd.DataFrame()
        finally:
            client.close() 

    def query(self):
        log.debug('InfluxDBManager.query :: query called')
        while True:
            measurement = config.get('APP', 'MEASUREMENT_NAME')
            log.debug('measurement name [{}]'.format(measurement))
            latestDataDF = self.getLatestData(measurement)

            if not latestDataDF.empty:
                log.info(f"Latest data from measurement: '{measurement}'")
                Util.log_dataframe(latestDataDF)

                latestDataDF  = latestDataDF.drop(['table', '_start', '_stop', '_time', '_measurement', 'result'], axis=1)

                trainingBatchExecutor = TrainingBatchExecutor()
                scaler_dictionary = trainingBatchExecutor.getData()

                if scaler_dictionary.get(latestDataDF.loc[0, 'Short name']) is None or scaler_dictionary.get(latestDataDF.loc[0, 'Short name']).get(Constants.REPO_COUNT).loc[0, 'count'] < int(config.get('APP', 'DETECTION_COUNT')):
                    log.info(f"No suffiecient data to detect anamoly for cell  '{latestDataDF.loc[0, 'Short name']}'")
                    trainingBatchExecutor.updateScalars(latestDataDF.loc[0, 'Short name'], latestDataDF)
                else:
                    log.info(f"Greater than 1 day suffiecient data is present to detect anamoly for cell  '{latestDataDF.loc[0, 'Short name']}'")
                    trainingBatchExecutor.updateScalars(latestDataDF.loc[0, 'Short name'], latestDataDF)
                    detectionExecutor = DetectionExecutor()
                    detectionExecutor.execute(latestDataDF)        
            else:
                log.info(f"No data available in measurement '{measurement}'.")
            time.sleep(5)


    def write(self, data):
        pass