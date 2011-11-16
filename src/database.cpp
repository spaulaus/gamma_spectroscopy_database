/***********************************************************
 ** \file database.cpp
 ** File to handle communication with sqlite databases
 ** Written: S.V. Paulauskas - 30 Aug 2011
 **********************************************************/
#include <iostream>
#include <vector>

#include <cstdlib>

#include "DatabaseInput.h"
#include "DatabaseInterface.h"
#include "DatabaseOutput.h"
#include "sqlite3.h"

using std::cout;
using std::endl;
using std::string;
using std::vector;

int main(int argc, char* argv[])
{
   vector<string> gammas;

   //Open the connection to the database
   DatabaseInterface openInterface(argc);
   string databaseName = argv[1];
   if(databaseName == "-n") {
      openInterface.CreateDatabase(argv[2]);
      
      exit(1);
   }else {
      openInterface.OpenDatabase(databaseName);
   }
   
   //Check the existence and modification time of the data files.
   //Fill the database if necessary.
   DatabaseInput fillDatabase;
   //Start the output interface.
   DatabaseOutput outputHandling;
   
   switch(argc) {
   case 3:
      if (atoi(argv[2]) != 0) {
   	 gammas.push_back(argv[2]);
   	 outputHandling.VerbosityHandler(gammas, "-g");
   	 exit(1);
      }else
   	 openInterface.OutputHelpInfo();
      break;
   case 4: 
      if(atoi(argv[2]) != 0 &&  atoi(argv[3]) != 0) {
   	 gammas.push_back(argv[2]);
   	 gammas.push_back(argv[3]);
   	 outputHandling.VerbosityHandler(gammas, "-r");
   	 exit(1);
      }else if (atoi(argv[2]) == 0 &&  atoi(argv[3]) != 0) {
   	 gammas.push_back(argv[3]);
   	 outputHandling.VerbosityHandler(gammas, argv[2]);
      }else 
   	 openInterface.OutputHelpInfo();
      break;
   case 5:
      if(atoi(argv[2]) == 0 && atoi(argv[3]) != 0 
   	 && atoi(argv[4]) != 0) {
   	 gammas.push_back(argv[3]);
   	 gammas.push_back(argv[4]);
   	 outputHandling.VerbosityHandler(gammas, "-g");
      }else
   	 openInterface.OutputHelpInfo();
      break;
   default:
      openInterface.OutputHelpInfo();
   }//switch(argc)
}
