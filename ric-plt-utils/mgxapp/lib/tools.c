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
    Mnemonic:	tools.c
    Abstract:	Various functions to support the munchkin.

    Date:       16 June 2020
    Author:     E. Scott Daniels
*/

#ifdef __cplusplus
extern "C" {
#endif

#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <fcntl.h>

#include <mdclog/mdclog.h>

#include "tools.h"

/*
	Extended snprintf. Provides for %q making code much more readable
	than \"%s\".  Go got this right.
*/
extern int mt_snprintf( char* target, int size, char* fmt, ... ) {
	va_list argp;			// first arg in our parms which is variable
	char    nfmt[4096];		// new format
	char*	si;				// source index
	char*	di;				// dest index

	for( si = fmt, di = nfmt; *si && di < nfmt + (sizeof( nfmt ) - 8); ) {	// padding enough to avoid in loop len check
		switch( *si ) {
			case '%':
				if( *(si+1) == 'q' ) {
					si += 2;

					*(di++) = '"';
					*(di++) = '%';
					*(di++) = 's';
					*(di++) = '"';
				} else {
					*(di++) = *(si++);			// just plop it in
					if( *(si) == '%' ) {		// don't trip over %%q
						*(di++) = *(si++);		// just plop it in
					}
				}
				break;

			default:
				*(di++) = *(si++);
				break;
		}
	}

	*di = 0;

	va_start( argp, fmt );									// suss out parm past fmt
	return vsnprintf( target, size, nfmt, argp );			// format using expanded format
}

/*
	Print a usage message.
*/
extern void usage( const char* argv0 ) {
	fprintf( stderr, "usage: %s [[-P port-name] -c config-file] [-d] [-l log-file]"
				" [-p port] [-n num-threads] [-r] [-t ves-target-url] [-v|-V] [-w]\n", argv0 );

	fprintf( stderr, "%s",
				"\n"
				"\t-c provides the name of the configuration file to read\n"
				"\t-d enables debug level logging\n"
				"\t-l redirects all stderr messages to the named file\n"
				"\t-V enables verbosity with human readable messages\n"
				"\t-P is valid only when supplying a config file, and MUST be supplied before the -c option\n"
				"\t-r sets human readable messages when writing to stderr\n"

				"\n"
				"\t CAUTION: all options which appear before the config file name (when given) will be overriden by the config file\n"
				"\t          all options which appear AFTER the config file name (when given) will be override the contents of the config file\n"
	);
}

/*
	Ensure that there is a 'next' argument. Passing the exit code
	(ec) allows for testing to drive this with a "good" exit if
	it traps the error state.
*/
extern void ensure_nxt_arg( int max, int idx, int ec ) {
	if( idx  >= max ) {
		fprintf( stderr, "<MUNCHER> not enough parameters on the command line\n" );
		exit( ec );
	}
}

// ---- json building support ---------------------------------------------------------------

static char*	header = "";
static char*	trailer = "";

/*
	Set header (open bracket/brace) and trailer (comma, brackt/brace[comma])
	baed on the flags.

		MT_fl_close -- an opener symbol (bracket or brace) is addd bfore the
			name value pair.  e.g.  { "name": "Kenton"

		MT_FL_CLOSE -- a closer symbol (bracket or brace) is added before the comma
			e.g.  "name": "Kenton" },

		MT_FL_ARRAY -- a square bracket is used as the opener/closer

		MT_FL_LAST -- The comma normally set after the closer is omitted
			e.g.  "name": "Kenton" }

	Header/trailer are maintained as static vars to make life easy.
*/
static inline void set_ht( int flags ) {
	trailer = ", ";							// default trailer to just a comma

	if( flags & MT_FL_OPEN ) {
		header = flags & MT_FL_ARRAY ? "[" : "{ ";
	}

	if( flags & MT_FL_CLOSE ) {
		if( flags & MT_FL_LAST ) {
			trailer = flags & MT_FL_ARRAY ? "] }" : " }";
		} else {
			trailer = flags & MT_FL_ARRAY ? "]," : " },";
		}
	}
}

/*
	The next few function take a buffer with existing json, add name/value pair to it where value
	is a string, int, float, bool.  Flags is  MT_FL_OPEN  or MT_FL_CLOSE which causes either a
	leading or trailing curly brace to be added. Len is the max size of the
	buffer being constructed.

	They all return a pointer to the buf as a convenience.

	These are NOT efficent as they perform a strlen() call on the existing string for
	each call.  The assumption is that the munckin isn't "high speed" and so this won't
	matter, but it shouldn not be too difficult to improve the performance down the road
	if it becomes necessary; suggisness was selected for ease of coding.
*/
extern char* json_add_string( char* buf, int len, const char* name, const char* value, int flags ) {
	char	newstr[1024];
	int		nlen = 0;

	if( name != NULL && value != NULL )  {
		set_ht( flags );
		nlen = mt_snprintf( newstr, sizeof( newstr ), "%s%q: %q%s", header, name, value, trailer );

		if( (strlen( buf ) + nlen + 1) < len ) {
			strcat( buf, newstr );
		}
	}

	return buf;
}

extern char* json_add_int( char* buf, int len, const char* name, int value, int flags ) {
	char	newstr[1024];
	int		nlen = 0;

	if( name != NULL )  {
		set_ht( flags );
		nlen = mt_snprintf( newstr, sizeof( newstr ), "%s%q: %d%s", header, name, value, trailer );

		if( (strlen( buf ) + nlen + 1) < len ) {
			strcat( buf, newstr );
		}
	}

	return buf;
}

extern char* json_add_ll( char* buf, int len, const char* name, long long value, int flags ) {
	char	newstr[1024];
	int		nlen = 0;

	if( name != NULL )  {
		set_ht( flags );
		nlen = mt_snprintf( newstr, sizeof( newstr ), "%s%q: %lld%s", header, name, value, trailer );

		if( (strlen( buf ) + nlen + 1) < len ) {
			strcat( buf, newstr );
		}
	}

	return buf;
}

extern char* json_add_double( char* buf, int len, const char* name, double value, int flags ) {
	char	newstr[1024];
	int		nlen = 0;

	if( name != NULL )  {
		set_ht( flags );
		nlen = mt_snprintf( newstr, sizeof( newstr ), "%s%q: %.5f%s", header, name, value, trailer );

		if( (strlen( buf ) + nlen + 1) < len ) {
			strcat( buf, newstr );
		}
	}

	return buf;
}

extern char* json_add_bool( char* buf, int len, const char* name, int value, int flags ) {
	char	newstr[1024];
	int		nlen = 0;

	if( name != NULL )  {
		set_ht( flags );
		nlen = mt_snprintf( newstr, sizeof( newstr ), "%s%q: %s%s", header, name, value ? "true" : "false", trailer );

		if( (strlen( buf ) + nlen + 1) < len ) {
			strcat( buf, newstr );
		}
	}

	return buf;
}

/*
	Start an "object" by adding '"name": {' to the buffer. Objects are terminated
	with the last add_*() call and setting the flags to include MT_FL_CLOSE.
*/
extern char* json_start_obj( char* buf, int len, const char* name ) {
	char	newstr[1024];
	int		nlen = 0;

	if( name != NULL  ) {
		nlen = mt_snprintf( newstr, sizeof( newstr ), "%q: { ", name );

		if( (strlen( buf ) + nlen + 1) < len ) {
			strcat( buf, newstr );
		}
	}

	return buf;
}

extern char* json_add_nil( char* buf, int len, const char* name, int flags ) {
	char	newstr[1024];
	int		nlen = 0;

	if( name != NULL )  {
		set_ht( flags );
		nlen = mt_snprintf( newstr, sizeof( newstr ), "%s%q: null%s", header, name, trailer );

		if( (strlen( buf ) + nlen + 1) < len ) {
			strcat( buf, newstr );
		}
	}

	return buf;
}

// ------------- logging ----------------------------------------------------

/*
	Provide a simple interface to the standard RIC logger with the ability to
	generate human readable messages on stderr rather than json encrusted garbage.

	Logging, fprintf,  is slow, we make no attempt to be efficent.
*/

static int mt_hr_msgs = 0;							// true if we want readable messages
static int mt_mdc_init = 0;							// true if we called mdc to initialise
static int mt_log_vlevel = 1;						// the higher the value the lower the severity.
static mdclog_severity_t mt_mdc_level = MDCLOG_ERR;	// level to pass to mdc
static	FILE*	mt_log_dest = NULL;					// allow internal redirection


/*
	Allow caller to redirect human readable output
*/
extern void mt_log_target( const char* file_name ) {
	if( mt_log_dest != stderr ) {
		fclose( mt_log_dest );
	}

	if( strcmp( file_name, "stderr" ) == 0 ) {
		mt_log_dest = stderr;
	} else {
		mt_log_dest = fopen( file_name, "a" );
	}
}

/*
	Initialise things; automatically invoked if user does not
*/
extern void mt_log_init( void ) {
	if( mt_log_dest == NULL ) {
		mt_log_dest = stderr;
	}

	if( !mt_hr_msgs && ! mt_mdc_init ) {
		mdclog_init( NULL );
		mt_mdc_init = 1;
		mdclog_level_set( mt_mdc_level );
	}
}

/*
	Enable/disable the human readable state. If true (!0) then all subsequent calls to
	the logging function will generate human readable messgaes.
*/
extern void mt_log_set_hr( int state ) {
	mt_hr_msgs = !! state;
	mt_log_init();
}

/*
	Set the log level to the desired value.
*/
extern void mt_log_set_level( int new_level ) {
	if( new_level >= 0 && new_level <= MT_LOG_DEBUG ) {
		mt_log_vlevel = new_level;

		switch( mt_log_vlevel ) {
			case MT_LOG_CRIT:			// mdc has no concept of a critical error
					// fallthrough
			case MT_LOG_ERR:
					mt_mdc_level = MDCLOG_ERR;
					break;

			case MT_LOG_WARN:
					mt_mdc_level = MDCLOG_WARN;
					break;

			case MT_LOG_INFO:
					mt_mdc_level = MDCLOG_INFO;
					break;

			default:
					mt_mdc_level = MDCLOG_DEBUG;
					break;
		}

		if( ! mt_hr_msgs ) {
			mt_log_init();				// ensure initialised
		}
		mdclog_level_set( mt_mdc_level );

	}
}

/*
	Convert a string (crit, err, war, etc. to a log level and set the level.
*/
extern void mt_log_str2level( const char* str ) {
	char*	dstr;
	char*	c;

	dstr = strdup( str );
	for( c = dstr; *c; c++ ) {
		tolower( *c );
	}


	switch( *dstr ) {
		case 'c':				// accept crit critical Crit etc.
			mt_log_set_level( MT_LOG_CRIT );
			break;

		case 'd':
			if( strcmp( dstr, "default" ) == 0 ) {
				mt_log_set_level( MT_LOG_WARN );
			} else {
				mt_log_set_level( MT_LOG_DEBUG );
			}
			break;

		case 'e':
			mt_log_set_level( MT_LOG_ERR );
			break;

		case 'i':
			mt_log_set_level( MT_LOG_INFO );
			break;

		case 'w':
			mt_log_set_level( MT_LOG_WARN );
			break;

		default:
			mt_log_set_level( MT_LOG_WARN );
			mt_log( MT_LOG_WARN, "unrecognised log level '%s' defaulting to 'warning'\n", dstr );
			break;
	}

	free( dstr );
}

/*
	Write a message if the level is >= the selected level.
*/
extern void mt_log( int vlevel, const char* fmt, ... ) {
	static int	pid = 0;
	va_list		argp;			// variable arg pointer
	char		msg[4096];
	char*		nl;

	if( vlevel > mt_log_vlevel ) {
		return;
	}

	if( !pid ) {
		pid = getpid();
		mt_log_init();					// likely not initialised either
	}

	va_start( argp, fmt );
	vsnprintf( msg, sizeof( msg ),  fmt, argp );		// format user's message
	va_end( argp );

	if( (nl = strchr( msg, '\n' )) != NULL ) {		// newlines not allowed -- truncate at first
		*nl = 0;
	}

	if( mt_hr_msgs ) {
		char*	vstr = "UNK";

		switch( vlevel ) {
			case MT_LOG_CRIT:
				vstr = "[CRIT]";
				break;

			case MT_LOG_ERR:
				vstr = "[ERR] ";
				break;

			case MT_LOG_WARN:
				vstr = "[WARN]";
				break;

			case MT_LOG_INFO:
				vstr = "[INFO]";
				break;

			case MT_LOG_DEBUG:
				vstr = "[DBUG]";
				break;
		}

		fprintf( mt_log_dest, "%10d  %d/%s %s %s\n", (int) time( NULL ), pid, "munchkin",  vstr, msg );
	} else {
		mdclog_write( mt_mdc_level, "%s", msg );
	}
}



// -------------- time things ------------------------------------

/*
	Return the current time as an integer in microseconds (mu-sec) past the
	epoch.
*/
extern long long mt_now( void ) {
	struct timespec ts;
	long long		now = 0;

	clock_gettime( CLOCK_REALTIME, &ts );
	now = (ts.tv_sec * 1000000) + (ts.tv_nsec/1000);

	return now;
}

// ---- debugging help --------------------------------------------

/*
	Dump a hex representation of memory to the stderr device.
*/
extern void dump( const char* buf, int len ) {
	int i;

	fprintf( stderr, "Buffer at: %p\n", buf );
	for( i = 0; i < len; i++ ) {
		fprintf( stderr, "%02x ", (unsigned char) *(buf++) );
		if( i % 16 == 0 ) {
			fprintf( stderr, "\n" );
		}
	}

	fprintf( stderr, "\n" );
}


// ==========================================================================
#ifdef __cplusplus
}
#endif
