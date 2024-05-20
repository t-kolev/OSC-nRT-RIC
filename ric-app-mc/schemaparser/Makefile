#!/bin/make

# ------------------------------------------------
#   Copyright 2020 AT&T Intellectual Property
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.
# -------------------------------------------

#SHELL = /bin/sh

.SUFFIXES:.cc

mc_schema.a: schemaparser.o json.o block_allocator.o
	ar -cr mc_schema.a schemaparser.o json.o block_allocator.o

schemaparser.o : schemaparser.cc
	g++ -g -O3 -std=c++11 -c schemaparser.cc

json.o : json.cc
	g++ -g -O3 -std=c++11 -c json.cc

block_allocator.o : block_allocator.cc
	g++ -g -O3 -std=c++11 -c block_allocator.cc

mc_extract: mc_schema.a mc_extract.cc
	g++ mc_extract.cc mc_schema.a -g -std=c++11 -o mc_extract -lsdl

sample2: mc_schema.a sample2.cc
	g++ sample2.cc mc_schema.a -g -std=c++11 -o sample2 -lsdl

mc_store_schema: mc_schema.a mc_store_schema.cc
	g++ mc_store_schema.cc mc_schema.a -g -std=c++11 -o mc_store_schema -lsdl

load_mcnib1: mc_schema.a load_mcnib1.cc
	g++ load_mcnib1.cc mc_schema.a -g -std=c++11 -o load_mcnib1 -lsdl

mc_keys: mc_keys.cc
	g++ mc_keys.cc -g -std=c++11 -o mc_keys -lsdl

mc_extract_string: mc_extract_string.cc
	g++ mc_extract_string.cc -g -std=c++11 -o mc_extract_string -lsdl

clean:
	rm *.o *.a mc_extract sample2 mc_store_schema load_mcnib1 mc_keys mc_extract_string

utils: mc_extract sample2 mc_store_schema load_mcnib1 mc_keys mc_extract_string
