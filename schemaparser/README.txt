MC-NIB Schema Parser library

Use this library to parse records fetched from the MC-NIB

The schema parser library consists of 7 files
	block_allocator.cc
	block_allocator.h
	json.cc
	json.h
	schemaparser.h
	schemaparser_impl.h
	schemaparser.cc

If you enter
	make
you will build mc_schema.a.

To use the MC-NIB schema parser, include
	schemaparser.h
and link against
	mc_schema.a

This directory also contains some examples and utilities.
However to build them you need to have lsdl installed.

To build the examples and utilities, enter
	make utils

mc_extract
	This utility accepts table name and an optional key prefix.
	The utility will fetch all records in the MC-NIB from that table
	whose key matches the prefix and print formatted output.
	This utility also serves as an example of schema parser library usage.
	- mc_extract will attempt to fetch the schema from ther MC-NIB.
	  If the schema isn't in the MC-NIB (see below), an optional parameter
	  is the name of the schema file.  Use the provided file nib.json

sample2
	This example illustrates various ways of extracting fields using
	the schema parser.

mc_store_schema
	mc_extract will attempt to store a schema in the MC-NIB.
	Use this utility to store nib.json as the schema in the MC-NIB.

load_mcnib1
	This utility loads several records into the MC-NIB for mc_extract
	to fetch, using the table definition throughput_ue

mc_keys
	This utility will fetch all keys from MC-NIB which match an optional prefix

mc_extract_string
	This utility will fetch and print all strings in the MC-NIB which match 
	an optional prefix.  No interpretation is done, so this utility is
	mostly useful for extracting the schema, under key _schema
	


