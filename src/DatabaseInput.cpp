/***********************************************************
 ** \file DatabaseInput.cpp
 ** Class that handles the reading of datafiles and filling
 **   of the database.
 ** Written: S.V. Paulauskas - 09 Sept 2011
 **********************************************************/
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

#include <cstdlib>

#include "DatabaseInterface.h"
#include "DatabaseInput.h"
#include "sqlite3.h"
#include "sys/stat.h"

using namespace std;

//********** BuildCommand **********
vector<string> DatabaseInput::BuildCommand(void)
{
   vector<string> tempCommands;
   stringstream command;
   command << "insert or replace into " << tableName_ << " values(";
   for(vector<string>::iterator it = values_.begin(); 
       it != values_.end(); it++) { 
      unsigned int position = it - values_.begin();
      
      if(position == values_.size()-1)
	 command << "'" << *it << "')";
      else
	 command << "'" << *it << "',";
   }

   tempCommands.push_back(command.str());

   if(coinGammas_.size() != 0 && tableName_ == "coinInfo") {
      for(vector<string>::iterator it = coinGammas_.begin(); 
	  it != coinGammas_.end(); it++) {
	 stringstream coins; 
	 coins << "insert or replace into coincidences values(" 
	       << values_.at(0) << ",'" << *it << "')";
	 tempCommands.push_back(coins.str());
      }
   }
   return(tempCommands);
}//string DatabaseInput::BuildCommand


//********** CommandSizeCheck **********
void DatabaseInput::CommandSizeCheck(void)
{
   bool wrongSize = false;
   if(tableName_ == "generalInfo" && values_.size() != 6)
      wrongSize = true;
   else if(tableName_ == "coinInfo" && values_.size() != 9)
      wrongSize = true;
   else if(tableName_ == "fitInfo" && values_.size() != 10)
      wrongSize = true;
   else if(tableName_ == "coinFitInfo" && values_.size()!= 11)
      wrongSize = true;
   else
      wrongSize = false;
      
   if(wrongSize) {
      cout << endl << "There is a(are) missing value(s) in " << tableName_ 
	   << " on line number " << lineNo_ << ".  Maybe " 
	   << "you've added an extra '\"'." << endl 
	   << "Please correct this." << endl 
	   << endl;
      exit(1);
   }
}


//********** CompareModTimes **********
void DatabaseInput::CompareModTimes(void)
{
   for(map<string, time_t>::iterator itOld = oldTimes_.begin(); 
       itOld != oldTimes_.end(); itOld++) {
      map<string,time_t>::iterator itNew = newTimes_.find((*itOld).first);
      tableName_ = (*itNew).first;

      if((*itOld).second < (*itNew).second) {
	 ReadInformation();
	 UpdateModTimes((*itNew).second);
      }else {
      	 continue;
      }
   }	    
}//void DatabaseInput::CompareModTimes


//********** ExecuteBegin **********
void DatabaseInput::ExecuteSimpleCommands(const string &command)
{
   char *errorMessage = 0;
   int rc = sqlite3_exec(database, command.c_str(), 
   			 NULL, NULL, &errorMessage);
   if(rc != SQLITE_OK) {
      cout << "Error updating the database with new info from the datafiles."
   	   << endl << "ERROR:" << errorMessage << endl;
      sqlite3_free(errorMessage);
      exit(1);
   } 
}


//********** FillDatabase **********
void DatabaseInput::FillDatabase(void)
{
   vector<string> commands = BuildCommand();
   for(vector<string>::iterator it = commands.begin();
       it != commands.end(); it++) {
      char *errorMessage = 0;
      int rc = sqlite3_exec(database, (*it).c_str(), 
      			    NULL, NULL, &errorMessage);
      if(rc != SQLITE_OK) {
      	 cout << "Error updating the database with new info from the datafiles."
      	      << endl << "ERROR:" << errorMessage << endl;
      	 sqlite3_free(errorMessage);
      	 exit(1);
      }
   }
}


//********** GetComment **********
vector<string> DatabaseInput::GetComment(string &line)
{
   vector<string> temp;
   size_t foundStart = line.find("\"");
   size_t foundEnd   = line.find("\"", int(foundStart)+1);
   int size = foundEnd - foundStart;
   
   if(foundStart == string::npos || foundEnd == string::npos) {
      cout << endl << "Missing quotes in \"" << tableName_ 
	   << "\".  At line: " << lineNo_ << "." << endl
	   << "This is a fatal error." << endl << endl;
      exit(1);
   }
       
   string comment = line.substr(int(foundStart) + 1, size - 1);
   line.erase(foundStart, size + 1);
   
   temp.push_back(line);
   temp.push_back(comment);

   return(temp);
}


//********** GetNewModTime **********
time_t DatabaseInput::GetNewModTime(const string &tableName)
{
   struct stat fileAttributes;
   stringstream tempFileName;
   tempFileName << filePath << "/" << tableName << ".dat";
   int statResult = stat(tempFileName.str().c_str(), &fileAttributes);

   if(statResult == -1) {
      cout << endl << "Problem with the data file \"" << tableName
	   << "\".  It does not seem to exist." << endl << endl;
      exit(1);
   }else {
      return(fileAttributes.st_mtime);
   }
}


//********** GetOldModTimes **********
void DatabaseInput::GetOldModTimes(void)
{
   sqlite3_stmt *statement; 
   bool prepared = 
      sqlite3_prepare_v2(database, "select * from modTimes", 
 			 -1, &statement, 0) == SQLITE_OK;
   if(prepared) {
      while (true) {
	 int result = sqlite3_step(statement);
	 if(result == SQLITE_ROW) {
	    string fileName = (char*)sqlite3_column_text(statement,0);
	    int modTime = atoi((char*)sqlite3_column_text(statement,1));
	    oldTimes_.insert(make_pair(fileName, modTime));
	 }else {
	    break;
	 }
      }//while(true)
      sqlite3_finalize(statement);
   }//if(prepared)
}


//********** DatabaseInput **********
DatabaseInput::DatabaseInput(void)
{
   ExtractFilePath();

   newTimes_.insert(make_pair("coinInfo", GetNewModTime("coinInfo")));
   newTimes_.insert(make_pair("generalInfo", GetNewModTime("generalInfo")));
   newTimes_.insert(make_pair("fitInfo", GetNewModTime("fitInfo")));
   newTimes_.insert(make_pair("coinFitInfo", GetNewModTime("coinFitInfo")));

   ExecuteSimpleCommands("begin");
   GetOldModTimes();
   CompareModTimes();
   ExecuteSimpleCommands("commit");
}


//********** ~DatabaseInput **********
DatabaseInput::~DatabaseInput()
{
}


//********** ReadInformation **********
void DatabaseInput::ReadInformation(void)
{
   lineNo_ = 0;
   stringstream tempFileName;
   tempFileName << filePath << "/" << tableName_ << ".dat";
   ifstream inputFile(tempFileName.str().c_str());
   if(inputFile.is_open()) {
      while(inputFile.good()) {
	 values_.clear();
	 lineNo_++;

	 string line;
	 string temp;
	 	 
	 //Skip over the comment lines here. 
	 getline(inputFile, line);
	 if(line.find("#") != string::npos || line == "")
	    continue;
	 
	 //Get the Coincidence gammas
	 if(tableName_ == "coinInfo") {
	    vector<string> tempCoin = GetComment(line);
	    line = tempCoin.at(0);

	    stringstream tempStream;
	    tempStream << tempCoin.at(1);
	    while(tempStream >> temp)
	       coinGammas_.push_back(temp);
	 }	 
	 
	 vector<string> tempComment = GetComment(line);
	 line = tempComment.at(0);
	 string comment = tempComment.at(1);

	 vector<string> tempData;
	 stringstream lineParse;
	 lineParse << line;
	 while(lineParse >> temp)
	    tempData.push_back(temp);
	 tempData.push_back(comment);
	 values_ = tempData;
	 
	 //Do operations to update/fill the database with the
	 //collected values.  
	 CommandSizeCheck();
	 FillDatabase();

	 values_.clear();
	 coinGammas_.clear();
      } // while(coinInfoFile.good())
      inputFile.close();
   }else {
      cout << "Couldn't open \"" << tableName_ << ".dat.\"" << endl
	   << "  This is fatal!" << endl << "Exiting" << endl;
      exit(1);
   } // if(inputFile.is_open())
}//void DatabaseInput::ReadInformation


//********** UpdateModTimes **********
void DatabaseInput::UpdateModTimes(const time_t &modTime)
{
   stringstream command;
   command << "update modTimes set ModTime=" << modTime
	   << " where FileName='" << tableName_ << "'";
   
   char *errorMessage = 0;
   int rc = sqlite3_exec(database, command.str().c_str(), NULL, 0, &errorMessage);
   if(rc != SQLITE_OK) {
      cout << "Error updating the data file modification times." << endl
	   << "ERROR:" << errorMessage << endl;
      sqlite3_free(errorMessage);
      exit(1);
   }
}
