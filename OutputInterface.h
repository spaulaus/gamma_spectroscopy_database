/** \file OutputInterface.h
 * Class for outputting with sqlite3 databases
 * Written: S.V. Paulauskas - 30 Aug 2011
 */
#ifndef __OUTPUTINTERFACE_H_
#define __OUTPUTINTERFACE_H_ 1

#include "DatabaseInterface.h"

class OutputInterface : public DatabaseInterface
{
 public:
   OutputInterface(const std::string &databaseName);
   ~OutputInterface();

 private:
   sqlite3 *database_;
};
#endif //__OUTPUTINTERFACE_H_
