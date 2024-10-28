from scipy import stats

import pandas as pd

from ..utils import Util

log = Util.get_logger()

class Scaler:
    
    """
    Calculate the Z-scores for each column in the input DataFrame.

    This function computes the Z-score for each numerical column in the provided DataFrame,
    which standardizes the data by subtracting the mean and then dividing by the standard deviation.
    The resulting Z-scores represent how many standard deviations each value is from the mean.

    Parameters:
        df_kpi_value (pd.DataFrame): A pandas DataFrame containing the KPI values for which Z-scores need to be calculated.

    Returns:
        pd.DataFrame: A pandas DataFrame containing the Z-scores for each column of the input DataFrame.
    """
    def calculate_score(self, df_kpi_value):
        z_scores = stats.zscore(df_kpi_value, axis=0)
        return pd.DataFrame(z_scores, columns=df_kpi_value.columns)