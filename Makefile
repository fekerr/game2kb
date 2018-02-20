# fekerr 20180920
# Super simple basic Makefile for now
#

all: kcl kclp

kcl: kcl.c
	gcc -g -Wall kcl.c -o kcl

kclp: kcl.cpp
	g++ -g -Wall kcl.cpp -o kclp


