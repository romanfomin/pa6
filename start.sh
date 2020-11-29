#!/bin/bash

make

if [ $? -eq 0 ]
then

export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:~/Документы/ВСЕ/Распределённые_вычисления/pa4/";

LD_PRELOAD=~/Документы/ВСЕ/Распределённые_вычисления/pa4/libruntime.so ./a.out --mutexl -p 3 10 10 10
fi
