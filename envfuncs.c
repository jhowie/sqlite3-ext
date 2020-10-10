/* File: envfuncs.c
**
** Author: John Howie (john@howieconsultinginc.com)
**
** Description
**
** This file contains the source code for the library that implements the
** environment variables functions extension for SQLite3
**
**
** Modifications
**
** 2020-10-09 John Howie    Original.
**
*/

/*

Copyright 2020 Howie Consulting Inc.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in
   the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
DAMAGE.

*/

# include "sqlite3ext.h"
SQLITE_EXTENSION_INIT1
# include <assert.h>
# include <string.h>
# include <stdlib.h>

static const char* CopyrightNotice = "envfuncs.c (c) 2020 Howie Consulting Inc.";

// static void sqlite3_ext_getenvfunc (sqlite3_context *context, int argc, sqlite3_value **argv)
//
// This function is an implementation of the SQLite3 extension function
// GETENV(). The function takes as an argument a string, which is used in a
// search of environment variables. If a match is found the value of the
// environmental variable is returned. If the variable does not exist an empty
// string is required.
//
// NOTE: The extension function itself does not care about the case of the
//       name of the environment variable, but the underlying library or OS
//       most likely will.
//

static void sqlite3_ext_getenvfunc (sqlite3_context *context, int argc, sqlite3_value **argv)
{
    const  char *EnvVarName, *EnvVarValuePtr;
    char *EnvVarValueBuffer;
    int EnvVarValueLength = 0;

    // Make sure we got only a single argument, and that the argument is not
    // a NULL string

    assert (argc==1);
    if (sqlite3_value_type (argv[0]) == SQLITE_NULL) return;

    // Get the argument supplied, which is tbe name of the environment variable
    // we need to look up

    EnvVarName = (const char *) sqlite3_value_text (argv[0]);

    // Lookup the environment variable. If the variable does not exist just
    // return

    EnvVarValuePtr = getenv (EnvVarName);
    if (EnvVarValuePtr == (const char *) 0)
        return;

    // Get the length of the value associated with the environmental value,
    // and allocate buffer to hold the value. We add one bytes to the value
    // for the NULL terminating character

    EnvVarValueLength = strlen (EnvVarValuePtr);
    EnvVarValueBuffer = (char *) sqlite3_malloc (EnvVarValueLength +1);
    if (EnvVarValueBuffer == (char *) 0) {
        // An error occurred, and we could not allocate the memory required

        sqlite3_result_error_nomem (context);
        return;
    }

    // Set the buffer contents to NULL, and then copy over the value to
    // the buffer

    bzero ((void *) EnvVarValueBuffer, (EnvVarValueLength +1));
    bcopy ((const void *) EnvVarValuePtr, (void *) EnvVarValueBuffer, EnvVarValueLength);

    // Set the result, free up memory, and then return

    sqlite3_result_text (context, (const char *) EnvVarValueBuffer, (EnvVarValueLength +1), SQLITE_TRANSIENT);
    sqlite3_free (EnvVarValueBuffer);
}

// int sqlite3_envfuncs_init (sqlite3 *db, char **pzErrMsg, const sqlite3_api_routines *pApi)
//
// This is the entry function for the library, and registers the extension
// functions
//

#ifdef _WIN32
__declspec(dllexport)
#endif
int sqlite3_envfuncs_init (sqlite3 *db, char **pzErrMsg, const sqlite3_api_routines *pApi)
{
  int rc = SQLITE_OK;
  SQLITE_EXTENSION_INIT2 (pApi);
  (void) pzErrMsg;  /* Unused parameter */

  rc = sqlite3_create_function(db, "getenv", 1, (SQLITE_UTF8 /*| SQLITE_INNOCUOUS*/ | SQLITE_DETERMINISTIC), 0, sqlite3_ext_getenvfunc, 0, 0);
  
  return rc;
}