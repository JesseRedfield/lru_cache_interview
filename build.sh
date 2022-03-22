gcc -o sqlite3 sqlite3.c shell.c
g++ -o lrucache main.cpp -l sqlite3
./lrucache