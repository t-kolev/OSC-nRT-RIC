#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<string>
#include<iostream>
#include<fstream>

#include"json.h"
#include"schemaparser.h"

#include <sdl/syncstorage.hpp>

//	data type definitions from sdl
using Namespace = std::string;
using Key = std::string;
using Data = std::vector<uint8_t>;
using DataMap = std::map<Key, Data>;
using Keys = std::set<Key>;


using namespace std;
using namespace mc_schema;

vector<uint8_t> packData(const char *d, int len){
	const uint8_t *d8 = (const uint8_t *)d;
	return Data(d8, d8+len+1);
}

int main(int argc, char **argv){
	Namespace ns("mcnib");	

	string directory = ".";
	if(argc>1){
		directory = argv[1];
	}
	string inflnm = directory + "/" + string("nib.json");

	ifstream infl(inflnm);
	if(!infl){
		cerr << "Error, can't open " << inflnm << endl;
		exit(1);
	}
	string line;
	string nib_str;
	while(getline(infl, line)){
		nib_str += line;
	}
	infl.close();


	mc_schemas *mcs = new_mc_schemas(nib_str);
	if(mcs->has_errors()){	// ensure its a good schema
		fprintf(stderr, "Errors loading the schemas:\n%s\n",mcs->get_errors().c_str());
		exit(1);
	}
	string streams_json = "[";
	vector<string> streams = mcs->get_streams();
	for(int i=0;i<streams.size(); ++i){
		if(i>0) streams_json += ", ";
		streams_json += "\""+streams[i]+"\"";
	}
	streams_json += "]";
	
// ---------------------------------------------
	std::unique_ptr<shareddatalayer::SyncStorage> sdl(shareddatalayer::SyncStorage::create());
	DataMap D;

// --------------------------------------------

	vector<uint8_t> schema_buf = packData(nib_str.c_str(), nib_str.size());
	D["_schema"] = schema_buf;

	vector<uint8_t> streams_buf = packData(streams_json.c_str(), streams_json.size());
	D["_schema_streams"] = streams_buf;

	sdl->set(ns, D);
}
