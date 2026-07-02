# sqlite3-ext
SQLite3 Extensions

Various SQLLite3 extensions to add functionality.

envfuncs.c    - Functions to use environment variables. Currently includes
                the functions GETENV(), GETENVLEN(), GETENVEXISTS(), and
                GETENVDEFAULT(). Use in a SQL query to get an environment
                variable, e.g. SELECT GETENV('USER');.

netfuncs.c    - Network-related functions. Currently includes IP4INNET(),
                which can be used to check if an IPv4 address is in a given
                subnet. It comes in two forms IP4INNET([IP Address],
                [Subnet/CIDR]) and IP4INNET([IP Address], [Subnet], [Netmask]).
                In the second (three argument) form, [Netmask] can be specified
                in dotted notation (e.g. 255.255.255.0) or hexadecimal (e.g.
                0xffffff00). It returns 1 if [IP Address] is in [Subnet], and
                0 if it does not.

sysvalfuncs.c - Functions to get various values from the system. Includes the
                functions GETUID(), GETEUID(), GETGID(), GETEGID(), GETLOGIN(),
                and GETCWD(). Use in a SQL query to get the value, e.g.
                SELECT GETLOGIN();
             
Compile functions into a shared library on your platform or choice, e.g.:

    cc -o envfuncs.dylib -dynamiclib envfuncs.c -lsqlite3
    cc -o sysvalfuncs.dylib -dynamiclib sysvalfuncs.c -lsqlite3
    cc -o netfuncs.dylib -dynamiclib netfuncs.c -lsqlite3

NOTE:  You may need to use a library other than sqlite3. On MacOS you may need
       to download the entire SQLite3 source tree, compile it, and install it,
       so that you can link against libsqlite3.dylib, as later MacOS SDKs
       contain only tbd files that point to libraries, and not the libraries
       themselves
    
Load extensions into SQLite3 with the '.load' command.

