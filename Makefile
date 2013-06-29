#!/bin/make
SHELL=/bin/bash

#Define the virtual paths
vpath %.cpp src/
vpath %.hpp include/
vpath %.o obj/

#Set some paths to install the shared objects to
SO_INSTALL_PATH = /home/vincent/programs/libFiles
HEADER_INSTALL_PATH = /home/vincent/programs/headerFiles

#Set some of the compile options
CXX = g++
CXXFLAGS = -g -Wall -Dsqlite_enable_rtree=1 $(CINCLUDEDIRS)
LDLIBS = -ldl -lpthread -lsqlite3
CINCLUDEDIRS = -Iinclude
c++SrcSuf = cpp

#Set the name of the program to be compiled
PROGRAM = database
VERSION = $(shell git describe --abbrev=0 --tags)

#Define Objects
DATABASEINPUTO = DatabaseInput.o
DATABASEINTERFACEO = DatabaseInterface.o
DATABASEOUTPUTO = DatabaseOutput.o
MAINO = database.o

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

.PHONY: clean so
clean: 
	@echo "Cleaning..."
	@rm -f $(OBJDIR)/* $(PROGRAM) *~ src/*~ include/*~
