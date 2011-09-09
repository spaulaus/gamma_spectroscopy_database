/** \file DataReader.h
 * Class for interfacing with sqlite3 databases
 * Written: S.V. Paulauskas - 09 Sept 2011
 */
#ifndef __DATAREADER_H_
#define __DATAREADER_H_ 1

#include <map>

#include <ctime>

#include "DatabaseInterface.h"
#include "sqlite3.h"

class DataReader : public DatabaseInterface
{
 public:
   DataReader();
   ~DataReader();

 private:
   std::string BuildCommand(void);
   std::time_t GetNewModTime(const std::string &fileName);
   std::vector<std::string> GetComment(std::string &line);
   
   void CommandSizeCheck(void);
   void CompareModTimes(void);
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
#endif //__DATAREADER_H_

