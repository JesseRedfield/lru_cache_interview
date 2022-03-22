gcc -o sqlite3 sqlite.c shell.c
g++ -o lrucache.exe main.cpp -l sqlite3
lrucache.exe 