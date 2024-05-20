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
	printf("Deleting:\n");
	for(auto si=K.begin();si!=K.end();++si){
		printf("\t%s\n",(*si).c_str());
	}

	sdl->remove(ns, K);

}
