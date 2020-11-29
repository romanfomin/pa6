#!/bin/bash

make

if [ $? -eq 0 ]
then

export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:~/Projects/distributed/pa6/";

LD_PRELOAD=~/Projects/distributed/pa6/libruntime.so ./a.out --mutexl -p 3 10 10 10
fi
