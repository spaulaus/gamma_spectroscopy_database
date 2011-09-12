/** \file DatabaseOutput.h
 * Class for outputting with sqlite3 databases
 * Written: S.V. Paulauskas - 30 Aug 2011
 */
#ifndef __DATABASEOUTPUT_H_
#define __DATABASEOUTPUT_H_ 1

#include "DatabaseInterface.h"

class DatabaseOutput : public DatabaseInterface
{
 public:
   DatabaseOutput();
   ~DatabaseOutput();

   void VerbosityHandler(const std::vector<std::string> &gammas, 
			 const std::string &verbosity);

 private:
   std::string BuildCommand(void);
   void OutputHeading(void);
   void OutputInformation(void);
   void RetrieveInformation(const std::string &table);

   std::vector<std::string> gammas_;
   std::vector<int> coincidences_;
   std::string table_;
   std::vector<std::pair<std::string, std::string> > data_;
};
#endif //__DATABASEOUTPUT_H_
