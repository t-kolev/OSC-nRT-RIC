from scipy import stats

import pandas as pd

from ..utils import Util

log = Util.get_logger()

class Scaler:

    def calculate_score(self, df_kpi_value):
        z_scores = stats.zscore(df_kpi_value, axis=0)
        return pd.DataFrame(z_scores, columns=df_kpi_value.columns)