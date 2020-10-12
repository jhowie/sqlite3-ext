# sqlite3-ext
SQLite3 Extensions

Various SQLLite3 extensions to add functionality.

envfuncs.c    - Functions to use environment variables. Currently includes
                the functions GETENV(), GETENVLEN(), GETENVEXISTS(), and
                GETENVDEFAULT(). Use in a SQL query to get an environment
                variable, e.g. SELECT GETENV('USER');.

sysvalfuncs.c - Functions to get various values from the system. Includes the
                functions GETUID(), GETEUID(), GETGID(), GETEGID(), GETLOGIN(),
                and GETCWD(). Use in a SQL query to get the value, e.g.
                SELECT GETLOGIN();
             
Compile functions into a shared library on your platform or choice, e.g.:

    cc -o envfuncs.dylib -dynamiclib envfuncs.c
    cc -o sysvalfuncs.dylib -dynamiclib sysvalfuncs.c
    
Load extensions into SQLite3 with the '.load' command.

