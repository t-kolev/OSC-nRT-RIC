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

    def loadModel(self):
        model = None
        with open(self.loadFile(), 'rb') as file:
            model = pickle.load(file)
        return model