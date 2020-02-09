#!/bin/bash

g++ -c -fPIC libtest.cpp -I include
g++ -shared libtest.o -o libtest.so -I include
rm libtest.o
