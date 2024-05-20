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
    Mnemonic:	config.h
    Abstract:	Header for the configuration module

    Date:       13 July 2020
    Author:     E. Scott Daniels
*/

#ifndef _CONFIG_H
#define  _CONFIG_H

#include <cstdlib>
#include <string>
#include <memory>
#include <fstream>

#include "config.h"

#include "ricxfcpp/jhash.hpp"

namespace munchkin {


// ------------ prototypes -----------------------------------------------------
std::shared_ptr<xapp::Jhash> Conf_parse( std::string fname );
extern std::string Conf_get_port( std::shared_ptr<xapp::Jhash> jh, std::string name );
extern bool Conf_get_cvbool( std::shared_ptr<xapp::Jhash> jh, std::string name, bool defval );
extern bool Conf_get_cvbool( std::shared_ptr<xapp::Jhash> jh, std::string name );
extern std::string Conf_get_cvstr( std::shared_ptr<xapp::Jhash> jh, std::string name, std::string defval );
extern std::string Conf_get_cvstr( std::shared_ptr<xapp::Jhash> jh, std::string name );



} // namespace

#endif
