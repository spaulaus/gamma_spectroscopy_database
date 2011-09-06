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

   std::time_t GetModTime(const std::string &fileName);
   
   void CompareModTimes(void);
   void UpdateModTimes(void);
 private:
   std::map<std::string, std::time_t> modTimes_;
}; //class DatabaseInterface
#endif //__DATAREADER_H_
