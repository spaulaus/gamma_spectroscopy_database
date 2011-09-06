/*************************************************
 ** DatabaseInterface.cpp
 ** Class to interface with sqlite3.
 ** Written: S.V. Paulauskas - 30 Aug 2011
 ************************************************/
#include <iostream>
#include <vector>

#include <cstdlib>

#include "DatabaseInterface.h"

using std::cout;
using std::endl;
using std::string;
using std::vector;

sqlite3 *DatabaseInterface::database;
string DatabaseInterface::databaseName;

//********** CreateDatabase **********
void DatabaseInterface::CreateDatabase(const string &name)
{
   int returnCode = sqlite3_open(name.c_str(), &database);

   if(returnCode) {
      cout << "Cannot open database named " << databaseName << endl
	   << "Sqlite3 error message: " << sqlite3_errmsg(database) << endl;
      sqlite3_close(database);
      exit(1);
   }else {
      vector<string> commandList;
      commandList.push_back("create table coinInfo(Gamma integer primary key "
			    "asc not null, Spectrum integer, GammaLow integer, "
			    "GammaHigh integer, BkgLowA integer, "
			    "BkgHighA integer,BkgLowB integer, "
			    "BkgHighB integer, Comments varchar)");
      commandList.push_back("CREATE TABLE coincidences(gamma integer asc "
			    "not null, coinGamma integer not null)");
      commandList.push_back("CREATE TABLE generalInfo(Gamma integer "
			    "primary key asc not null, Spectrum integer, "
			    "Nuclide varchar, HalfLife real, Comments varchar)");
      commandList.push_back("CREATE TABLE fitInfo(Gamma integer primary key "
			    "asc not null, Spectrum integer, FitLow integer, "
			    "FitHigh integer, Centroid real, Area integer, "
			    "PercentErr real, FWHM real, LowHighMethod integer, "
			    "Comments varchar)");
      commandList.push_back("CREATE TABLE modTimes(FileName varchar primary key "
			    "asc not null, ModTime date not null)");
      commandList.push_back("insert into modTimes values('generalInfo.dat', 0)");
      commandList.push_back("insert into modTimes values('coinInfo.dat', 0)");
      commandList.push_back("insert into modTimes values('fitInfo.dat',0)");

      char *zErrMsg = 0;
      for(vector<string>::iterator it = commandList.begin(); 
	  it != commandList.end(); it++){
	 int rc = sqlite3_exec(database, (*it).c_str(), NULL, 0, &zErrMsg);
	 if(rc != SQLITE_OK){
	    cout << "Error creating the tables in the new database." << endl
		 << "ERROR:" << zErrMsg << endl;
	    sqlite3_free(zErrMsg);
	    exit(1);
	 }
      }
   }// if(returnCode)
}


//********** DatabaseInterface **********
DatabaseInterface::DatabaseInterface()
{
}


//********** DatabaseInterface **********
DatabaseInterface::DatabaseInterface(const int &noArgs)
{
   if(noArgs < 3) {
      OutputHelpInfo();
      exit(1);
   }
}


//********** ~DatabaseInterface() **********
DatabaseInterface::~DatabaseInterface()
{
   sqlite3_close(database);
}


//********** OpenDatabase **********
void DatabaseInterface::OpenDatabase(const string &dbName)
{
   databaseName = dbName;
   int returnCode = sqlite3_open_v2(databaseName.c_str(), &database, 
				    SQLITE_OPEN_READWRITE,"unix");
   if(returnCode) {
      cout << "Cannot open database named " << databaseName << endl
	   << "Sqlite3 error message: " << sqlite3_errmsg(database) << endl;
      sqlite3_close(database);
      exit(1);
   }else {
      cout << "Successfully opened database: " << databaseName << endl;
   }
}


//********** OutputHelpInfo **********
void DatabaseInterface::OutputHelpInfo(void)
{
   cout << endl << "A program to help organize gammas." << endl
	<< "Example: ./gammaSearch <database name> <flag> <gammaEnergy>" << endl
	<< "Recognized flags (place before the energy): " << endl
	<< "-h -> displays this message" << endl
	<< "-g -> outputs general information (default)" << endl
	<< "-c -> outputs general + coincidence information" << endl
	<< "-f -> outputs general + fit information" << endl
	<< "-v -> outputs all information" << endl 
	<< "-e -> Enters edit mode for editing the database." << endl
	<< "To search in an energy range simply enter the " 
	<< "low and high bound." << endl
	<< "General output for a range can be generated by passing" 
	<< " the \"-g\" flag." << endl
	<< "Problems? Contact stanpaulauskas@gmail.com" << endl << endl;
}
