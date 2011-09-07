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

//********** CommandSizeCheck **********
void DataReader::CommandSizeCheck(const string &fileName, const vector<string> &values,
				  const int &lineNo)
{
   bool wrongSize = false;
   if(fileName == "generalInfo.dat" && values.size() != 5)
      wrongSize = true;
   else if(fileName == "coinInfo.dat" && values.size() != 9)
      wrongSize = true;
   else if(fileName == "fitInfo.dat" && values.size() != 10)
      wrongSize = true;
   else
      wrongSize = false;
      
   if(wrongSize){
      cout << endl << "There is a missing value in " << fileName 
	   << " on line" << lineNo << ".  Please correct this." << endl;
      exit(1);
   }
}


//********** CompareModTimes **********
void DataReader::CompareModTimes(void)
{
   map<string, time_t> oldTimes;
   sqlite3_stmt *statement; 
   string command = "select * from modTimes";
   bool prepared = 
      sqlite3_prepare_v2(database, command.c_str(), 
 			 -1, &statement, 0) == SQLITE_OK;
   if(prepared) {
      while (true) {
	 int result = sqlite3_step(statement);
	 if(result == SQLITE_ROW) {
	    string fileName = (char*)sqlite3_column_text(statement,0);
	    int modTime = atoi((char*)sqlite3_column_text(statement,1));
	    oldTimes.insert(make_pair(fileName, modTime));
	 }else {
	    break;
	 }
      }//while(true)
      sqlite3_finalize(statement);
   }//if(prepared)
   
   for(map<string, time_t>::iterator itOld = oldTimes.begin(); itOld != oldTimes.end();
       itOld++) {
      map<string,time_t>::iterator itNew = modTimes_.find((*itOld).first);
      
      if((*itOld).second < (*itNew).second){
	 //ReadInformation((*itOld).second);
	 UpdateModTimes((*itNew).first, (*itNew).second);
      }else {
	 continue;
      }
   }	    
}//void DataReader::CompreModTimes


//********** GetComment **********
vector<string> DataReader::GetComment(string &line, const int &lineNo, const string &fileName)
{
   vector<string> temp;
   size_t foundComStart = line.find("\"");
   size_t foundComEnd   = line.find("\"", int(foundComStart)+1);
   int commentSize = foundComEnd - foundComStart;
   
   if(foundComStart == string::npos || foundComEnd == string::npos) {
      cout << endl << "Cannot find comment/coincidence quotes in \"" 
	   << fileName << "\".  At line: " << lineNo << "." << endl
	   << "This is a fatal error." << endl << endl;
      exit(1);
   }
       
   string comment = line.substr(int(foundComStart) + 1, commentSize - 1);
   line.erase(foundComStart, commentSize + 1);
   
   temp.push_back(line);
   temp.push_back(comment);

   return(temp);
}


//********** GetModTime **********
time_t DataReader::GetModTime(const string &fileName)
{
   struct stat fileAttributes;
   int statResult = stat(fileName.c_str(), &fileAttributes);

   if(statResult == -1) {
      cout << endl << "Problem with the data file \"" << fileName 
	   << "\".  It does not seem to exist." << endl << endl;
      exit(1);
   }else {
      return(fileAttributes.st_mtime);
   }
}


//********** DataReader **********
DataReader::DataReader()
{
   modTimes_.insert(make_pair("coinInfo.dat", GetModTime("coinInfo.dat")));
   modTimes_.insert(make_pair("generalInfo.dat", 
			      GetModTime("generalInfo.dat")));
   modTimes_.insert(make_pair("fitInfo.dat", GetModTime("fitInfo.dat")));

   CompareModTimes();
}


//********** ~DataReader **********
DataReader::~DataReader()
{
}


//********** ReadInformation **********
void DataReader::ReadInformation(const string &fileName)
{
   int lineNo = 0;
   ifstream inputFile(fileName.c_str());
   if(inputFile.is_open()) {
      while(inputFile.good()) {
	 lineNo++;
	 string line;
	 string temp;
	 vector<string> coinGammas;
	 
	 //Skip over the comment lines here. 
	 getline(inputFile, line);
	 if(line.find("#") != string::npos || line == "")
	    continue;
	 
	 if(fileName == "coinInfo.dat") {
	    //Get the Coincidence gammas
	    vector<string> tempCoin = GetComment(line, lineNo, fileName);
	    line = tempCoin.at(0);
	    
	    stringstream tempStream (stringstream::in | stringstream::out);
	    tempStream << tempCoin.at(1);
	    while(tempStream >> temp)
	       coinGammas.push_back(temp);
	    sort(coinGammas.begin(), coinGammas.end());
	 }	 
	 
	 vector<string> tempComment = GetComment(line, lineNo, fileName);
	 line = tempComment.at(0);
	 string comment = tempComment.at(1);

	 vector<string> tempData;
	 stringstream lineParse (stringstream::in | stringstream::out);
	 lineParse << line;
	 while(lineParse >> temp)
	    tempData.push_back(temp);
	 tempData.push_back(comment);
	 CommandSizeCheck(fileName, tempData, lineNo);
      } // while(coinInfoFile.good())
   }else {
      cout << "Couldn't open \"" << fileName << ".\"" << endl
	   << "  This is fatal!" << endl << "Exiting" << endl;
      exit(1);
   } // if(inputFile.is_open())
}


//********** UpdateModTimes **********
void DataReader::UpdateModTimes(const string &fileName, const time_t &modTime)
{
   stringstream ss;
   ss << modTime;
   string command = "update modTimes";
   command.append(" set ModTime=");
   command.append(ss.str());
   command.append(" where FileName='");
   command.append(fileName);
   command.append("'");
   
   char *errorMessage = 0;
   int rc = sqlite3_exec(database, command.c_str(), NULL, 0, &errorMessage);
   if(rc != SQLITE_OK) {
      cout << "Error updating the data file modification times." << endl
	   << "ERROR:" << errorMessage << endl;
      sqlite3_free(errorMessage);
      exit(1);
   }
}
