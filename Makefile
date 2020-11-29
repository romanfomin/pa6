all:
	clang-4.0 -std=c99 -Wall -pedantic *.c -L. -lruntime