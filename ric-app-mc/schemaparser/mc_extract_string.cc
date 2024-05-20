#include<stdlib.h>
#include<stdio.h>
#include<string.h>

#include <sdl/syncstorage.hpp>


//	data type definitions from sdl
using Namespace = std::string;
using Key = std::string;
using Data = std::vector<uint8_t>;
using DataMap = std::map<Key, Data>;
using Keys = std::set<Key>;

int main(int argc, char **argv){

	Namespace ns("mcnib");	
	std::string prefix = "";
	if(argc>1){
		prefix = argv[1];
	}

	std::unique_ptr<shareddatalayer::SyncStorage> sdl(shareddatalayer::SyncStorage::create());

	Keys K = sdl->findKeys(ns, prefix);	// just the prefix
	
	DataMap Dk = sdl->get(ns, K);
	for(auto si=K.begin();si!=K.end();++si){
		std::vector<uint8_t> val_v = Dk[(*si)]; // 4 lines to unpack a string
		char val[val_v.size()+1];				// from Data
		int i;
		for(i=0;i<val_v.size();++i) val[i] = (char)(val_v[i]);
		val[i]='\0';
		printf("%s = %s\n",(*si).c_str(), val);
	}

}
