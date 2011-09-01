/***********************************************************
 ** \file database.cpp
 ** File to handle communicate with sqlite databases
 ** Written: S.V. Paulauskas - 30 Aug 2011
 **********************************************************/
#include <iostream>

#include <cstdlib>

#include "DatabaseInterface.h"
//#include "OutputInterface.h"
#include "sqlite3.h"

using std::cout;
using std::endl;

int main(int argc, char* argv[])
{
   DatabaseInterface interface(argv[1]);
   //OutputInterface interface(argv[1]);


   if(argc != 3) {
      cout << "Usage: " << argv[0] << " DATABASE SQL-STATEMENT" << endl;
      exit(1);
   }

   interface.OpenDatabase();

   //char *errorMessage;
   //int returnCode = sqlite3_exec(database, argv[2], interface.Callback, 0, &errorMessage);

   // if(returnCode != SQLITE_OK) {
   //    cout << "SQL error: " << errorMessage << endl;
   //    sqlite3_free(errorMessage);
   // }
}
