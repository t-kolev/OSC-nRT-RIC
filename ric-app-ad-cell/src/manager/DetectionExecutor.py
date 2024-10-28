import numpy as np

from ..utils.constants import Constants
from ..utils.FileLoader import FileLoader
from ..utils import Util

from .Scaler import Scaler


log = Util.get_logger()


class DetectionExecutor:

    """
    Executes the detection process using the trained model on the given DataFrame.

     This method performs data scaling, extracts the latest data point, loads the pre-trained model,
    and makes a prediction based on the scaled data point.

    Parameters:
    df (pd.DataFrame): The input DataFrame containing the data to be processed.
    """
    def execute(self, df):
        log.info('In DetectionExecutor')

        df_scaled = self.data_scaling(df)

        X_test = np.array(df_scaled.iloc[[-1]].reset_index(drop=True))

        model = FileLoader(Constants.MODEL_AD_CELL_FILE_NAME).loadModel()
        log.info('Model Prediction [{}]'.format(model.predict(X_test)))

    """
    Scale the input dataframe using a Z-score for each cell.

    Parameters:
    df (pandas.DataFrame): The input dataframe containing the data to be scaled.

    Returns:
    pandas.DataFrame: A dataframe with scaled cell scores, with NaN values replaced by zeros.
    """
    def data_scaling(self, df):
        log.debug('input df:')
        Util.log_dataframe(df)

        scaler = Scaler()
        result_df_cell_scores  = scaler.calculate_score(df)
        result_df_cell_scores.fillna(0, inplace=True)
        log.debug('result_df_cell_scores.shape: {}'.format(result_df_cell_scores.shape))

        log.debug('result_df_cell_scores:')
        Util.log_dataframe(result_df_cell_scores)

        return result_df_cell_scores