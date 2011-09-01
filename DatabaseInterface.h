/** \file DatabaseInterface.h
 * Class for interfacing with sqlite3 databases
 * Written: S.V. Paulauskas - 30 Aug 2011
 */
#ifndef __DATABASEINTERFACE_H_
#define __DATABASEINTERFACE_H_ 1

#include <string>

#include "sqlite3.h"

class DatabaseInterface
{
 public:
   DatabaseInterface(const std::string &databaseName);
   ~DatabaseInterface();

   static int Callback(void *NotUsed, int numElements, char *element[],
		       char *columnName[]);
   void OpenDatabase(void);
   void ExecuteGeneralQuery(void);

 private:
   sqlite3 *database_;
   static std::string databaseName_;
}; //class DatabaseInterface
#endif //__DATABASEINTERFACE_H_
