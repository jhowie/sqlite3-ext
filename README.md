# sqlite3-ext
SQLite3 Extensions

Various SQLLite3 extensions to add functionality.

envfuncs.c - Functions to read environment variables. Currently includes
             the function GETENV(). Use in a SQL query to get an environment
             variable, e.g. SELECT GETENV('USER');.
             
Compile functions into a shared library on your platform or choice, e.g.:

    cc -o envfuncs.dylib -dynamiclib envfuncs.c
    
Load extensions into SQLite3 with the '.load' command.

