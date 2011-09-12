/*************************************************
 ** \file DatabaseOutput.cpp
 ** Class to output information with sqlite3
 **   databases.
 ** Written: S.V. Paulauskas - 30 Aug 2011
 ************************************************/
#include <algorithm>
#include <iostream>
#include <sstream>

#include <cstdlib>

#include "DatabaseOutput.h"

using namespace std;

//********** BuildCommand **********
string DatabaseOutput::BuildCommand(void)
{
   stringstream command;
   switch(gammas_.size()) {
   case 0:
      cout << "The vector containing the gammas for search is empty." 
	   << "  This is a big problem.  Exiting... " << endl;
      exit(1);
      break;
   case 1:
      command << "select * from " << table_ << " where Gamma=" 
	      << gammas_.at(0);
      break;
   case 2:
      if(table_ == "range")
	 command << "select Gamma from generalInfo where Gamma>="
		 << gammas_.at(0) << " and Gamma<=" << gammas_.at(1);
      else
	 command << "select * from " << table_ << " where Gamma>="
		 << gammas_.at(0) << " and Gamma<=" << gammas_.at(1);
      break;
   default:
      cout << "Problems constructing the sql command for output." << endl;
      exit(1);
      break;
   }
   return (command.str());
}


//********** DatabaseOutput **********
DatabaseOutput::DatabaseOutput()
{
}


//********** ~DatabaseOutput **********
DatabaseOutput::~DatabaseOutput()
{
}


//********** OutputHeading **********
void DatabaseOutput::OutputHeading(void)
{
   if(table_ == "generalInfo"){
      cout << "General Information" << endl 
	   << "---------------------------" << endl;
   }else if(table_ == "coinInfo"){
      cout << "Coincidence Information" << endl 
	   << "---------------------------" << endl;
   }else if(table_ == "fitInfo") {
      cout << "Fit Information" << endl 
	   << "---------------------------" << endl;
   }else if(table_ == "range") {
      cout << "Gammas found in this range: ";
   }
}


//********** OutputInformation **********
void DatabaseOutput::OutputInformation(void)
{
   int area = 0;

   if(table_ == "coincidences") {
      sort(coincidences_.begin(), coincidences_.end());
      cout << "These Gammas are in coincidence: ";
      for(vector<int>::iterator it = coincidences_.begin();
	  it != coincidences_.end(); it++) {
	 cout << *it << ",";
      }
      cout << endl;
   }else {
      for(vector<pair<string,string> >::iterator it = data_.begin(); 
	  it != data_.end(); it++){
	 if(table_ == "range") {
	    cout << (*it).second << ", ";
	 }else if((*it).first == "Comments") {
	    cout << (*it).first << " = " << (*it).second << endl << endl;
	 }else{
	    if((*it).first == "Area") {
	       area = atoi((*it).second.c_str());
	       cout << (*it).first << " = " << (*it).second;
	    } else if((*it).first == "PercentErr") {
	       cout << " +- " << area * (atof((*it).second.c_str())/100) << endl;
	    } else {
	       cout << (*it).first << " = " << (*it).second << endl;
	    }
	 }
      }
   }
   cout << endl;
}


//********** RetrieveInformation **********
void DatabaseOutput::RetrieveInformation(const std::string &table)
{
   table_ = table;
   
   string command = BuildCommand();
   sqlite3_stmt *statement;
   bool prepared = 
      sqlite3_prepare_v2(database, command.c_str(), 
   			 -1, &statement, 0) == SQLITE_OK;
   
   if(prepared) {
      int noColumns = sqlite3_column_count(statement);

      while(true) {
      	 int result = sqlite3_step(statement);
      	 if(result == SQLITE_ROW) {
      	    for(int col = 0; col < noColumns; col++) {
      	       string colName = (char*)sqlite3_column_name(statement,col);
      	       string colValue = (char*)sqlite3_column_text(statement, col);
	       if(table == "coincidences" && col == 1){
		  coincidences_.push_back(atoi(colValue.c_str()));
	       }else
		  data_.push_back(make_pair(colName, colValue));
   	    }
      	 }else {
   	    break;   
      	 }
      }//while(true)
      sqlite3_finalize(statement);
   }//if(prepared)
   OutputHeading();
   OutputInformation();
   data_.clear();
   coincidences_.clear();
}


//********** Verbosity Handler **********
void DatabaseOutput::VerbosityHandler(const vector<string> &gammas,
				       const string &verbosity)
{
   gammas_ = gammas;

   if(verbosity == "-r")
      RetrieveInformation("range");
   else
      RetrieveInformation("generalInfo");
   
   if(verbosity == "-c") {
      RetrieveInformation("coinInfo");
      RetrieveInformation("coincidences");
   }else if(verbosity == "-f") {
      RetrieveInformation("fitInfo");
   }else if(verbosity == "-v") {
      RetrieveInformation("fitInfo");
      RetrieveInformation("coinInfo");
      RetrieveInformation("coincidences");
   }
}
