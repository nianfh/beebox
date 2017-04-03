#!/bin/sh

basepath=$(cd `dirname $0`; pwd)
echo $basepath

# make libuv
if [ ! -f "lib/libuv/v1.11.0.zip" ]; then
    echo "downloading https://github.com/libuv/libuv/archive/v1.11.0.zip"
    wget https://github.com/libuv/libuv/archive/v1.11.0.zip -P lib/libuv
fi
cd lib/libuv
if [ ! -d "libuv-1.11.0" ]; then
    unzip -o v1.11.0.zip
fi 
cd libuv-1.11.0
./autogen.sh
./configure --prefix=$basepath/lib/libuv
make && make install
cd $basepath
cp lib/libuv/include/* ../include/libuv
cp lib/libuv/lib/libuv.a ../lib

# make http-parser
if [ ! -f "lib/http_parser/v2.7.1.zip" ]; then
    echo "https://github.com/nodejs/http-parser/archive/v2.7.1.zip"
    wget https://github.com/nodejs/http-parser/archive/v2.7.1.zip -P lib/http_parser
fi
cd lib/http_parser
if [ ! -d "http-parser" ]; then
    unzip -o v2.7.1.zip
    mv http-parser-2.7.1 http-parser
fi
cd http-parser
cmake ..
make
cd $basepath
cp lib/http_parser/http-parser/http_parser.h ../include/http_parser
cp lib/http_parser/http-parser/libhttp_parser.a ../lib

# make sqlite3
if [ ! -f "lib/sqlite3/sqlite-amalgamation-3170000.zip" ]; then
    echo "downloading http://www.sqlite.org/2017/sqlite-amalgamation-3170000.zip"
    wget http://www.sqlite.org/2017/sqlite-amalgamation-3170000.zip -P lib/sqlite3
fi
cd lib/sqlite3
if [ ! -d "src" ]; then
    unzip -o sqlite-amalgamation-3170000.zip
    mv sqlite-amalgamation-3170000 src
fi
cd src
cmake ..
make
cp libsqlite3.a ..
cd $basepath
mv lib/sqlite3/libsqlite3.a ../lib
cp lib/sqlite3/src/sqlite3.h ../include/sqlite3

# make beebox
cmake ..
make

