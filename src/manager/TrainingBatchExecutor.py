import numpy as np
import pandas as pd

from .CustomFileRepository import CustomFileRepository

from ..utils import Util
from ..utils.constants import Constants

log = Util.setup_logger()

class TrainingBatchExecutor:

    def getData(self):
        return CustomFileRepository.scaler_dictionary
    
    def reset(self):
        CustomFileRepository.scaler_dictionary = {}
        
    def updateScalars(self, cell, df_current):
        log.info('Update scalar started for [{}]'.format(cell))

        df_current.set_index(Constants.KPIS_INDEXER, inplace=True)

        if CustomFileRepository.scaler_dictionary.get(cell) is None:
            log.info('Dictionary not contains cell [{}].'.format(cell))
            
            df_sum = df_current.groupby(Constants.KPIS_INDEXER).sum()
            log.debug('df_sum content:')
            Util.log_dataframe(df_sum)
            
            df_powsum = df_current.groupby(Constants.KPIS_INDEXER).apply(lambda x: np.square(x).sum())
            log.debug('df_powsum content:')
            Util.log_dataframe(df_powsum)

            df_count = pd.DataFrame(df_current.groupby(Constants.KPIS_INDEXER).apply(lambda x: len(x)))
            df_count.columns = ['count']
            log.debug('df_count content:')
            Util.log_dataframe(df_count)

            df_org_cumsum = df_sum
            df_org_powsum = df_powsum
            df_org_count = df_count
        else :
           if CustomFileRepository.scaler_dictionary.get(cell).get(Constants.REPO_COUNT) is not None:
                log.info('Dictionary contains cell [{}].'.format(cell))

                df_org_count = CustomFileRepository.scaler_dictionary.get(cell).get(Constants.REPO_COUNT)
                log.debug('df_org_count: ')
                Util.log_dataframe(df_org_count)

                df_org_cumsum = CustomFileRepository.scaler_dictionary.get(cell).get(Constants.REPO_SUM)
                log.debug('df_org_cumsum: ')
                Util.log_dataframe(df_org_cumsum)

                df_org_powsum = CustomFileRepository.scaler_dictionary.get(cell).get(Constants.REPO_POW_SUM)
                log.debug('df_org_powsum: ')
                Util.log_dataframe(df_org_powsum)

                df_org_count.set_index(Constants.KPIS_INDEXER, inplace=True)
                df_org_cumsum.set_index(Constants.KPIS_INDEXER, inplace=True)
                df_org_powsum.set_index(Constants.KPIS_INDEXER, inplace=True)

                df_sum = df_current.groupby(Constants.KPIS_INDEXER).sum()
                log.debug('df_sum: ')
                Util.log_dataframe(df_sum)

                df_powsum = df_current.groupby(Constants.KPIS_INDEXER).apply(lambda x: np.square(x).sum())
                log.debug('df_powsum: ')
                Util.log_dataframe(df_powsum)

                df_count = pd.DataFrame(df_current.groupby(Constants.KPIS_INDEXER).apply(lambda x: len(x)))
                df_count.columns = ['count']
                log.debug('df_count: ')
                Util.log_dataframe(df_count)

                df_org_count_all_idx = df_org_count.add(df_count)
                df_org_count_all_idx.loc[:, :] = 0
                df_org_cumsum_all_idx = df_org_cumsum.add(df_sum)
                df_org_cumsum_all_idx.loc[:, :] = 0
                df_org_powsum_all_idx = df_org_powsum.add(df_powsum)
                df_org_powsum_all_idx.loc[:, :] = 0

                df_org_count = df_org_count.reindex(df_org_count_all_idx.index, fill_value=0)
                df_org_cumsum = df_org_cumsum.reindex(df_org_cumsum_all_idx.index, fill_value=0)
                df_org_powsum = df_org_powsum.reindex(df_org_powsum_all_idx.index, fill_value=0)
                df_count = df_count.reindex(df_org_count_all_idx.index, fill_value=0)
                df_cumsum = df_sum.reindex(df_org_cumsum_all_idx.index, fill_value=0)
                df_powsum = df_powsum.reindex(df_org_powsum_all_idx.index, fill_value=0)

                df_org_count = df_org_count.add(df_count)
                log.debug('df_org_count: ')
                Util.log_dataframe(df_org_count)

                df_org_cumsum = df_org_cumsum.add(df_cumsum)
                log.debug('df_org_cumsum: ')
                Util.log_dataframe(df_org_cumsum)

                df_org_powsum = df_org_powsum.add(df_powsum)
                log.debug('df_org_powsum: ')
                Util.log_dataframe(df_org_powsum)

        df_org_mean = pd.DataFrame(df_org_cumsum.values / df_org_count.values, index=df_org_cumsum.index,
                                       columns=df_org_cumsum.columns)
        log.debug('df_org_mean: ')
        Util.log_dataframe(df_org_mean)
        
        df_org_var = (df_org_powsum - (df_org_cumsum * df_org_mean)) / (df_org_count.values - 1)
        log.debug('df_org_var: ')
        Util.log_dataframe(df_org_var)

        df_org_sd = np.sqrt(df_org_var)
        log.debug('df_org_sd: ')
        Util.log_dataframe(df_org_sd)

        df_org_count =  df_org_count.fillna(0).reset_index()
        log.debug('df_org_count: ')
        Util.log_dataframe(df_org_count)

        df_org_cumsum = df_org_cumsum.fillna(0).reset_index()
        log.debug('df_org_cumsum: ')
        Util.log_dataframe(df_org_cumsum)

        df_org_mean = df_org_mean.fillna(0).reset_index()
        log.debug('df_org_mean: ')
        Util.log_dataframe(df_org_mean)

        df_org_powsum = df_org_powsum.fillna(0).reset_index()
        log.debug('df_org_powsum: ')
        Util.log_dataframe(df_org_powsum)

        df_org_sd =  df_org_sd.fillna(0).reset_index()
        log.debug('df_org_sd: ')
        Util.log_dataframe(df_org_sd)

        CustomFileRepository.scaler_dictionary[cell] =  {Constants.REPO_SUM: df_org_cumsum, Constants.REPO_MEAN: df_org_mean, Constants.REPO_SD: df_org_sd, 
                                           Constants.REPO_COUNT: df_org_count, Constants.REPO_POW_SUM: df_org_powsum}
        log.debug('Dictionary after scaler update[{}].'.format(CustomFileRepository.scaler_dictionary))
        
        df_current.reset_index(inplace=True)