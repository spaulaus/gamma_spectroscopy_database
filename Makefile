#!/bin/make
SHELL=/bin/bash

CXX = g++
CXXFLAGS += -Wall -Dsqlite_enable_rtree=1
LDLIBS += -lsqlite3 -ldl -lpthread
c++SrcSuf = cpp

#Define Objects
MAINO = database.o
DATABASEINTERFACEO = DatabaseInterface.o
OUTPUTINTERFACEO = OutputInterface.o

#List Objects
OBJS = $(MAINO) $(DATABASEINTERFACEO) $(OUTPUTINTERFACEO)

PROGRAM = database

.SUFFIXES: .$(c++SrcSuf)

.phony: all clean
all: $(PROGRAM)

clean: 
	@echo "Cleaning..."
	@rm -f $(OBJS) $(PROGRAM)

$(PROGRAM): $(OBJS) 
	$(CXX) $(LDLIBS) $^ -o $@