/***********************************************************
 ** \file DataReader.cpp
 ** Class to handle the read/write of the datafiles. 
 ** Written: S.V. Paulauskas - 09 Sept 2011
 **********************************************************/
#include <iostream>

#include <cstdlib>

#include "DatabaseInterface.h"
#include "DataReader.h"
#include "sqlite3.h"
#include "sys/stat.h"

using namespace std;

//********** UpdateModTimes **********
void DataReader::CompareModTimes(void)
{
   map<string, time_t> oldTimes;
   sqlite3_stmt *statement; 
   string command = "select * from modTimes";
   bool prepared = 
      sqlite3_prepare_v2(database, command.c_str(), 
 			 -1, &statement, 0) == SQLITE_OK;
   if(prepared) {
      int noColumns = sqlite3_column_count(statement);
      int result = 0;
      while (true) {
	 result = sqlite3_step(statement);
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
		   		
   for(map<string, time_t>::iterator it = oldTimes.begin(); it != oldTimes.end();
       it++) {
      map<string,time_t>::iterator newIt = modTimes_.find((*it).first);

      if((*it).second < (*newIt).second){
	 cout << "The file on disk is newer than the file in the database" 
	      << endl;
      }else {
	 cout << "The files in the database are up to date" << endl;
      }
   }	    
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

//********** UpdateModTimes **********
void DataReader::UpdateModTimes(void)
{


}
