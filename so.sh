#!/bin/bash

g++ -c -fPIC libtest.cpp -I include
g++ -shared libtest.o -o libtest.so -lgreencloud -lglobals -I include
rm libtest.o
