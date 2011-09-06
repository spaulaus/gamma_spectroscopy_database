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
   DatabaseInterface();
   DatabaseInterface(const int &noArgs);
   ~DatabaseInterface();

   void CreateDatabase(const std::string &name);
   void OpenDatabase(const std::string &dbName);
   void OutputHelpInfo(void);
   
 protected:
   static sqlite3 *database;
   static std::string databaseName;
}; //class DatabaseInterface
#endif //__DATABASEINTERFACE_H_
