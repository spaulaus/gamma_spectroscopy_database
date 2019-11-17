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
#include <sqlite3.h>
#include <sys/stat.h>

#include "DatabaseInterface.hpp"
#include "DatabaseInput.hpp"

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


//********** FillDatabase **********
void DatabaseInput::FillDatabase(void)
{
   vector<string> commandList;
   commandList.push_back("create table coinInfo(Gamma integer primary key "
			 "asc not null, Spectrum integer, GammaLow integer, "
			 "GammaHigh integer, BkgLowA integer, "
			 "BkgHighA integer,BkgLowB integer, "
			 "BkgHighB integer, Comments varchar)");
   commandList.push_back("CREATE TABLE coinFitInfo(Gamma integer asc "
    			 "not null, CoinGamma integer not null, "
    			 "Spectrum integer, FitLow integer, "
    			 "FitHigh integer, Centroid real, Area integer, "
    			 "PercentErr real, FWHM real, LowHighMethod integer, "
    			 "Comments varchar, primary key (Gamma, CoinGamma))");
   commandList.push_back("create table coincidences(Gamma integer asc "
			 "not null, CoinGamma integer not null, "
			 "primary key (Gamma, CoinGamma))");
   commandList.push_back("CREATE TABLE generalInfo(Gamma integer "
			 "primary key asc not null, Spectrum integer, "
			 "Nucleus varchar, HalfLife real, PercentErr real,"
			 "Comments varchar)");
   commandList.push_back("CREATE TABLE fitInfo(Gamma integer "
			 "asc not null, Spectrum integer, FitLow integer, "
			 "FitHigh integer, Centroid real, Area integer, "
			 "PercentErr real, FWHM real, LowHighMethod integer, "
			 "Comments varchar, primary key (Gamma, Spectrum))");
   commandList.push_back("CREATE TABLE eff(name real primary key asc "
			 "not null, value real, error real)");
   commandList.push_back("CREATE TABLE modTimes(FileName varchar primary key "
			 "asc not null, ModTime date not null)");
   commandList.push_back("insert into modTimes values('generalInfo', 0)");
   commandList.push_back("insert into modTimes values('coinInfo', 0)");
   commandList.push_back("insert into modTimes values('fitInfo',0)");
   commandList.push_back("insert into modTimes values('coinFitInfo',0)");
   commandList.push_back("insert into modTimes values('eff',0)");

   cout << "Begin creating database: " << databaseName_ << endl;
   interface.ExecuteCommand(commandList);
   cout << "Successfully created database: " << databaseName_ << endl;
}//void DatabaseInterface::CreateDatabase


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
   else if(tableName_ == "eff" && values_.size() != 3)
      wrongSize = true;

   if(wrongSize) {
      cout << endl << "There is a(are) missing value(s) in " << tableName_
	   << " on line number " << lineNo_ << ".  Maybe "
	   << "you've added an extra '\"'." << endl
	   << "Please correct this." << endl
	   << endl;
      exit(1);
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
void DatabaseInput::StatDataFiles(void)
{
   const char* arr[] =
      {"coinInfo", "generalInfo","fitInfo", "eff"};
   vector<string> fileNames (arr, arr +sizeof(arr) / sizeof(arr[0]));
   for(unsigned int i = 0; i < fileNames.size(); i++) {
      struct stat fileAttributes;
      stringstream tempFileName;
      tempFileName << filePath_ << "/" << fileNames[i] << ".dat";
      int statResult = stat(tempFileName.str().c_str(), &fileAttributes);

      if(statResult == -1) {
	 cout << endl << "Problem with the data file \"" << fileNames[i]
	      << "\".  It does not seem to exist." << endl << endl;
	 exit(1);
      }else {
	 newTimes_.insert(make_pair(fileNames[i], fileAttributes.st_mtime));
      }
   }
}


//********** GetOldModTimes **********
void DatabaseInput::CompareModTimes(void)
{
   vector<string> command (1, "select * from modTimes");
   interface.QueryDatabase(command);
   vector<pair<string,string> > *oldTimes = interface.GetRequestedData();

   for(unsigned int i = 0; i < oldTimes->size(); i+=2) {
      map<string,time_t>::iterator itNew =
	 newTimes_.find(oldTimes->at(i).second);
      tableName_ = oldTimes->at(i).second;
      if(atoi(oldTimes->at(i+1).second.c_str()) < (*itNew).second) {
       	 ReadDataFiles();
	 UpdateModTimes((*itNew).second);
      }else {
       	 continue;
      }
   }
}


//********** DatabaseInput **********
DatabaseInput::DatabaseInput()
{
   filePath_ = interface.GetFilePath();

   StatDataFiles();
   vector<string> temp0 (1,"begin");
   interface.ExecuteCommand(temp0);
   CompareModTimes();
   vector<string> temp1 (1, "commit");
   interface.ExecuteCommand(temp1);
}


//********** ReadDataFiles **********
void DatabaseInput::ReadDataFiles(void)
{
   lineNo_ = 0;
   stringstream tempFileName;
   tempFileName << filePath_ << "/" << tableName_ << ".dat";
   ifstream inputFile(tempFileName.str().c_str());
   if(inputFile.is_open()) {
      while(inputFile.good()) {
	 values_.clear();
	 coinGammas_.clear();
	 lineNo_++;

	 string line, temp;

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

	 string comment = "";
	 if(tableName_ != "eff") {
	    vector<string> tempComment = GetComment(line);
	    line = tempComment.at(0);
	    comment = tempComment.at(1);
	 }

	 vector<string> tempData;
	 stringstream lineParse;
	 lineParse << line;
	 while(lineParse >> temp)
	    tempData.push_back(temp);
	 if(tableName_ != "eff")
	    tempData.push_back(comment);
	 values_ = tempData;

	 //Do operations to update/fill the database with the
	 //collected values.
	 CommandSizeCheck();
	 vector<string> commands = BuildCommand();
	 interface.ExecuteCommand(commands);
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
   vector<string> temp (1,command.str());
   interface.ExecuteCommand(temp);
}
