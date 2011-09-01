/*************************************************
 ** OutputInterface.cpp
 ** Class to interface with sqlite3.
 ** Written: S.V. Paulauskas - 30 Aug 2011
 ************************************************/
#include <iostream>

#include "OutputInterface.h"

//********** Callback **********
int OutputInterface::Callback(void *NotUsed, int numElements, char *element[],
				char *columnName[])
{
   for(int i = 0; i < numElements; i++){
      std::cout << columnName[i] << " = " << element[i] << std::endl;
   }
   std::cout << std::endl;
   return 0;
}


OutputInterface::OutputInterface(const std::string &databaseName) : 
   DatabaseInterface(databaseName)
{
   database_ = DatabaseInterface::database;
   std::cout << databaseName << " " << "OH YEAHHHHH" << std::endl;
   
   char *errorMessage;
   int returnCode = sqlite3_exec(database_, "select * from generalInfo", DatabaseInterface::Callback, 0, &errorMessage);
}

OutputInterface::~OutputInterface()
{
}
