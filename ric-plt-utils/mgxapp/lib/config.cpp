// vi: ts=4 sw=4 noet:
/*
==================================================================================
    Copyright (c) 2020 AT&T Intellectual Property.
    Copyright (c) 2020 Nokia

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
==================================================================================
*/

/*
    Mnemonic:	config.cpp
    Abstract:	Support for reading config json.

    Date:       13 July 2020
    Author:     E. Scott Daniels
*/

#include <memory>
#include <sstream>

#include "ricxfcpp/jhash.hpp"


#include "config.h"
#include "tools.h"

namespace munchkin {


/*
	Read a file containing json and parse into a framework Jhash.

	Using C i/o will speed this up, but I can't imagine that we need
	speed reading the config file once in a while.
	The file read comes from a stack overflow example:
		stackoverflow.com/questions/2912520/read-file-contents-into-a-string-in-c
*/
extern std::shared_ptr<xapp::Jhash> Conf_parse( std::string fname ) {
	std::shared_ptr<xapp::Jhash>	jh;

	std::ifstream ifs( fname );
	std::string st( (std::istreambuf_iterator<char>( ifs ) ), (std::istreambuf_iterator<char>() ) );

	jh = std::shared_ptr<xapp::Jhash>( new xapp::Jhash( st.c_str() ) );
	return  jh->Parse_errors() ? NULL : jh;
}

/*
	Suss out the port for the named "interface". The interface is likely the application
	name.
*/
extern std::string Conf_get_port( std::shared_ptr<xapp::Jhash> jh, std::string name ) {
	int i;
	int	nele = 0;
	double value;
	std::string rv = "";		// result value
	std::string pname;			// element port name in the json

	if( jh == NULL ) {
		return rv;
	}

	jh->Unset_blob();
	if( jh->Set_blob( (char *) "messaging" ) ) {
		nele = jh->Array_len( (char *) "ports" );
		for( i = 0; i < nele; i++ ) {
			if( jh->Set_blob_ele( (char *) "ports", i ) ) {
				pname = jh->String( (char *) "name" );
				if( pname.compare( name ) == 0 ) {				// this element matches the name passed in
					value = jh->Value( (char *) "port" );
					rv = std::to_string( (int) value );
					jh->Unset_blob( );							// leave hash in a known state
					return rv;
				}
			}

			jh->Unset_blob( );								// Jhash requires bump to root, and array reselct to move to next ele
			jh->Set_blob( (char *) "messaging" );
		}
	}

	jh->Unset_blob();
	return rv;
}

/*
	Suss out the named string from the control object. If the resulting value is
	missing or "", then the default is returned.
*/
extern std::string Conf_get_cvstr( std::shared_ptr<xapp::Jhash> jh, std::string name, std::string defval ) {
	std::string value;
	std::string rv;				// result value

	rv = defval;
	if( jh == NULL ) {
		return rv;
	}

	jh->Unset_blob();
	if( jh->Set_blob( (char *) "controls" ) ) {
		if( jh->Exists( name.c_str() ) )  {
			value = jh->String( name.c_str() );
			if( value.compare( "" ) != 0 ) {
				rv = value;
			}
		}
	}

	jh->Unset_blob();
	return rv;
}

/*
	Convenience funciton.
	No default value; returns "" if not set.
*/
extern std::string Conf_get_cvstr( std::shared_ptr<xapp::Jhash> jh, std::string name ) {
	return Conf_get_cvstr( jh, name, "" );
}

/*
	Suss out the named field from the control object with the assumption that it is a boolean.
	If the resulting value is missing then the defval is used.
*/
extern bool Conf_get_cvbool( std::shared_ptr<xapp::Jhash> jh, std::string name, bool defval ) {
	bool value;
	bool rv;				// result value

	rv = defval;
	if( jh == NULL ) {
		return rv;
	}

	jh->Unset_blob();
	if( jh->Set_blob( (char *) "controls" ) ) {
		if( jh->Exists( name.c_str() ) )  {
			rv = jh->Bool( name.c_str() );
		}
	}

	jh->Unset_blob();
	return rv;
}


/*
	Convenience function.
*/
extern bool Conf_get_cvbool( std::shared_ptr<xapp::Jhash> jh, std::string name ) {
	return Conf_get_cvbool( jh, name, false );
}

} // namespace

