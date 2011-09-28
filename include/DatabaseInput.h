/** \file DatabaseInput.h
 * Handles filling/updating of the databases.
 * Written: S.V. Paulauskas - 09 Sept 2011
 */
#ifndef __DATABASEINPUT_H_
#define __DATABASEINPUT_H_ 1

#include <map>

#include <ctime>

#include "DatabaseInterface.h"

class DatabaseInput : public DatabaseInterface
{
 public:
   DatabaseInput();
   ~DatabaseInput();

 private:
   std::time_t GetNewModTime(const std::string &fileName);
   std::vector<std::string> BuildCommand(void);
   std::vector<std::string> GetComment(std::string &line);

   void CommandSizeCheck(void);
   void CompareModTimes(void);
   void ExecuteBeginAndEnd(const std::string &command);
   void FillDatabase(void);
   void GetOldModTimes(void);
   void ReadInformation(void);
   void UpdateModTimes(const std::time_t &modTime);
      
   int lineNo_;
   std::map<std::string, std::time_t> newTimes_;
   std::map<std::string, std::time_t> oldTimes_;
   std::string tableName_;
   std::vector<std::string> values_;
   std::vector<std::string> coinGammas_;
}; //class DatabaseInterface
#endif //__DATABASEINPUT_H_

