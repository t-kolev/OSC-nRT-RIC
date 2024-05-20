#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<string>
#include<iostream>
#include<fstream>

#include"schemaparser.h"

#include <sdl/syncstorage.hpp>

using namespace std;

//	data type definitions from sdl
using Namespace = std::string;
using Key = std::string;
using Data = std::vector<uint8_t>;
using DataMap = std::map<Key, Data>;
using Keys = std::set<Key>;

int main(int argc, char **argv){

//		MCNIB setup
	Namespace ns("mcnib");	
	std::unique_ptr<shareddatalayer::SyncStorage> sdl(shareddatalayer::SyncStorage::create());


	if(argc< 2){
		fprintf(stderr,"Error, usage is %s schema [prefix] [directory]\n", argv[0]);
		exit(1);
	}
	string schema = argv[1];
	string prefix("");
	if(argc>2)
		prefix = argv[2];

//		Get the nib.json file
	string nib_str;

	if(argc>3){
		string directory = argv[3];
	
		string inflnm = directory + "/" + string("nib.json");

		ifstream infl(inflnm);
		if(!infl){
			cerr << "Error, can't open " << inflnm << endl;
			exit(1);
		}
		string line;
		while(getline(infl, line)){
			nib_str += line;
		}
		infl.close();
	}else{
//fprintf(stderr,"Fetching from sdl\n");
		Key k = "_schema";
		Keys K;
		K.insert(k);
		DataMap Dk = sdl->get(ns, K);
		if(Dk.count(k)>0){
			vector<uint8_t> val_v = Dk[k];
			char val[val_v.size()+1];				// from Data
			int i;
			for(i=0;i<val_v.size();++i) val[i] = (char)(val_v[i]);
			val[i]='\0';
			nib_str = val;
		}
	}

//		Load the schemas, get the representation for the desired schema
	mc_schema::mc_schemas *mcs = mc_schema::new_mc_schemas(nib_str);
	if(mcs->has_errors()){	// TODO some schemas might have been loaded
		fprintf(stderr, "Errors loading the schemas:\n%s\n",mcs->get_errors().c_str());
		exit(1);
	}
	mc_schema::query_rep *qr = mcs->get_query_rep(schema);
	if(qr==NULL){
		fprintf(stderr,"Error, schema %s not found, available schemas are:\n",schema.c_str());
		vector<string> streams = mcs->get_streams();
		for(int i=0;i<streams.size(); ++i){
			printf("\t%s\n",streams[i].c_str());
		}
		exit(1);
	}

//	Get the table representation
	vector<string> fields;
	vector<mc_schema::field_handle> handles;
	int n_fields = qr->get_num_fields();
	for(int i=0;i<n_fields;++i){
		fields.push_back(qr->get_field_name(i));
		handles.push_back(qr->get_handle_of_field(fields[i]));
	}

//	CSV header
	printf("Key_");
	for(int i=0;i<n_fields;++i){
		printf(",%s",fields[i].c_str());
	}
	printf("\n");

//		
	string pk =  schema+":"+prefix;
	Keys K = sdl->findKeys(ns, pk);	// prefix keys in schema
	
	DataMap Dk = sdl->get(ns, K);
	for(auto si=K.begin();si!=K.end();++si){

		std::vector<uint8_t> val_v = Dk[(*si)]; // 5 lines to unpack a string
		int len = val_v.size();
		char val[len];				// from Data
		for(int i=0;i<val_v.size();++i)
			val[i] = (char)(val_v[i]);

		printf("%s",(*si).c_str());
		for(int i=0;i<n_fields;++i){
			string s = mc_schema::get_field_string(handles[i], val, len);
			printf(",%s",s.c_str());
		}
		printf("\n");
	}

	delete mcs;

}
