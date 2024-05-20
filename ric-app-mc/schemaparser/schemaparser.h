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



#ifndef __SCHEMAPARSER_INCLUDED__
#define __SCHEMAPARSER_INCLUDED__

#include<string>
#include<map>
#include<algorithm>
#include<vector>


//		If an mc_string is returned, it points to text in the input buffer.
struct mc_string{
	unsigned int length;
	char *data;
};

struct mc_ipv6_str{
	unsigned int v[4];
};



///////////////////////////////////////////////////
//		Type functions

#define UNDEFINED_TYPE 0
#define UINT_TYPE 1
#define INT_TYPE 2
#define ULLONG_TYPE 3
#define LLONG_TYPE 4
#define USHORT_TYPE 5
#define FLOAT_TYPE 6
#define BOOL_TYPE 7
#define VSTR_TYPE 8
#define TIMEVAL_TYPE 9
#define IP_TYPE 10
#define FSTRING_TYPE 11
#define IPV6_TYPE 12

namespace mc_schema{

//		Number of bytes used by a field
int type_size(int dt);
//		string to integer
int assign_type_from_string(std::string st);
//		integer to string
std::string type_to_string(int typ);
//		there is a casting function
bool is_castable(int src, int dst);
//		cast without loss of information
bool is_compatible(int src, int dst);

	
////////////////////////////////////////////
//		Represent the parsed schema of a query stream.
//		Created by mc_schema
//		will be returned on request from mc_schema

//	field_handle simplifies some of the field access functions
struct field_handle{
	int type;
	int offset;
};


class query_rep{
protected:
	query_rep() {
	};
public:
	virtual int finalize() = 0;

//		Return a text representation
	virtual std::string to_string() = 0;
//		Number of fields
	virtual int get_num_fields() = 0;
//		name of ith field (starting at zero)
	virtual std::string get_field_name(int i) = 0;
//		lookup field index by name, -1 if not found
	virtual int get_index_of_field(std::string name) = 0;
//		lookup field handle by name, -1 if not found
	virtual field_handle get_handle_of_field(std::string name) = 0;
//		data type of ith field.
	virtual int get_type(int i) = 0;
//		string representation of data type of ith field
	virtual std::string get_type_name(int i) = 0;
//		byte offset of ith field in a data block
	virtual int get_offset(int i) = 0;
//		handle of ith field in a data block
	virtual field_handle get_handle(int i) = 0;
};

//		This class parses a json representation of the streams and manages them
class mc_schemas{
protected:
//		n is a char buffer holding the json description of the stream schemas
	mc_schemas(){ }

public:
	virtual ~mc_schemas(){};
	virtual void init(const char *n) = 0;
//		true if there are any errors.
	virtual bool has_errors() = 0;

//		string with the error reports.  empty if there is no error.
	virtual std::string get_errors() = 0;
//		return the names of the parsed streams.
	virtual std::vector<std::string> get_streams() = 0;
//		return the names of the unsucessfully parsed streams
	virtual std::vector<std::string> get_error_streams() = 0;
//		number of sucessfully and unsucessfully parsed streams
	virtual bool stream_found(std::string s) = 0;
//		true if there is a stream with name s which parsed successfully
	virtual bool stream_ok(std::string s) = 0;
//		return the error associated with a stream, if any.
	virtual std::string get_stream_error(std::string s) = 0;
//		Get the query representation of a successfully parsed stream
	virtual query_rep *get_query_rep(std::string s) = 0;

};	

//		Factory function.  They do not GC the input.
mc_schemas *new_mc_schemas(std::string s);
mc_schemas *new_mc_schemas(const char *s);
		
//		Universal return type
struct access_result {
	int field_data_type; // as defined
	union {
		int i;
		unsigned int ui;
		long long int l;
		unsigned long long int ul;
		double f;
		struct timeval t;		// defined in sys/time.h
		mc_string vs;
		mc_ipv6_str ip6;
	} r;
};

////////////////////////////////////////////
//			Direct tuple access functions.
//			No casting is done.  use query_rep to get the offset
unsigned int unpack_uint(void *data, int len, int offset, int *problem);
unsigned int unpack_ushort(void *data, int len, int offset, int *problem);
unsigned int unpack_bool(void *data, int len, int offset, int *problem);
int unpack_int(void *data, int len, int offset, int *problem);
unsigned long long int unpack_ullong(void *data, int len, int offset, int *problem);
long long int unpack_llong(void *data, int len, int offset, int *problem);
double unpack_float(void *data, int len, int offset, int *problem);
timeval unpack_timeval(void *data, int len, int offset, int *problem);
mc_string unpack_vstr(void *data,  int len, int offset, int *problem);
struct mc_ipv6_str unpack_ipv6(void *data,  int len, int offset, int *problem);

//		Result returned in universal return type.
//			will lookup type and offset in query_rep
access_result get_field_by_index(query_rep *qr, int index, void *data, int len);
//			user must lookup field_handle type in query_rep
access_result get_field_by_handle(field_handle f, void * data, int len);

//		These access functions perform casting,
//		use is_castable and is_compatible to determine if a field can
//		be cast to the return type.  Everything can cast to string.
//			user must lookup offset and type in query_rep
int get_field_int(field_handle f, void * data, int len);
unsigned int get_field_uint(field_handle f, void * data, int len);
long long int get_field_llong(field_handle f, void * data, int len);
unsigned long long int get_field_ullong(field_handle f, void * data, int len);
double get_field_float(field_handle f, void * data, int len);
timeval get_field_timeval(field_handle f, void * data, int len);
mc_ipv6_str get_field_ipv6(field_handle f, void * data, int len);
std::string get_field_string(field_handle f, void * data, int len);
		
}	

#endif

