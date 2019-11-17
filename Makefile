#!/bin/make
SHELL=/bin/bash

#Define the virtual paths
vpath %.cpp src/
vpath %.hpp inc/
vpath %.o obj/

#Set some of the compile options
CXX = g++
CXXFLAGS = -fPIC -g -std=c++11 -Wall $(CINCLUDEDIRS) -Dsqlite_enable_rtree=1
LDLIBS = -ldl -lpthread -lsqlite3
CINCLUDEDIRS = -Iinc
c++SrcSuf = cpp

#Set the name of the program to be compiled
PROGRAM = gammaDatabase
VERSION = $(shell git describe --abbrev=0 --tags)

#Define Objects
DATABASEINPUTO = DatabaseInput.o
DATABASEINTERFACEO = DatabaseInterface.o
DATABASEOUTPUTO = DatabaseOutput.o
MAINO = main.o

#Make the object list and prefix the object directory
OBJS = $(MAINO) $(DATABASEINTERFACEO) $(DATABASEINPUTO) $(DATABASEOUTPUTO)
OBJDIR = obj
OBJS_W_DIR = $(addprefix $(OBJDIR)/,$(OBJS))

.SUFFIXES: .$(c++SrcSuf)

all: $(OBJS_W_DIR) $(PROGRAM)

$(OBJS_W_DIR): | $(OBJDIR)

$(OBJDIR):
	mkdir $(OBJDIR)

$(PROGRAM): $(OBJS_W_DIR)
	$(CXX) $(CXXFLAGS) $(LDLIBS) $^ -o $@

$(OBJDIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	@echo "Cleaning..."
	@rm -f $(OBJDIR)/*.o $(PROGRAM) *~ src/*~ inc/*~ data/*~
