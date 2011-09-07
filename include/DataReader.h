/** \file DataReader.h
 * Class for interfacing with sqlite3 databases
 * Written: S.V. Paulauskas - 09 Sept 2011
 */
#ifndef __DATAREADER_H_
#define __DATAREADER_H_ 1

#include <map>
#include <string>

#include <ctime>

#include "sqlite3.h"

class DataReader : public DatabaseInterface
{
 public:
   DataReader();
   ~DataReader();

 private:
   std::time_t GetModTime(const std::string &fileName);
   std::vector<std::string> GetComment(std::string &line, const int &lineNo,
				       const std::string &fileName);
   void CommandSizeCheck(const std::string &fileName, 
			 const std::vector<std::string> &values, 
			 const int &lineNo);
   void CompareModTimes(void);
   void ReadInformation(const std::string &fileName);
   void UpdateModTimes(const std::string &fileName, const std::time_t &modTime);
   
   std::map<std::string, std::time_t> modTimes_;
}; //class DatabaseInterface
#endif //__DATAREADER_H_
