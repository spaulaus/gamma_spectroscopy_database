/** \file DatabaseInterface.h
 * Class for interfacing with sqlite3 databases
 * Written: S.V. Paulauskas - 30 Aug 2011
 */
#ifndef __DATABASEINTERFACE_H_
#define __DATABASEINTERFACE_H_ 1

#include <string>
#include <vector>

#include "sqlite3.h"

class DatabaseInterface
{
 public:
   DatabaseInterface(){};
   ~DatabaseInterface(){};

   void CloseDatabase(){sqlite3_close(database_);};
   void OpenDatabase(const std::string &name);
   void SetFilePath(void);
   void ExecuteCommand(const std::vector<std::string> &command);
   void QueryDatabase(const std::vector<std::string> &command);
   
   std::string GetFilePath(void) {return filePath_;};
   std::string GetDatabaseName(void) {return databaseName_;};
   std::vector<std::pair<std::string, std::string> >* GetRequestedData(void)
      {return &requestedData_;};

 private:
   static std::string filePath_;
   static std::string databaseName_;
   static sqlite3 *database_;

   std::vector<std::pair<std::string, std::string> > requestedData_;
}; //class DatabaseInterface
#endif //__DATABASEINTERFACE_H_
