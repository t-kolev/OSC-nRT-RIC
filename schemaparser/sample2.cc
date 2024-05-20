#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<string>
#include<iostream>
#include<fstream>

#include"schemaparser.h"

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

int main(int argc, char **argv){
//		Get the nib.json file
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

//		Load the schemas
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

//		Load a sample record
	char buf[150];
	thpt *t = (thpt *)buf;
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

	int t_len = sizeof(thpt)+6;
	
//		Get the throughput_ue schema
	query_rep *qr = mcs->get_query_rep("throughput_ue");

//		Extract stuff by various methods
	int ts_idx = qr->get_index_of_field("TS");
	int ts_type = qr->get_type(ts_idx);
	int ts_offset = qr->get_offset(ts_idx);
	field_handle ts_handle = qr->get_handle(ts_idx);

	string ts_s = get_field_string(ts_handle, buf, t_len);
	unsigned long long int ts_lu = get_field_ullong(ts_handle, buf, t_len);
	unsigned int ts_u = get_field_uint(ts_handle, buf, t_len);
	printf("ts string=%s, ullong=%lld, uint = %d\n",ts_s.c_str(), ts_lu, ts_u);

	field_handle erab_handle = qr->get_handle(1);
	access_result erab_ar = get_field_by_handle(erab_handle, buf, t_len);
	printf("erab = %lld\n", erab_ar.r.l);

	access_result ue_ar = get_field_by_index(qr, 2, buf, t_len);
	printf("ue = %lld\n", ue_ar.r.l);

	int gnb_idx = qr->get_index_of_field("GNB_ID");
	field_handle gnb_handle = qr->get_handle(gnb_idx);
	string gnb = get_field_string(gnb_handle, buf, t_len);
	printf("gnb=%s\n",gnb.c_str());

	access_result mt_ar = get_field_by_index(qr, 8, buf, t_len);
	printf("mt = %lld, type=%d\n", mt_ar.r.l, mt_ar.field_data_type);

	access_result none_ar = get_field_by_index(qr, 9, buf, t_len);
	printf("none = %lld, type=%d\n", none_ar.r.l, none_ar.field_data_type);

}
