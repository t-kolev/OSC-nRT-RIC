import logging

def setup_logger():
    logger = logging.getLogger()
    logger.setLevel(logging.DEBUG)
    formatter = logging.Formatter('%(asctime)s [%(levelname)s] %(name)s: %(message)s')
    ch = logging.StreamHandler()
    ch.setFormatter(formatter)
    logger.addHandler(ch)
    return logger

log = setup_logger()

def log_dataframe(df):
    for column in df.columns:
        for row in range(len(df)):
            log.debug("{} : {}".format(column, df.iloc[row][column]))

def get_logger():
    return log