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

struct vstring32 {
    unsigned int length;
    unsigned int offset;
    unsigned int reserved;
};


struct thpt{
	unsigned long long int TS;
	unsigned long long int e_RAB_ID;
	unsigned long long int UE_ID;
	vstring32 GNB_ID;
	double measurement_interval;
	unsigned long long int active_throughput;
	unsigned long long int average_throughput;
	unsigned long long int min_throughput;
	unsigned long long int max_throughput;
};
	

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
	if(mcs->has_errors()){
		fprintf(stderr, "Errors loading the schemas:\n%s\n",mcs->get_errors().c_str());
	}else{
		vector<string> streams = mcs->get_streams();
		printf("Loaded %ld streams:\n", streams.size());
		for(int i=0;i<streams.size(); ++i){
			string str_rep = mcs->get_query_rep(streams[i])->to_string();
			printf("\t%s\n",str_rep.c_str());
		}
	}
	query_rep *qr = mcs->get_query_rep("throughput_ue");
	
// ---------------------------------------------
	std::unique_ptr<shareddatalayer::SyncStorage> sdl(shareddatalayer::SyncStorage::create());
	DataMap D;

// --------------------------------------------

	char buf[150];
	thpt *t = (thpt *)buf;
	int t_len = sizeof(thpt)+6;

	t->TS = 10001;
	t->e_RAB_ID = 2;
	t->UE_ID = 3;
	t->GNB_ID.length = 6;
	t->GNB_ID.offset = sizeof(thpt);
	string foobar("foobar");
	strncpy(buf+sizeof(thpt), foobar.c_str(), 6);
	t->measurement_interval = 10.1;
	t->active_throughput = 4;
	t->average_throughput = 5;
	t->min_throughput = 6;
	t->max_throughput = 7;

	string key = "throughput_ue:"+to_string(t->UE_ID)+":"+foobar+":"+to_string(t->e_RAB_ID);
	vector<uint8_t> val1 = packData(buf,t_len);
	D[key] = val1;
	

	t->TS = 110001;
	t->e_RAB_ID = 12;
	t->UE_ID = 3;
	t->GNB_ID.length = 6;
	t->GNB_ID.offset = sizeof(thpt);
//	string foobar("foobar");
	foobar = "foobar";
	strncpy(buf+sizeof(thpt), foobar.c_str(), 6);
	t->measurement_interval = 110.1;
	t->active_throughput = 14;
	t->average_throughput = 15;
	t->min_throughput = 16;
	t->max_throughput = 17;

	key = "throughput_ue:"+to_string(t->UE_ID)+":"+foobar+":"+to_string(t->e_RAB_ID);
	vector<uint8_t> val2 = packData(buf,t_len);
	D[key] = val2;
	

	t->TS = 210001;
	t->e_RAB_ID = 12;
	t->UE_ID = 3;
	t->GNB_ID.length = 6;
	t->GNB_ID.offset = sizeof(thpt);
//	string foobar("foobaz");
	foobar = "foobaz";
	strncpy(buf+sizeof(thpt), foobar.c_str(), 6);
	t->measurement_interval = 210.1;
	t->active_throughput = 24;
	t->average_throughput = 25;
	t->min_throughput = 26;
	t->max_throughput = 27;

	key = "throughput_ue:"+to_string(t->UE_ID)+":"+foobar+":"+to_string(t->e_RAB_ID);
	vector<uint8_t> val3 = packData(buf,t_len);
	D[key] = val3;
	

	t->TS = 310001;
	t->e_RAB_ID = 12;
	t->UE_ID = 33;
	t->GNB_ID.length = 6;
	t->GNB_ID.offset = sizeof(thpt);
//	string foobar("foobar");
	foobar = "foobar";
	strncpy(buf+sizeof(thpt), foobar.c_str(), 6);
	t->measurement_interval = 310.1;
	t->active_throughput = 34;
	t->average_throughput = 35;
	t->min_throughput = 36;
	t->max_throughput = 37;

	key = "throughput_ue:"+to_string(t->UE_ID)+":"+foobar+":"+to_string(t->e_RAB_ID);
	vector<uint8_t> val4 = packData(buf,t_len);
	D[key] = val4;
	
	sdl->set(ns, D);
}
