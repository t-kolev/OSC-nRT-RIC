import os
import pickle
import sys

import pandas as pd

from ..utils.constants import Constants


class FileLoader:
    def __init__(self, fileName, fileType=None):
        self.__fileName = fileName
        self.__fileType = fileType
        self.__packageName = Constants.CONFIG_DIR
    
    """
    Loads a file based on the specified package name, file name, and file type.

    This method checks for the existence of the file in the current working directory,
    and falls back to using the PyInstaller bundle directory if running in a frozen environment.
    It then reads the file based on its type (CSV or Excel) and returns the data as a Pandas DataFrame.
        
    If the file type is neither CSV nor Excel, it returns the file path as a string.

    Parameters:
    - self: The instance of the class itself.
        
    Returns:
    - A Pandas DataFrame containing the data from the loaded file if the file type is CSV or Excel.
    - The file path as a string if the file type is neither CSV nor Excel.
    """
    def loadFile(self):
        isNewConfigExists = False
        extDataDir = None
        if os.path.exists(os.path.join(os.getcwd(), self.__packageName)):
            if os.path.exists(os.path.join(os.getcwd(), self.__packageName, self.__fileName)):
                isNewConfigExists = True
                extDataDir = os.path.join(os.getcwd(), self.__packageName, self.__fileName)
        if not isNewConfigExists:
            if getattr(sys, 'frozen', False):
                # if you are running in a |PyInstaller| bundle
                extDataDir = sys._MEIPASS
                extDataDir = os.path.join(extDataDir, self.__packageName, self.__fileName)
                # you should use extDataDir as the path to your file Store_Codes.csv file
            else:
                # we are running in a normal Python environment
                extDataDir = os.getcwd()
                extDataDir = os.path.join(extDataDir, self.__packageName, self.__fileName)
                # you should use extDataDir as the path to your file Store_Codes.csv file
        if self.__fileType == 'csv':
            return pd.read_csv(extDataDir)
        elif self.__fileType == 'xlsx' or self.__fileType == 'xls':
            return pd.read_excel(extDataDir)
        return extDataDir

    """
    Loads a machine learning model from a specified file path.

    This method reads a serialized model from a file using pickle and returns the loaded model object.

    Parameters:
    - None (self.loadFile() is used to determine the file path).

    Return Value:
    - The loaded machine learning model object.
    """
    def loadModel(self):
        model = None
        with open(self.loadFile(), 'rb') as file:
            model = pickle.load(file)
        return model