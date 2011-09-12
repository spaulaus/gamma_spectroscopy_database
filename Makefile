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
DATABASEINPUTO = DatabaseInput.o
DATABASEINTERFACEO = DatabaseInterface.o
DATABASEOUTPUTO = DatabaseOutput.o
MAINO = database.o

#List Objects
OBJS = $(MAINO) $(DATABASEINTERFACEO) $(DATABASEOUTPUTO) $(DATABASEINPUTO)

PROGRAM = database

.SUFFIXES: .$(c++SrcSuf)

.phony: all clean
all: $(PROGRAM)

clean: 
	@echo "Cleaning..."
	@rm -f $(OBJS) $(PROGRAM) *~ src/*~ include/*~

$(PROGRAM): $(OBJS) 
	$(CXX) $(LDLIBS) $^ -o $@