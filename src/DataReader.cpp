/***********************************************************
 ** \file DataReader.cpp
 ** Class to handle the read/write of the datafiles. 
 ** Written: S.V. Paulauskas - 09 Sept 2011
 **********************************************************/
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

#include <cstdlib>

#include "DatabaseInterface.h"
#include "DataReader.h"
#include "sqlite3.h"
#include "sys/stat.h"

using namespace std;

//********** BuildCommand **********
string DataReader::BuildCommand(void)
{
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
   return(command.str());
}//string DataReader::BuildCommand


//********** CommandSizeCheck **********
void DataReader::CommandSizeCheck(void)
{
   bool wrongSize = false;
   if(tableName_ == "generalInfo" && values_.size() != 5)
      wrongSize = true;
   else if(tableName_ == "coinInfo" && values_.size() != 9)
      wrongSize = true;
   else if(tableName_ == "fitInfo" && values_.size() != 10)
      wrongSize = true;
   else
      wrongSize = false;
      
   if(wrongSize){
      cout << endl << "There is a missing value in " << tableName_ 
	   << " on line" << lineNo_ << ".  Please correct this." << endl;
      exit(1);
   }
}


//********** CompareModTimes **********
void DataReader::CompareModTimes(void)
{
   for(map<string, time_t>::iterator itOld = oldTimes_.begin(); 
       itOld != oldTimes_.end(); itOld++) {
      map<string,time_t>::iterator itNew = newTimes_.find((*itOld).first);
      tableName_ = (*itNew).first;
      
      if((*itOld).second <= (*itNew).second) { //remove the = after tests
	 ReadInformation();
	 UpdateModTimes((*itNew).second);
      }else {
      	 continue;
      }
   }	    
}//void DataReader::CompareModTimes


//********** FillDatabase **********
void DataReader::FillDatabase(void)
{
   string command = BuildCommand();
   char *errorMessage = 0;
   int rc = sqlite3_exec(database, command.c_str(), 
   			 NULL, 0, &errorMessage);
   if(rc != SQLITE_OK) {
      cout << "Error updating the database with new info from the datafiles."
   	   << endl << "ERROR:" << errorMessage << endl;
      sqlite3_free(errorMessage);
      exit(1);
   }
}


//********** GetComment **********
vector<string> DataReader::GetComment(string &line)
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
time_t DataReader::GetNewModTime(const string &tableName_)
{
   struct stat fileAttributes;
   stringstream tempFileName;
   tempFileName << "data/" << tableName_ << ".dat";
   int statResult = stat(tempFileName.str().c_str(), &fileAttributes);

   if(statResult == -1) {
      cout << endl << "Problem with the data file \"" << tableName_ 
	   << "\".  It does not seem to exist." << endl << endl;
      exit(1);
   }else {
      return(fileAttributes.st_mtime);
   }
}


//********** GetOldModTimes **********
void DataReader::GetOldModTimes(void)
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


//********** DataReader **********
DataReader::DataReader(void)
{
   newTimes_.insert(make_pair("coinInfo", GetNewModTime("coinInfo")));
   newTimes_.insert(make_pair("generalInfo", GetNewModTime("generalInfo")));
   newTimes_.insert(make_pair("fitInfo", GetNewModTime("fitInfo")));

   GetOldModTimes();
   CompareModTimes();
}


//********** ~DataReader **********
DataReader::~DataReader()
{
}


//********** ReadInformation **********
void DataReader::ReadInformation(void)
{
   
   lineNo_ = 0;
   stringstream tempFileName;
   tempFileName << "data/" << tableName_ << ".dat";
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
	    sort(coinGammas_.begin(), coinGammas_.end());
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
      } // while(coinInfoFile.good())
   }else {
      cout << "Couldn't open \"" << tableName_ << ".dat.\"" << endl
	   << "  This is fatal!" << endl << "Exiting" << endl;
      exit(1);
   } // if(inputFile.is_open())
}//void DataReader::ReadInformation


//********** UpdateModTimes **********
void DataReader::UpdateModTimes(const time_t &modTime)
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
