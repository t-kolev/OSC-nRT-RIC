class CustomFileRepository:
    scaler_dictionary = {}

    def getRepoData(self, repoType):
        return self.__data_dict[repoType]