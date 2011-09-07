/** \file OutputInterface.h
 * Class for outputting with sqlite3 databases
 * Written: S.V. Paulauskas - 30 Aug 2011
 */
#ifndef __OUTPUTINTERFACE_H_
#define __OUTPUTINTERFACE_H_ 1

#include <vector>

#include "DatabaseInterface.h"

class OutputInterface : public DatabaseInterface
{
 public:
   OutputInterface();
   ~OutputInterface();

   void OutputInformation(const std::vector<std::string> &gammas, 
			  const std::string &table);
   void OutputRange(const std::vector<std::string> &gammas, 
		    const std::string &verbosity);
   void VerbosityHandler(const std::vector<std::string> &gammas, 
			 const std::string &verbosity);
};
#endif //__OUTPUTINTERFACE_H_
