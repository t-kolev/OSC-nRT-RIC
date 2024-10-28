import logging

"""
Set up a logger with INFO level that outputs messages to the console.

This function configures a logger instance with a StreamHandler that writes log messages to the console.
The log messages will include the timestamp, log level, logger name, and the actual message.

Returns: logging.Logger: A configured logger instance.
"""
def setup_logger():
    logger = logging.getLogger()
    logger.setLevel(logging.INFO)
    formatter = logging.Formatter('%(asctime)s [%(levelname)s] %(name)s: %(message)s')
    ch = logging.StreamHandler()
    ch.setFormatter(formatter)
    logger.addHandler(ch)
    return logger

log = setup_logger()

"""
Logs each cell of the given DataFrame to the logging system.

This function iterates over each column and row of the input DataFrame and logs the 
value of each cell using the 'log.debug' method. The format of the log message includes 
the column name and the corresponding cell value.

Parameters: df (pd.DataFrame): The DataFrame whose cells need to be logged.
"""
def log_dataframe(df):
    for column in df.columns:
        for row in range(len(df)):
            log.debug("{} : {}".format(column, df.iloc[row][column]))

"""
Retrieve the logger object configured for the application.

This function returns the logger instance that has been set up for logging purposes within the application.
It assumes that the logger 'log' has already been initialized elsewhere in the codebase.

Returns: logging.Logger: The logger object configured for the application.
"""
def get_logger():
    return log