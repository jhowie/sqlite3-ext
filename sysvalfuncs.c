/* File: sysvalfuncs.c
**
** Author: John Howie (john@howieconsultinginc.com)
**
** Description
**
** This file contains the source code for the library that implements the
** system values functions extension for SQLite3
**
**
** Modifications
**
** 2020-10-11 John Howie    Original.
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
# include <unistd.h>
# include <stdlib.h>
# include <sys/param.h>

static const char* CopyrightNotice = "sysvalfuncs.c (c) 2020 Howie Consulting Inc.";

// static void sqlite3_ext_getuidfunc (sqlite3_context *context, int argc, sqlite3_value **argv)
//
// This function is the implementation of the SQLite3 extension function
// GETUID(). The function takes no arguments and returns the UID of the
// current user
//

static void sqlite3_ext_getuidfunc (sqlite3_context *context, int argc, sqlite3_value **argv)
{
    // Make sure we got no arguments

    assert (argc == 0);

    // This is rerally simple - just set the result to the UID value obtained
    // by a call to getuid ()

    sqlite3_result_int (context, (int) getuid ());
}

// static void sqlite3_ext_geteuidfunc (sqlite3_context *context, int argc, sqlite3_value **argv)
//
// This function is the implementation of the SQLite3 extension function
// GETEUID(). The function takes no arguments and returns the effective
// UID (EUID) of the current user
//

static void sqlite3_ext_geteuidfunc (sqlite3_context *context, int argc, sqlite3_value **argv)
{
    // Make sure we got no arguments

    assert (argc == 0);

    // This is rerally simple - just set the result to the effectuive UID (EUID)
    // value obtained by a call to geteuid ()

    sqlite3_result_int (context, (int) geteuid ());
}

// static void sqlite3_ext_getgidfunc (sqlite3_context *context, int argc, sqlite3_value **argv)
//
// This function is the implementation of the SQLite3 extension function
// GETGID(). The function takes no arguments and returns the GID of the
// current user
//

static void sqlite3_ext_getgidfunc (sqlite3_context *context, int argc, sqlite3_value **argv)
{
    // Make sure we got no arguments

    assert (argc == 0);

    // This is rerally simple - just set the result to the group id (GID)
    // value obtained by a call to getgid ()

    sqlite3_result_int (context, (int) getgid ());
}

// static void sqlite3_ext_getegidfunc (sqlite3_context *context, int argc, sqlite3_value **argv)
//
// This function is the implementation of the SQLite3 extension function
// GETEGID(). The function takes no arguments and returns the effective
// GID (EGID) of the current user
//

static void sqlite3_ext_getegidfunc (sqlite3_context *context, int argc, sqlite3_value **argv)
{
    // Make sure we got no arguments

    assert (argc == 0);

    // This is rerally simple - just set the result to the effectuive GID (EGID)
    // value obtained by a call to getegid ()

    sqlite3_result_int (context, (int) getegid ());
}

// static void sqlite3_ext_getloginfunc (sqlite3_context *context, int argc, sqlite3_value **argv)
//
// This function is the implementation of the SQLite3 extension function
// GETLOGIN(). The function takes no arguments and returns the login name
// of the current user. If an error occurs it returns a NULL value
//

static void sqlite3_ext_getloginfunc (sqlite3_context *context, int argc, sqlite3_value **argv)
{
    char LoginName [MAXLOGNAME +1];

    // Make sure we got no arguments

    assert (argc == 0);
 
    // Call getlogin_r () to get the login name for the current user. We
    // zero out the buffer we use, first

    bzero ((void *) LoginName, (MAXLOGNAME +1));
    if (getlogin_r (LoginName, (MAXLOGNAME +1)) != 0) {
        // An error occurred, so return NULL

        sqlite3_result_null (context);
    }
    else {
        // Copy the login name to the return result

        sqlite3_result_text (context, (const char *) LoginName, (MAXLOGNAME +1), SQLITE_TRANSIENT);
    }
}

// static void sqlite3_ext_getcwdfunc (sqlite3_context *context, int argc, sqlite3_value **argv)
//
// This function is the implementation of the SQLite3 extension function
// GETCWD(). The function takes no arguments and returns the current working
// directory. If an error occurs it returns a NULL value
//

static void sqlite3_ext_getcwdfunc (sqlite3_context *context, int argc, sqlite3_value **argv)
{
    char CurrentWorkingDirectory [MAXPATHLEN +1];

    // Make sure we got no arguments

    assert (argc == 0);
 
    // Call getlcwd () to get the current working directory. We zero out the
    // buffer we use, first

    bzero ((void *) CurrentWorkingDirectory, (MAXPATHLEN +1));
    if (getcwd (CurrentWorkingDirectory, (MAXPATHLEN +1)) == 0) {
        // An error occurred, so return NULL

        sqlite3_result_null (context);
    }
    else {
        // Copy the current working directory to the return result

        sqlite3_result_text (context, (const char *) CurrentWorkingDirectory, (MAXPATHLEN +1), SQLITE_TRANSIENT);
    }
}

// int sqlite3_envfuncs_init (sqlite3 *db, char **pzErrMsg, const sqlite3_api_routines *pApi)
//
// This is the entry function for the library, and registers the extension
// functions
//

#ifdef _WIN32
__declspec(dllexport)
#endif
int sqlite3_sysvalfuncs_init (sqlite3 *db, char **pzErrMsg, const sqlite3_api_routines *pApi)
{
    int rc = SQLITE_OK;
    SQLITE_EXTENSION_INIT2 (pApi);
    (void) pzErrMsg;  /* Unused parameter */

    rc = sqlite3_create_function (db, "getuid", 0, (SQLITE_UTF8 /*| SQLITE_INNOCUOUS*/ | SQLITE_DETERMINISTIC), 0, sqlite3_ext_getuidfunc, 0, 0);
    if (rc == SQLITE_OK)
        rc = sqlite3_create_function (db, "geteuid", 0, (SQLITE_UTF8 /*| SQLITE_INNOCUOUS*/ | SQLITE_DETERMINISTIC), 0, sqlite3_ext_geteuidfunc, 0, 0);
    if (rc == SQLITE_OK)
        rc = sqlite3_create_function (db, "getgid", 0, (SQLITE_UTF8 /*| SQLITE_INNOCUOUS*/ | SQLITE_DETERMINISTIC), 0, sqlite3_ext_getgidfunc, 0, 0);
    if (rc == SQLITE_OK)
        rc = sqlite3_create_function (db, "getegid", 0, (SQLITE_UTF8 /*| SQLITE_INNOCUOUS*/ | SQLITE_DETERMINISTIC), 0, sqlite3_ext_getegidfunc, 0, 0);
     if (rc == SQLITE_OK)
        rc = sqlite3_create_function (db, "getlogin", 0, (SQLITE_UTF8 /*| SQLITE_INNOCUOUS*/ | SQLITE_DETERMINISTIC), 0, sqlite3_ext_getloginfunc, 0, 0);
    if (rc == SQLITE_OK)
        rc = sqlite3_create_function (db, "getcwd", 0, (SQLITE_UTF8 /*| SQLITE_INNOCUOUS*/ | SQLITE_DETERMINISTIC), 0, sqlite3_ext_getcwdfunc, 0, 0);
 
  return rc;
}