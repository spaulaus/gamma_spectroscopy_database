/** \file DatabaseOutput.h
 * Class for outputting with sqlite3 databases
 * Written: S.V. Paulauskas - 30 Aug 2011
 */
#ifndef __DATABASEOUTPUT_H_
#define __DATABASEOUTPUT_H_ 1

#include "DatabaseInterface.hpp"

//Define the comparison Function for sort
bool CoinComparison(const std::string &alpha, const std::string &beta);

class DatabaseOutput : public DatabaseInterface
{
 public:
   DatabaseOutput(){};
   ~DatabaseOutput(){};

   void ParseData(const std::vector<std::string> &tables,
		  const std::vector<std::string> &gammas);
   void OutputHelpInfo(void);
 private:
   DatabaseInterface interface;
   double EfficiencyCalc(const double &area);
   std::vector<std::string> BuildCommand(void);
   void OutputHeading(void);
   void OutputData(void);

   std::vector<std::string> gammas_;
   std::vector<std::string> coincidences_;
   std::string table_;
   std::vector<std::pair<std::string, std::string> > *data_;
   std::vector<std::pair<std::string, std::string> > efficiency_;
};
#endif //__DATABASEOUTPUT_H_
