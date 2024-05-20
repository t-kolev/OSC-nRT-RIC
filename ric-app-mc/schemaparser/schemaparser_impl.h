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

//	TODO garbage collection isn't being done
//		the json package doesn't do any GC.



#ifndef __SCHEMAPARSER_IMPL_INCLUDED__
#define __SCHEMAPARSER_IMPL_INCLUDED__

#include"schemaparser.h"
#include "json.h"

///////////////////////////////////////
//		structured types
struct vstring32 {
    unsigned int length;
    unsigned int offset;
    unsigned int reserved;
};


namespace mc_schema{

//////////////////////////////////////////////////////
//		Field_entry
//		Use to load definitions from json
//		Internally used class.

class field_entry{
public:
	std::string name;
	std::string type;
	int pos;
	bool is_ts;

	void init(){
		type=-1;
		pos=-1;
		is_ts = false;
	}
	field_entry(){ this->init(); }
	std::string load_from_json(mc_json::json_value *froot);
};
	
////////////////////////////////////////
//		Parsed representation from the json.
//		Internally used class.
class tuple_access_info{
public:
	std::string field_name;
	int offset;
	int pdt;
	bool is_ts;

	tuple_access_info(){
		pdt = UNDEFINED_TYPE;
		offset = 0;
		is_ts = 0;
	};
	int init(field_entry *fe);
	std::string to_string();
};

////////////////////////////////////////////
//		Represent the parsed schema of a query stream.
//		Created by mc_schema
//		will be returned on request from mc_schema
class _query_rep : public query_rep{
public:
	std::string name;
	std::vector<std::string> keys;
	std::vector<tuple_access_info> field_info;
	int min_tuple_size;

	_query_rep() {
			min_tuple_size = 0;
	};
	std::string init(const mc_json::json_value *strm);
	int finalize();

//		Return a text representation
	std::string to_string();
//		Number of fields
	int get_num_fields();
//		name of ith field (starting at zero)
	std::string get_field_name(int i);
//		lookup field index by name, -1 if not found
	int get_index_of_field(std::string name);
//		lookup field handle by name, -1 if not found
	field_handle get_handle_of_field(std::string name);
//		data type of ith field.
	int get_type(int i);
//		string representation of data type of ith field
	std::string get_type_name(int i);
//		byte offset of ith field in a data block
	int get_offset(int i);
//		handle of ith field in a data block
	field_handle get_handle(int i);
};

//		This class parses a json representation of the streams and manages them
class _mc_schemas : public mc_schemas{
public:
	std::map<std::string, _query_rep *> qreps;
	std::map<std::string, std::string> qerrs;
	std::string err;
	char *nib_str;

//		n is a char buffer holding the json description of the stream schemas
	_mc_schemas(const char *n);
//		n is a string holding the json description of the stream schemas
	_mc_schemas(std::string s);

	~_mc_schemas();

	void init(const char *n);

//		true if there are any errors.
	bool has_errors();

//		string with the error reports.  empty if there is no error.
	std::string get_errors();
//		return the names of the parsed streams.
	std::vector<std::string> get_streams();
//		return the names of the unsucessfully parsed streams
	std::vector<std::string> get_error_streams();
//		true if some stream was parsed successful or not
	bool stream_found(std::string s);
//		true if there is a stream with name s which parsed successfully
	bool stream_ok(std::string s);
//		return the error associated with a stream, if any.
	std::string get_stream_error(std::string s);
//		Get the query representation of a successfully parsed stream
	query_rep *get_query_rep(std::string s);

};	
		
		
}	

#endif

