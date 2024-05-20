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
    Mnemonic:	tools.h
    Abstract:	Header file for the C tools

    Date:       16 June 2020
    Author:     E. Scott Daniels
*/

#ifdef __cplusplus
extern "C" {
#endif

#define MT_LOG_CRIT	0
#define MT_LOG_ERR	1
#define MT_LOG_WARN	2
#define MT_LOG_INFO 3
#define MT_LOG_DEBUG 4

#define MT_NO_FLAGS	0x00

// add open/closing curly brace on json
#define MT_FL_OPEN	0x01		// add an opener [ or { depending on array flag
#define MT_FL_CLOSE	0x02		// add closer } or ] depending on setting of array flag
#define MT_FL_ARRAY 0x04		// add array square brackets not curly braces
#define MT_FL_LAST	0x08		// last closer; no comma to be added

// ----- prototypes ------------------------------------------------
extern int mt_snprintf( char* target, int size, char* fmt, ... );
extern void usage( const char* argv0 );
extern void ensure_nxt_arg( int max, int idx, int exit_code );

extern char* json_add_bool( char* buf, int len, const char* name, int value, int flags );
extern char* json_add_double( char* buf, int len, const char* name, double value, int flags );
extern char* json_add_int( char* buf, int len, const char* name, int value, int flags );
extern char* json_add_ll( char* buf, int len, const char* name, long long value, int flags );
extern char* json_add_nil( char* buf, int len, const char* name, int flags );
extern char* json_start_obj( char* buf, int len, const char* name );
extern char* json_add_string( char* buf, int len, const char* name, const char* value, int flags );

extern void mt_log_target( const char* file_name );
extern void mt_log_init( void );
extern void mt_log_set_hr( int state );
extern void mt_log_set_level( int new_level );
extern void mt_log_str2level( const char* level );
extern void mt_log( int vlevel, const char* fmt, ... );

extern long long mt_now( void );

extern void dump( const char* buf, int len );

#ifdef __cplusplus
}
#endif

