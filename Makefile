#!/bin/make
SHELL=/bin/bash

vpath %.cpp src/
vpath %.h include/

CINCLUDEDIRS = -Iinclude

CXX = g++
CXXFLAGS += -Wall -Dsqlite_enable_rtree=1 $(CINCLUDEDIRS)
LDLIBS += -lsqlite3 -ldl -lpthread
c++SrcSuf = cpp

#Define Objects
MAINO = database.o
DATABASEINTERFACEO = DatabaseInterface.o
OUTPUTINTERFACEO = OutputInterface.o
DATAREADERO = DataReader.o

#List Objects
OBJS = $(MAINO) $(DATABASEINTERFACEO) $(OUTPUTINTERFACEO) $(DATAREADERO)

PROGRAM = database

.SUFFIXES: .$(c++SrcSuf)

.phony: all clean
all: $(PROGRAM)

clean: 
	@echo "Cleaning..."
	@rm -f $(OBJS) $(PROGRAM) *~ src/*~ include/*~

$(PROGRAM): $(OBJS) 
	$(CXX) $(LDLIBS) $^ -o $@