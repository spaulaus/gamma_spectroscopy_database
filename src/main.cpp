/***********************************************************
 ** \file database.cpp
 ** File to handle communication with sqlite databases
 ** Written: S.V. Paulauskas - 30 Aug 2011
 **********************************************************/
#include <iostream>

#include <cstdlib>

#include <sqlite3.h>
#include <unistd.h>

#include "DatabaseInput.hpp"
#include "DatabaseInterface.hpp"
#include "DatabaseOutput.hpp"

using namespace std;

int main(int argc, char* argv[]) {
   vector<string> gammas, outputs;
   int opt;

   if(argc < 3) {
      DatabaseOutput helpNeeded;
      helpNeeded.OutputHelpInfo();
      exit(1);
   }

   while((opt = getopt (argc, argv, "hgcfvn:")) != -1)
      switch(opt) {
      case 'h': {
          DatabaseOutput helpNeeded;
          helpNeeded.OutputHelpInfo();
          break; }
      case 'g':
          outputs.push_back("generalInfo");
          break;
      case 'c':
         outputs.push_back("generalInfo");
         outputs.push_back("coinInfo");
         outputs.push_back("coincidences");
         break;
      case 'f':
         outputs.push_back("generalInfo");
         outputs.push_back("fitInfo");
         break;
      case 'v':
         outputs.push_back("generalInfo");
         outputs.push_back("fitInfo");
         outputs.push_back("coinInfo");
         outputs.push_back("coincidences");
         break;
      case 'n': {
         string databaseName = optarg;
         DatabaseInterface interface;
         interface.OpenDatabase(databaseName);
         DatabaseInput createDatabase;
         createDatabase.SetDatabaseName(databaseName);
         createDatabase.FillDatabase();
         interface.CloseDatabase();
         exit(1); }
         break;
      case '?':
         DatabaseOutput helpNeeded;
         helpNeeded.OutputHelpInfo();
         break;
      }

   string databaseName = argv[optind];
   for(int i = optind+1; i < argc; i++)
      gammas.push_back(argv[i]);

   if(outputs.empty() && gammas.size() == 1)
      outputs.push_back("generalInfo");
   else if (outputs.empty() && gammas.size() == 2)
      outputs.push_back("range");

   //Open the connection to the database
   DatabaseInterface interface;
   interface.OpenDatabase(databaseName);

   //Check the existence and modification time of the data files.
   //Fill the database if necessary.
   DatabaseInput fillDatabase;
   DatabaseOutput output;
   output.ParseData(outputs, gammas);
   interface.CloseDatabase();
}
