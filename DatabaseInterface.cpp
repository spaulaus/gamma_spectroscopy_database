/*************************************************
 ** DatabaseInterface.cpp
 ** Class to interface with sqlite3.
 ** Written: S.V. Paulauskas - 30 Aug 2011
 ************************************************/
#include <iostream>

#include <cstdlib>

#include "DatabaseInterface.h"

using std::cout;
using std::endl;

std::string DatabaseInterface::databaseName_;

//********** Callback **********
int DatabaseInterface::Callback(void *NotUsed, int numElements, char *element[],
				char *columnName[])
{
   for(int i = 0; i < numElements; i++){
      cout << columnName[i] << " = " << element[i] << endl;
   }
   cout << endl;
   return 0;
}


//********** DatabaseInterface **********
DatabaseInterface::DatabaseInterface(const std::string &databaseName)
{
   databaseName_ = databaseName;
}


//********** ~DatabaseInterface() **********
DatabaseInterface::~DatabaseInterface()
{
   sqlite3_close(database_);
}



//********** ExecuteCoincidenceQuery **********
//********** ExecuteFitQuery **********
//********** ExecuteGeneralQuery **********
void DatabaseInterface::ExecuteGeneralQuery()
{
}


//********** OpenDatabase **********
void DatabaseInterface::OpenDatabase(void)
{
   int returnCode = sqlite3_open(databaseName_.c_str(), &database_);
   char *errorMessage;

   if(returnCode) {
      cout << "Cannot open database named " << databaseName_ << endl
	   << "Sqlite3 error message: " << sqlite3_errmsg(database_);
      sqlite3_close(database_);
      exit(1);
   } else {
      cout << "Successfully opened database: " << databaseName_ << endl;
   }

   returnCode = sqlite3_exec(database_, "select * from generalInfo", &DatabaseInterface::Callback, 0, &errorMessage);

}
