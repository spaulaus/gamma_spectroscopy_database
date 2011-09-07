/*************************************************
 ** OutputInterface.cpp
 ** Class to interface with sqlite3.
 ** Written: S.V. Paulauskas - 30 Aug 2011
 ************************************************/
#include <iostream>
#include <string>

#include <cstdlib>

#include "OutputInterface.h"

using std::cout;
using std::endl;
using std::string;
using std::vector;

//********** OutputInformation **********
void OutputInterface::OutputInformation(const vector<string> &gammas, 
					const std::string &table)
{
   sqlite3_stmt *statement;
   
   string command = "select ";

   if(gammas.size() == 0) { 
      cout << "Problem with the gamma range." << endl;
      exit(1);
   }else if(gammas.size() == 1) {
      command.append("* from ");
      command.append(table);
      command.append(" where gamma=");
      command.append(gammas.at(0));
   }else if(gammas.size() == 2 && table == "range") {
      command.append("gamma from generalInfo where gamma>=");
      command.append(gammas.at(0));
      command.append(" and gamma<=");
      command.append(gammas.at(1));      
   }else if(gammas.size() == 2 && table == "generalInfo") {
      command.append("* from ");
      command.append(table);
      command.append(" where gamma>=");
      command.append(gammas.at(0));
      command.append(" and gamma<=");
      command.append(gammas.at(1));      
   }else {
      cout << "Problems constructing the sql command." << endl;
      exit(1);
   }

   cout << command << endl;

   bool prepared = 
      sqlite3_prepare_v2(DatabaseInterface::database, command.c_str(), 
			 -1, &statement, 0) == SQLITE_OK;

   if(prepared) {
      int noColumns = sqlite3_column_count(statement);
      int result = 0;
      
      if(table == "generalInfo"){
	 cout << "General Information" << endl 
	      << "---------------------------" << endl;
      }else if(table == "coinInfo"){
	 cout << "Coincidence Information" << endl 
	      << "---------------------------" << endl;
      }else if(table == "fitInfo") {
	 cout << "Fit Information" << endl 
	      << "---------------------------" << endl;
      }else if(table == "range") {
	 cout << "Gammas found in this range: " << endl;
      }else {
	 cout << "Problem with the table definition" << endl;
	 exit(1);
      }
      
      while(true) {
      	 result = sqlite3_step(statement);
      	 if(result == SQLITE_ROW) {
      	    for(int col = 0; col < noColumns; col++) {
      	       string colName = (char*)sqlite3_column_name(statement,col);
      	       string colValue = (char*)sqlite3_column_text(statement, col);
      	       cout << colName << " : " << colValue << endl;
      	    }
      	    cout << endl;
      	 }else {
      	    break;   
      	 }
      }//while(true)
      sqlite3_finalize(statement);
   }//if(prepared)
}


//********** Verbosity Handler **********
void OutputInterface::VerbosityHandler(const vector<string> &gammas,
				       const string &verbosity)
{
   OutputInformation(gammas, "generalInfo");

   if(verbosity == "-c") {
      OutputInformation(gammas, "coinInfo");
   }else if(verbosity == "-f") {
      OutputInformation(gammas, "fitInfo");
   }else if(verbosity == "-v") {
      OutputInformation(gammas, "coinInfo");
      OutputInformation(gammas, "fitInfo");
   }
}


//********** OutputInterface **********
OutputInterface::OutputInterface()
{
}


//********** ~OutputInterface **********
OutputInterface::~OutputInterface()
{
}
