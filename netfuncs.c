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
** 2026-07-01 John Howie    Original.
*/

/*

Copyright 2026 Howie Consulting Inc.

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
# include <stdio.h>
# include <assert.h>
# include <string.h>
# include <strings.h>
# include <sys/socket.h>
# include <arpa/inet.h>

static const char* CopyrightNotice = "netfuncs.c (c) 2026 Howie Consulting Inc.";

extern int errno;

/* static void sqlite3_ext_ipinsubnet4 (sqlite3_context *context, int argc, sqlite3_value **argv)
**
** This function implements the SQLite function IPINSUBNET4(), which takes two
** or three arguments:
**
**      IP Address (CHAR), Subnet[/CIDR] (CHAR)
**      IP Address (CHAR), Subnet (CHAR), Netmask (CHAR)
**
** In both forms, the IP Address can be any valid IPv4 IP Adddress, whether
** specified as a hexadecimal number or using the dotted notation, with each
** octet separated by dots.
**
** In the first form, which takes two arguments, the Subnet should be in the
** format of up-to four octets, separated by '.', and can be appended by '/'
** and the number of the bits in the CIDR subnet.
**
** In the second form, which takes three arguments, the Subnet should be in the
** form of four octets, separated by '.'. It may also be specified as a
** hexadecimal netmask preceeded by '0x'.
**
** It returns 1 (TRUE) if the IP Address is in the Subnet with the Subnet Mask
** applied, otherwise it returns 0 (FALSE)
*/

static void sqlite3_ext_ipinsubnet4 (sqlite3_context *context, int argc, sqlite3_value **argv)
{
        in_addr_t       ip_address, ip_subnet, ip_netmask;
        struct in_addr  ip_net_subnet;
        int             netmaskbits;
        char            errormsg [256 +1];

        // Check we got two or three arguments, and return if we did not, as we
        // cannot proceed. Make sure that the first argument is not a NULL value

        assert ((argc == 2) || (argc == 3));
        if (sqlite3_value_type (argv[0]) == SQLITE_NULL) return;

        // Get the IP address value provided by the user, and turn it into a
        // number we can work with

        ip_address = ntohl (inet_addr ((const char *) sqlite3_value_text (argv [0])));

        // Now, we need to calculate the subnet. This is easy if the user
        // provided a subnet mask (three arguments)

        if (argc == 3) {
                // The user supplied the netmask. This makes it extremely easy
                // as we just read the subnet and the netmask as-is. Note that
                // we use inet_addr () to read the netmask, as inet_network
                // does not accept 0x<mask> notation!

                ip_subnet = inet_network ((const char *) sqlite3_value_text (argv [1]));
                ip_netmask = ntohl (inet_addr ((const char *) sqlite3_value_text (argv [2])));
        }
        else {
                // We have a network / CIDR notation

                memset (&ip_net_subnet, 0, sizeof (struct in_addr));
                netmaskbits = inet_net_pton (AF_INET, (const char *) sqlite3_value_text (argv [1]), &ip_net_subnet, sizeof (struct in_addr));
                if (netmaskbits == -1) {
                        // An error occurred. All we can do is return an error

                        sprintf (errormsg, "Unable to calculate network and mask for %s, inet_net_pton: %s\n", (const char *) sqlite3_value_text (argv [1]), strerror (errno));
                        sqlite3_result_error (context, errormsg, -1);
                }
 
                // Copy over the subnet and calculate the netmask, based on the
                // number of bits in the network address

                ip_subnet = ntohl (ip_net_subnet.s_addr);
                ip_netmask = (0xFFFFFFFF << (32 - netmaskbits));
        }

        // Now that we have the IP Address, the Subnet, and the Netmask values,
        // we can check if the IP Adddress is in the Subnet

        sqlite3_result_int (context, ((ip_address & ip_netmask) == (ip_subnet & ip_netmask)) ? 1 : 0);
}

/* int sqlite3_envfuncs_init (sqlite3 *db, char **pzErrMsg, const sqlite3_api_routines *pApi)
**
** This is the entry function for the library, and registers the extension
** functions
*/

#ifdef _WIN32
__declspec(dllexport)
#endif
int sqlite3_netfuncs_init (sqlite3 *db, char **pzErrMsg, const sqlite3_api_routines *pApi)
{
        int rc = SQLITE_OK;
        SQLITE_EXTENSION_INIT2 (pApi);
        (void) pzErrMsg;  /* Unused parameter */

        rc = sqlite3_create_function (db, "ip4innet", 2, (SQLITE_UTF8 | SQLITE_INNOCUOUS | SQLITE_DETERMINISTIC), 0, sqlite3_ext_ipinsubnet4, 0, 0);
        if (rc == SQLITE_OK)
                rc = sqlite3_create_function (db, "ip4innet", 3, (SQLITE_UTF8 | SQLITE_INNOCUOUS | SQLITE_DETERMINISTIC), 0, sqlite3_ext_ipinsubnet4, 0, 0);

        return rc;
}