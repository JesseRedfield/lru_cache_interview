gcc -o sqlite3 sqlite.c shell.c
g++ -o lrucache main.cpp -l sqlite3