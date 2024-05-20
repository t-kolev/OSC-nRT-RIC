/* ------------------------------------------------
Copyright 2020 AT&T Intellectual Property
   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
 ------------------------------------------- */

#include "schemaparser_impl.h"
#include<string.h>

namespace mc_schema{

int type_size(int dt){
	switch(dt){
	case INT_TYPE:
		return(sizeof(int));
	case UINT_TYPE:
	case USHORT_TYPE:
	case BOOL_TYPE:
	case IP_TYPE:
		return(sizeof(unsigned int));
	case ULLONG_TYPE:
		return(sizeof(unsigned long long int));
	case LLONG_TYPE:
		return(sizeof(long long int));
	case FLOAT_TYPE:
		return(sizeof(double));
	case IPV6_TYPE:
		return(sizeof(mc_ipv6_str));
	case TIMEVAL_TYPE:
		return(sizeof(timeval));
	case VSTR_TYPE:
		return(sizeof(mc_string));	
	default:
		return(UNDEFINED_TYPE);
	}
	return(UNDEFINED_TYPE);
};

int assign_type_from_string(std::string st){
//		convert to upper case
	std::for_each(st.begin(), st.end(), [](char & c) {
		c = ::toupper(c);
	});

	if(st == "BOOL"){
		return BOOL_TYPE;
	}
	if(st == "USHORT"){
		return USHORT_TYPE;
	}
	if(st == "UINT"){
		return UINT_TYPE;
	}
	if(st == "INT"){
		return INT_TYPE;
	}
	if(st == "ULLONG"){
		return ULLONG_TYPE;
	}
	if(st == "LLONG"){
		return LLONG_TYPE;
	}
	if(st == "FLOAT"){
		return FLOAT_TYPE;
	}
	if(st == "STRING" || st == "V_STR"){
		return VSTR_TYPE;		
	}
	if(st == "TIMEVAL"){
		return TIMEVAL_TYPE;
	}
	if(st == "IP"){
		return IP_TYPE;
	}
	if(st == "IPV6"){
		return IPV6_TYPE;
	}
	return 0;
}

std::string type_to_string(int typ){
	switch(typ){
	case BOOL_TYPE:
		return "BOOL";
	case USHORT_TYPE:
		return "USHORT";
	case UINT_TYPE:
		return "UINT";
	case INT_TYPE:
		return "INT";
	case ULLONG_TYPE:
		return  "ULLONG";
	case LLONG_TYPE:
		return "LLONG";
	case FLOAT_TYPE:
		return "FLOAT";
	case VSTR_TYPE:
		return "STRING";
	case TIMEVAL_TYPE:
		return "TIMEVAL";
	case IP_TYPE:
		return "IP";
	case IPV6_TYPE:
		return "IPV6_TYPE";
	default:
		return "UNDEFINED";
	}
	return "UNDEFINED";
}
	
//		there is a casting function
bool is_castable(int src, int dst){
	switch(src){
	case BOOL_TYPE:
		switch(dst){
		case BOOL_TYPE:
		case USHORT_TYPE:
		case UINT_TYPE:
		case INT_TYPE:
		case ULLONG_TYPE:
		case LLONG_TYPE:
		case FLOAT_TYPE:
		case VSTR_TYPE:
			return true;
		default:
			return false;
		}
	case USHORT_TYPE:
		switch(dst){
		case BOOL_TYPE:
		case USHORT_TYPE:
		case UINT_TYPE:
		case INT_TYPE:
		case ULLONG_TYPE:
		case LLONG_TYPE:
		case FLOAT_TYPE:
		case VSTR_TYPE:
			return true;
		default:
			return false;
		}
	case UINT_TYPE:
		switch(dst){
		case BOOL_TYPE:
		case USHORT_TYPE:
		case UINT_TYPE:
		case INT_TYPE:
		case ULLONG_TYPE:
		case LLONG_TYPE:
		case FLOAT_TYPE:
		case VSTR_TYPE:
			return true;
		default:
			return false;
		}
	case INT_TYPE:
		switch(dst){
		case BOOL_TYPE:
		case USHORT_TYPE:
		case UINT_TYPE:
		case INT_TYPE:
		case ULLONG_TYPE:
		case LLONG_TYPE:
		case FLOAT_TYPE:
		case VSTR_TYPE:
			return true;
		default:
			return false;
		}
	case ULLONG_TYPE:
		switch(dst){
		case BOOL_TYPE:
		case USHORT_TYPE:
		case UINT_TYPE:
		case INT_TYPE:
		case ULLONG_TYPE:
		case LLONG_TYPE:
		case FLOAT_TYPE:
		case VSTR_TYPE:
			return true;
		default:
			return false;
		}
	case LLONG_TYPE:
		switch(dst){
		case BOOL_TYPE:
		case USHORT_TYPE:
		case UINT_TYPE:
		case INT_TYPE:
		case ULLONG_TYPE:
		case LLONG_TYPE:
		case FLOAT_TYPE:
		case VSTR_TYPE:
			return true;
		default:
			return false;
		}
	case FLOAT_TYPE:
		switch(dst){
		case FLOAT_TYPE:
		case VSTR_TYPE:
			return true;
		default:
			return false;
		}
	case VSTR_TYPE:
		switch(dst){
		case VSTR_TYPE:
			return true;
		default:
			return false;
		}
	case TIMEVAL_TYPE:
		switch(dst){
		case TIMEVAL_TYPE:
		case VSTR_TYPE:
			return true;
		default:
			return false;
		}
	case IP_TYPE:
		switch(dst){
		case IP_TYPE:
		case VSTR_TYPE:
			return true;
		default:
			return false;
		}
	case IPV6_TYPE:
		switch(dst){
		case IPV6_TYPE:
		case VSTR_TYPE:
			return true;
		default:
			return false;
		}
	default:
		return false;
	}
	return false;
}

//		cast without loss of information
bool is_compatible(int src, int dst){
	switch(src){
	case BOOL_TYPE:
		switch(dst){
		case BOOL_TYPE:
		case USHORT_TYPE:
		case UINT_TYPE:
		case INT_TYPE:
		case ULLONG_TYPE:
		case LLONG_TYPE:
		case FLOAT_TYPE:
		case VSTR_TYPE:
			return true;
		default:
			return false;
		}
	case USHORT_TYPE:
		switch(dst){
		case USHORT_TYPE:
		case UINT_TYPE:
		case INT_TYPE:
		case ULLONG_TYPE:
		case LLONG_TYPE:
		case FLOAT_TYPE:
		case VSTR_TYPE:
			return true;
		default:
			return false;
		}
	case UINT_TYPE:
		switch(dst){
		case UINT_TYPE:
		case INT_TYPE:
		case ULLONG_TYPE:
		case LLONG_TYPE:
		case FLOAT_TYPE:
		case VSTR_TYPE:
			return true;
		default:
			return false;
		}
	case INT_TYPE:
		switch(dst){
		case INT_TYPE:
		case ULLONG_TYPE:
		case LLONG_TYPE:
		case FLOAT_TYPE:
		case VSTR_TYPE:
			return true;
		default:
			return false;
		}
	case ULLONG_TYPE:
		switch(dst){
		case ULLONG_TYPE:
		case LLONG_TYPE:
		case FLOAT_TYPE:
		case VSTR_TYPE:
			return true;
		default:
			return false;
		}
	case LLONG_TYPE:
		switch(dst){
		case LLONG_TYPE:
		case FLOAT_TYPE:
		case VSTR_TYPE:
			return true;
		default:
			return false;
		}
	case FLOAT_TYPE:
		switch(dst){
		case FLOAT_TYPE:
		case VSTR_TYPE:
			return true;
		default:
			return false;
		}
	case VSTR_TYPE:
		switch(dst){
		case VSTR_TYPE:
			return true;
		default:
			return false;
		}
	case TIMEVAL_TYPE:
		switch(dst){
		case TIMEVAL_TYPE:
		case VSTR_TYPE:
			return true;
		default:
			return false;
		}
	case IP_TYPE:
		switch(dst){
		case IP_TYPE:
		case VSTR_TYPE:
			return true;
		default:
			return false;
		}
	case IPV6_TYPE:
		switch(dst){
		case IPV6_TYPE:
		case VSTR_TYPE:
			return true;
		default:
			return false;
		}
	default:
		return false;
	}
	return false;
}

//////////////////////////////////////////////////////
//		Field_entry

std::string field_entry::load_from_json(mc_json::json_value *froot){
	this->init();
	for(mc_json::json_value *fparts=froot->first_child; fparts!=NULL; fparts=fparts->next_sibling){
		if(strcmp(fparts->name, "name")==0){
			if(fparts->type!=mc_json::JSON_STRING){
				return "Error, the name of a field must be a string ("+std::to_string(fparts->type)+")";
			}
			name = fparts->string_value;
		}
		if(strcmp(fparts->name, "type")==0){
			if(fparts->type!=mc_json::JSON_STRING){
				return "Error, the type of a field must be a string ("+std::to_string(fparts->type)+")";
			}
			type = fparts->string_value;
		}
		if(strcmp(fparts->name, "pos")==0){
			if(fparts->type!=mc_json::JSON_STRING){
				return "Error, the pos of a field must be an int ("+std::to_string(fparts->type)+")";
			}
			pos = atoi(fparts->string_value);
		}
		if(strcmp(fparts->name, "is_ts")==0){
			if(fparts->type!=mc_json::JSON_BOOL){
				return "Error, the is_ts of a field must be a bool ("+std::to_string(fparts->type)+")";
			}
			is_ts = fparts->int_value;
		}
	}
	return "";
}

	
	


int tuple_access_info::init(field_entry *fe){
	field_name = fe->name;
	pdt = assign_type_from_string(fe->type);
	is_ts = fe->is_ts;
	offset = 0;
	return(pdt);
};

std::string tuple_access_info::to_string(){
	std::string ret = field_name+": offset="+std::to_string(offset)+", type="+type_to_string(pdt);
	if(is_ts){
		ret+=", is_ts";
	}
	return ret;
}

//////////////////////////////////////////////////////////////////
//		query_rep implementation
/////////////////////////////////////////////////////////////////


std::string _query_rep::init(const mc_json::json_value *strm){
	min_tuple_size = 0;

	if(strm->type != mc_json::JSON_OBJECT){
		return "Error, json stream must be a dict ("+std::to_string(strm->type)+").";
	}
	this->name = strm->name;
	for(mc_json::json_value *sparts=strm->first_child; sparts!=NULL; sparts=sparts->next_sibling){
//keys
		if(strcmp(sparts->name, "keys")==0){
			if(sparts->type != mc_json::JSON_ARRAY){
				return "Error, the keys in stream must be an array (" +std::to_string(sparts->type)+ ").";
				}
				for(mc_json::json_value *kparts=sparts->first_child; kparts!=NULL; kparts=kparts->next_sibling){
					if(kparts->type != mc_json::JSON_STRING){
						return "Error, the key entries in a stream must all be strings (" +std::to_string(kparts->type)+").";
					}
					keys.push_back(kparts->string_value);
				}
			}
// fields
		if(strcmp(sparts->name, "fields")==0){
			if(sparts->type != mc_json::JSON_ARRAY){
				return "Error, the fields in a stream must all be dict (" +std::to_string(sparts->type )+ ").";
			}
			for(mc_json::json_value *field=sparts->first_child; field!=NULL; field=field->next_sibling){
				if(field->type != mc_json::JSON_OBJECT){
					return "Error, the field entries in a stream must all be dicts (" +std::to_string(field->type )+ ").";
				}
				field_entry fe;
				std::string err = fe.load_from_json(field);
				if(err!=""){
					return "Error loading stream "+std::string(this->name)+": "+err;
				}
				tuple_access_info tai;
				int pdt = tai.init(&fe);
				if(pdt==UNDEFINED_TYPE){
					err = "Error in field "+fe.name+" of stream "+this->name+", unknown type "+fe.type;
					return err;
				}
				field_info.push_back(tai);
			}
		}
	}

	return "";
}


int _query_rep::finalize(){
	int f;
	int curr_pos = 0;
	int fld_undefined = 0;
	for(f=0;f<field_info.size();++f){
		field_info[f].offset = curr_pos;
		int sz = type_size(field_info[f].pdt);
		if(sz==0) fld_undefined = 1;
		curr_pos += sz;
	}
	min_tuple_size = curr_pos;
	if(fld_undefined) return(-1);
	else return(0);
}

//		Return a text representation
std::string _query_rep::to_string(){
	std::string ret = name + ": keys=[";
	for(int k=0;k<keys.size();++k){
		if(k>0)
			ret+=",";
		ret += keys[k];
	}
	ret += "], fields=\n";
	for(int f=0;f<field_info.size();++f){
		ret += field_info[f].to_string()+"\n";
	}
	ret+="\tmin_size="+std::to_string(min_tuple_size)+"\n";
	return ret;
}

//		Number of fields
int  _query_rep::get_num_fields(){
	return field_info.size();
}

//		name of ith field (starting at zero)
std::string _query_rep::get_field_name(int i){
	return field_info[i].field_name;
}

//		lookup field index by name, -1 if not found
int _query_rep::get_index_of_field(std::string name){
	for(int i=0;i<field_info.size();++i){
		if(field_info[i].field_name==name)
			return i;
	}
	return -1;
}

//		lookup field handle by name, offset is -1 if not found
field_handle _query_rep::get_handle_of_field(std::string name){
	field_handle ret;
	ret.offset = -1;
	for(int i=0;i<field_info.size();++i){
		if(field_info[i].field_name==name){
			ret.offset = field_info[i].offset;
			ret.type = field_info[i].pdt;
			return ret;
		}
	}
	return ret;
}


//		data type of ith field.
int _query_rep::get_type(int i){
	if(i<0 || i>=field_info.size())
		return UNDEFINED_TYPE;
	return field_info[i].pdt;
}

std::string _query_rep::get_type_name(int i){
	return type_to_string(this->get_type(i));
}

//		byte offset of ith field in a data block
int _query_rep::get_offset(int i){
	if(i<0 || i>field_info.size())
		return 0;
	return field_info[i].offset;
}

//		byte offset of ith field in a data block
field_handle _query_rep::get_handle(int i){
	field_handle ret;
	ret.offset = -1;
	if(i<0 || i>field_info.size())
		return ret;
	ret.offset = field_info[i].offset;
	ret.type = field_info[i].pdt;
	return ret;
}




//////////////////////////////////////////////////////////////////
//	mc_schemas implementation

//		n is a char buffer holding the json description of the stream schemas
_mc_schemas::_mc_schemas(const char *n) : mc_schemas() {
	this->init(n);
}
//		n is a string holding the json description of the stream schemas
_mc_schemas::_mc_schemas(std::string s){
	this->init(s.c_str());
}

void _mc_schemas::init(const char *n){
	nib_str = strdup(n);

	char *errorPos = 0;
	char *errorDesc = 0;
	int errorLine = 0;
	mc_json::block_allocator allocator(1 << 10); // 1 KB per block
	mc_json::json_value *jroot = mc_json::json_parse(nib_str, &errorPos, (const char**)&errorDesc, &errorLine, &allocator);

	if(jroot->type != mc_json::JSON_OBJECT){
		err =  "Error, root of the nib json must be a dict.";
		return ; 
	}

	for(mc_json::json_value *stream=jroot->first_child; stream!=NULL; stream=stream->next_sibling){
		if(stream->type != mc_json::JSON_OBJECT){
			err = "Error, the streams in the nib json must all be dict (" +std::to_string(stream->type )+ ").";
			return ;
		}
		std::string stream_name = stream->name;
		_query_rep *qr = new _query_rep();
		std::string stream_err = qr->init(stream);
		if(stream_err == ""){
			qr->finalize();
			qreps[stream_name] = qr;
		}else{
			qerrs[stream_name] = stream_err;
			delete qr;
		}
	}
}

//		true if there are any errors.
bool _mc_schemas::has_errors(){
	if(err!="")
		return true;
	if(qerrs.size()>0)
		return true;
	return false;
}

//		string with the error reports.  empty if there is no error.
std::string _mc_schemas::get_errors(){
	std::string ret = this->err;
	for(auto mi=qerrs.begin(); mi!=qerrs.end(); ++mi){
		if(err!="")
			err += "\n";
		err += (*mi).first+": "+(*mi).second;
	}
	return err;
}

//		return the names of the parsed streams.
std::vector<std::string> _mc_schemas::get_streams(){
	std::vector<std::string> ret;
	for(auto mi=qreps.begin(); mi!=qreps.end(); ++mi){
		ret.push_back(mi->first);
	}
	return ret;
}

//		return the names of the unsucessfully parsed streams
std::vector<std::string> _mc_schemas::get_error_streams(){
	std::vector<std::string> ret;
	for(auto mi=qerrs.begin(); mi!=qerrs.end(); ++mi){
		ret.push_back(mi->first);
	}
	return ret;
}

//		true if some stream was parsed successful or not
bool _mc_schemas::stream_found(std::string s){
	if(qreps.count(s)+qerrs.count(s) > 0)
		return true;
	return false;
}

//		true if there is a stream with name s which parsed successfully
bool _mc_schemas::stream_ok(std::string s){
	return qreps.count(s) > 0;
}

//		return the error associated with a stream, if any.
std::string _mc_schemas::get_stream_error(std::string s){
	if(qerrs.count(s)>0)
		return qerrs[s];
	if(qreps.count(s)>0)
		return "";
	return "not_found";
}

//		Get the query representation of a successfully parsed stream
query_rep *_mc_schemas::get_query_rep(std::string s){
	if(qreps.count(s)>0)
		return qreps[s];
	return NULL;
}

//	Destructor
_mc_schemas::~_mc_schemas(){
	free(nib_str);
	for(auto q=qreps.begin(); q!=qreps.end();++q){
		delete q->second;
	}
}


////////////
//	mc_schemas factory
mc_schemas *new_mc_schemas(std::string s){
	return new _mc_schemas(s);
}
mc_schemas *new_mc_schemas(const char *s){
	return new _mc_schemas(s);
}


////////////////////////////////////////////
//			Direct tuple access functions.

unsigned int unpack_uint(void *data, int len, int offset, int *problem){
	unsigned int retval;
	if(offset+sizeof(unsigned int) > len){
		*problem = 1;
		return(0);
	}
	memcpy(&retval, ((char *)data)+offset, sizeof(unsigned int));
	return(retval);
}
unsigned int unpack_ushort(void *data, int len, int offset, int *problem){
	unsigned int retval;
	if(offset+sizeof(unsigned int) > len){
		*problem = 1;
		return(0);
	}
	memcpy(&retval, ((char *)data)+offset, sizeof(unsigned int));
	return(retval);
}
unsigned int unpack_bool(void *data, int len, int offset, int *problem){
	unsigned int retval;
	if(offset+sizeof(unsigned int) > len){
		*problem = 1;
		return(0);
	}
	memcpy(&retval, ((char *)data)+offset, sizeof(unsigned int));
	return(retval);
}
int unpack_int(void *data, int len, int offset, int *problem){
	int retval;
	if(offset+sizeof(int) > len){
		*problem = 1;
		return(0);
	}
	memcpy(&retval, ((char *)data)+offset, sizeof(unsigned int));
	return(retval);
}
unsigned long long int unpack_ullong(void *data, int len, int offset, int *problem){
	unsigned long long int retval;
	if(offset+sizeof(unsigned long long int) > len){
		*problem = 1;
		return(0);
	}
	memcpy(&retval, ((char *)data)+offset, sizeof(unsigned long long int));
	return(retval);
}
long long int unpack_llong(void *data, int len, int offset, int *problem){
	long long int retval;
	if(offset+sizeof(long long int) > len){
		*problem = 1;
		return(0);
	}
	memcpy(&retval, ((char *)data)+offset, sizeof(long long int));
	return(retval);
}
double unpack_float(void *data, int len, int offset, int *problem){
	double retval;
	if(offset+sizeof(double) > len){
		*problem = 1;
		return(0);
	}
	memcpy(&retval, ((char *)data)+offset, sizeof(double));
	return(retval);
}
timeval unpack_timeval(void *data, int len, int offset, int *problem){
	timeval retval;
	if(offset+sizeof(timeval) > len){
		*problem = 1;
		retval.tv_sec = 0;
		retval.tv_usec = 0;
		return(retval);
	}
	memcpy(&retval, ((char *)data)+offset, sizeof(timeval));
	return(retval);
}
mc_string unpack_vstr(void *data,  int len, int offset, int *problem){
	mc_string retval;
	vstring32 unpack_s;

	if(offset+sizeof( vstring32) > len){
		*problem = 1;
		return(retval);
	}

	memcpy(&unpack_s, ((char *)data)+offset, sizeof(vstring32));

	retval.length = unpack_s.length;

	if(unpack_s.offset + retval.length > len){
		*problem = 1;
		return(retval);
	}
	retval.data = (char *)data + unpack_s.offset;
	return(retval);
}

struct mc_ipv6_str unpack_ipv6(void *data,  int len, int offset, int *problem){
        struct mc_ipv6_str retval;
        if(offset+sizeof(mc_ipv6_str) > len){
        	*problem = 1;
        	return(retval);
        }
        memcpy(&retval, ((char *)data)+offset, sizeof(mc_ipv6_str));
        return(retval);
}


access_result get_field_by_index(query_rep *qr, int index,
			void * data, int len){
	access_result retval;
	retval.field_data_type = UNDEFINED_TYPE;
	int problem = 0;

	if(index >= qr->get_num_fields()){
		return(retval);
	}

	switch(qr->get_type(index)){
	case UINT_TYPE:
		retval.r.ui = unpack_uint(data,  len,
			qr->get_offset(index), &problem);
		if(!problem) retval.field_data_type = UINT_TYPE;
		break;
	case IP_TYPE:
		retval.r.ui = unpack_uint(data,  len,
			qr->get_offset(index), &problem);
		if(!problem) retval.field_data_type = IP_TYPE;
		break;
	case INT_TYPE:
		retval.r.i = unpack_int(data,  len,
			qr->get_offset(index), &problem);
		if(!problem) retval.field_data_type = INT_TYPE;
		break;
	case ULLONG_TYPE:
		retval.r.ul = unpack_ullong(data,  len,
			qr->get_offset(index), &problem);
		if(!problem) retval.field_data_type = ULLONG_TYPE;
		break;
	case LLONG_TYPE:
		retval.r.l = unpack_llong(data,  len,
			qr->get_offset(index), &problem);
		if(!problem) retval.field_data_type = LLONG_TYPE;
		break;
	case USHORT_TYPE:
		retval.r.ui = unpack_ushort(data,  len,
			qr->get_offset(index), &problem);
		if(!problem) retval.field_data_type = USHORT_TYPE;
		break;
	case FLOAT_TYPE:
		retval.r.f = unpack_float(data,  len,
			qr->get_offset(index), &problem);
		if(!problem) retval.field_data_type = FLOAT_TYPE;
		break;
	case BOOL_TYPE:
		retval.r.ui = unpack_bool(data,  len,
			qr->get_offset(index), &problem);
		if(!problem) retval.field_data_type = BOOL_TYPE;
		break;
	case VSTR_TYPE:
		retval.r.vs = unpack_vstr(data,  len,
			qr->get_offset(index), &problem);
		if(!problem) retval.field_data_type = VSTR_TYPE;
		break;
	case TIMEVAL_TYPE:
		retval.r.t = unpack_timeval(data,  len,
			qr->get_offset(index), &problem);
		if(!problem) retval.field_data_type = TIMEVAL_TYPE;
		break;
	case IPV6_TYPE:
		retval.r.ip6 = unpack_ipv6(data,  len,
			qr->get_offset(index), &problem);
		if(!problem) retval.field_data_type = IPV6_TYPE;
		break;
	case UNDEFINED_TYPE:
		break;
	}
	return(retval);
}

access_result get_field_by_handle(field_handle f, void * data, int len){
	access_result retval;
	retval.field_data_type = UNDEFINED_TYPE;
	int problem = 0;

	switch(f.type){
	case UINT_TYPE:
		retval.r.ui = unpack_uint(data,  len, f.offset, &problem);
		if(!problem) retval.field_data_type = UINT_TYPE;
		break;
	case IP_TYPE:
		retval.r.ui = unpack_uint(data,  len, f.offset, &problem);
		if(!problem) retval.field_data_type = IP_TYPE;
		break;
	case INT_TYPE:
		retval.r.i = unpack_int(data,  len, f.offset, &problem);
		if(!problem) retval.field_data_type = INT_TYPE;
		break;
	case ULLONG_TYPE:
		retval.r.ul = unpack_ullong(data,  len, f.offset, &problem);
		if(!problem) retval.field_data_type = ULLONG_TYPE;
		break;
	case LLONG_TYPE:
		retval.r.l = unpack_llong(data,  len, f.offset, &problem);
		if(!problem) retval.field_data_type = LLONG_TYPE;
		break;
	case USHORT_TYPE:
		retval.r.ui = unpack_ushort(data,  len, f.offset, &problem);
		if(!problem) retval.field_data_type = USHORT_TYPE;
		break;
	case FLOAT_TYPE:
		retval.r.f = unpack_float(data,  len, f.offset, &problem);
		if(!problem) retval.field_data_type = FLOAT_TYPE;
		break;
	case BOOL_TYPE:
		retval.r.ui = unpack_bool(data,  len, f.offset, &problem);
		if(!problem) retval.field_data_type = BOOL_TYPE;
		break;
	case VSTR_TYPE:
		retval.r.vs = unpack_vstr(data,  len, f.offset, &problem);
		if(!problem) retval.field_data_type = VSTR_TYPE;
		break;
	case TIMEVAL_TYPE:
		retval.r.t = unpack_timeval(data,  len, f.offset, &problem);
		if(!problem) retval.field_data_type = TIMEVAL_TYPE;
		break;
	case IPV6_TYPE:
		retval.r.ip6 = unpack_ipv6(data,  len, f.offset, &problem);
		if(!problem) retval.field_data_type = IPV6_TYPE;
		break;
	case UNDEFINED_TYPE:
		break;
	}
	return(retval);
}

int get_field_int(field_handle f, void * data, int len){
	access_result a = get_field_by_handle(f, data, len);
	switch(f.type){
		case BOOL_TYPE:
		case USHORT_TYPE:
		case UINT_TYPE:
			return (int)(a.r.ui);
		case INT_TYPE:
		case IP_TYPE:
			return (int)(a.r.i);
		case LLONG_TYPE:
			return (int)(a.r.l);
		case ULLONG_TYPE:
			return (int)(a.r.ul);
		case FLOAT_TYPE:
		case VSTR_TYPE:
		case TIMEVAL_TYPE:
		case IPV6_TYPE:
			return 0;
		default:
			return 0;
	}
	return 0;
}

unsigned int get_field_uint(field_handle f, void * data, int len){
	access_result a = get_field_by_handle(f, data, len);
	switch(f.type){
		case BOOL_TYPE:
		case USHORT_TYPE:
		case UINT_TYPE:
			return (unsigned int)(a.r.ui);
		case INT_TYPE:
		case IP_TYPE:
			return (unsigned int)(a.r.i);
		case LLONG_TYPE:
			return (unsigned int)(a.r.l);
		case ULLONG_TYPE:
			return (unsigned int)(a.r.ul);
		case FLOAT_TYPE:
		case VSTR_TYPE:
		case TIMEVAL_TYPE:
		case IPV6_TYPE:
			return 0;
		default:
			return 0;
	}
	return 0;
}

long long int get_field_llong(field_handle f, void * data, int len){
	access_result a = get_field_by_handle(f, data, len);
	switch(f.type){
		case BOOL_TYPE:
		case USHORT_TYPE:
		case UINT_TYPE:
			return (long long int)(a.r.ui);
		case INT_TYPE:
		case IP_TYPE:
			return (long long int)(a.r.i);
		case LLONG_TYPE:
			return (long long int)(a.r.l);
		case ULLONG_TYPE:
			return (long long int)(a.r.ul);
		case FLOAT_TYPE:
		case VSTR_TYPE:
		case TIMEVAL_TYPE:
		case IPV6_TYPE:
			return 0;
		default:
			return 0;
	}
	return 0;
}
			
unsigned long long int get_field_ullong(field_handle f, void * data, int len){
	access_result a = get_field_by_handle(f, data, len);
	switch(f.type){
		case BOOL_TYPE:
		case USHORT_TYPE:
		case UINT_TYPE:
			return (unsigned long long int)(a.r.ui);
		case INT_TYPE:
		case IP_TYPE:
			return (unsigned long long int)(a.r.i);
		case LLONG_TYPE:
			return (unsigned long long int)(a.r.l);
		case ULLONG_TYPE:
			return (unsigned long long int)(a.r.ul);
		case FLOAT_TYPE:
		case VSTR_TYPE:
		case TIMEVAL_TYPE:
		case IPV6_TYPE:
			return 0;
		default:
			return 0;
	}
	return 0;
}

double get_field_float(field_handle f, void * data, int len){
	access_result a = get_field_by_handle(f, data, len);
	switch(f.type){
		case BOOL_TYPE:
		case USHORT_TYPE:
		case UINT_TYPE:
			return (unsigned long long int)(a.r.ui);
		case INT_TYPE:
		case IP_TYPE:
			return (unsigned long long int)(a.r.i);
		case LLONG_TYPE:
			return (unsigned long long int)(a.r.l);
		case ULLONG_TYPE:
			return (unsigned long long int)(a.r.ul);
		case FLOAT_TYPE:
		case VSTR_TYPE:
		case TIMEVAL_TYPE:
		case IPV6_TYPE:
			return 0;
		default:
			return 0;
	}
	return 0;
}
timeval get_field_timeval(field_handle f, void * data, int len){
	access_result a = get_field_by_handle(f, data, len);
	switch(f.type){
		case BOOL_TYPE:
		case USHORT_TYPE:
		case UINT_TYPE:
		case INT_TYPE:
		case IP_TYPE:
		case LLONG_TYPE:
		case ULLONG_TYPE:
		case FLOAT_TYPE:
		case VSTR_TYPE:
		case IPV6_TYPE:
			a.r.t.tv_sec = 0;
			a.r.t.tv_usec = 0;
			return a.r.t;
		case TIMEVAL_TYPE:
			return a.r.t;
		default:
			a.r.t.tv_sec = 0;
			a.r.t.tv_usec = 0;
			return a.r.t;
	}
	a.r.t.tv_sec = 0;
	a.r.t.tv_usec = 0;
	return a.r.t;
}

mc_ipv6_str get_field_ipv6(field_handle f, void * data, int len){
	access_result a = get_field_by_handle(f, data, len);
	switch(f.type){
		case BOOL_TYPE:
		case USHORT_TYPE:
		case UINT_TYPE:
		case INT_TYPE:
		case IP_TYPE:
		case LLONG_TYPE:
		case ULLONG_TYPE:
		case FLOAT_TYPE:
		case VSTR_TYPE:
		case TIMEVAL_TYPE:
			a.r.ip6.v[0] = 0;
			a.r.ip6.v[1] = 0;
			a.r.ip6.v[2] = 0;
			a.r.ip6.v[3] = 0;
			return a.r.ip6;
		case IPV6_TYPE:
			return a.r.ip6;
		default:
			a.r.ip6.v[0] = 0;
			a.r.ip6.v[1] = 0;
			a.r.ip6.v[2] = 0;
			a.r.ip6.v[3] = 0;
			return a.r.ip6;
	}
	a.r.ip6.v[0] = 0;
	a.r.ip6.v[1] = 0;
	a.r.ip6.v[2] = 0;
	a.r.ip6.v[3] = 0;
	return a.r.ip6;
}
	

std::string get_field_string(field_handle f, void * data, int len){
	access_result a = get_field_by_handle(f, data, len);
	switch(f.type){
		case BOOL_TYPE:
		case USHORT_TYPE:
		case UINT_TYPE:
			return std::to_string(a.r.ui);
		case INT_TYPE:
		case IP_TYPE:
			return std::to_string(a.r.i);
		case LLONG_TYPE:
			return std::to_string(a.r.l);
		case ULLONG_TYPE:
			return std::to_string(a.r.ul);
		case FLOAT_TYPE:
			return std::to_string(a.r.f);
		case VSTR_TYPE:{
			std::string rets(a.r.vs.data, a.r.vs.length);
			return rets;
		}
		case TIMEVAL_TYPE:{
			double tv = a.r.t.tv_sec + ((double)a.r.t.tv_usec)/1000000.0;
			return std::to_string(tv);
		}
		case IPV6_TYPE:{
			char buf[100];
			sprintf(buf, "%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x",
				a.r.ip6.v[0] >> 16, (a.r.ip6.v[0]) & 0xffff,
				a.r.ip6.v[1] >> 16, (a.r.ip6.v[1]) & 0xffff,
				a.r.ip6.v[2] >> 16, (a.r.ip6.v[2]) & 0xffff,
				a.r.ip6.v[3] >> 16, (a.r.ip6.v[3]) & 0xffff
			);
			return buf;
		}
		default:
			return "";
	}
	return "";
}
		
}


