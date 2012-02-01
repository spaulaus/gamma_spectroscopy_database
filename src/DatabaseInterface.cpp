/*************************************************
 ** DatabaseInterface.cpp
 ** Class to interface with sqlite3.
 ** Written: S.V. Paulauskas - 30 Aug 2011
 ************************************************/
#include <iostream>

#include <cstdlib>

#include "DatabaseInterface.h"

using std::cout;
using std::endl;
using std::string;
using std::vector;

string DatabaseInterface::filePath_;
string DatabaseInterface::databaseName_;
sqlite3 *DatabaseInterface::database_;

//********** ExecuteCommands **********
void DatabaseInterface::ExecuteCommand(const vector<string> &command)
{
   char *errorMessage = 0;
   for(unsigned int i = 0; i < command.size(); i++) {
      int rc = sqlite3_exec(database_, command[i].c_str(), NULL, 
    			    NULL, &errorMessage);
      if(rc != SQLITE_OK) {
    	 cout << "Error executing a sqlite command."
    	      << endl << "ERROR: " << errorMessage << endl;
    	 sqlite3_free(errorMessage);
    	 exit(1);
      } 
   }
}

//********** SetFilePath **********
void DatabaseInterface::SetFilePath(void)
{
   size_t found = databaseName_.find_last_of("/\\");
   filePath_ = databaseName_.substr(0,found);
}


//********** OpenDatabase **********
void DatabaseInterface::OpenDatabase(const string &name)
{
   databaseName_ = name;
   SetFilePath();
   int returnCode = sqlite3_open_v2(databaseName_.c_str(), &database_, 
				    SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, 
				    "unix");
   if(returnCode) {
      cout << "Cannot open database named " << databaseName_ << endl
	   << "Sqlite3 error message: " << sqlite3_errmsg(database_) << endl;
      sqlite3_close(database_);
      exit(1);
   }else
      cout << "Successfully opened database: " << databaseName_ << endl;
}


//********** QueryDatabase **********
void DatabaseInterface::QueryDatabase(const vector<string> &command)
{
   requestedData_.clear();
   sqlite3_stmt *statement; 
   for(unsigned int i = 0; i < command.size(); i++) {
      bool prepared = 
	 sqlite3_prepare_v2(database_, command.at(i).c_str(), 
			    -1, &statement, 0) == SQLITE_OK;
      if(prepared) {
	 int noColumns = sqlite3_column_count(statement);
	 while (true) {
	    int result = sqlite3_step(statement);
	    if(result == SQLITE_ROW) {
	       for(int col = 0; col < noColumns; col++) { 
		  string name = (char*)sqlite3_column_name(statement, col);
		  string value = (char*)sqlite3_column_text(statement, col);
		  requestedData_.push_back(make_pair(name, value));
	       }
	    }else {
	       break;
	    }
	 }//while(true)
	 sqlite3_finalize(statement);
      }//if(prepared)
   }
}
