/***********************************************************
 ** \file database.cpp
 ** File to handle communicate with sqlite databases
 ** Written: S.V. Paulauskas - 30 Aug 2011
 **********************************************************/
#include <iostream>
#include <vector>

#include <cstdlib>

#include "DatabaseInterface.h"
#include "OutputInterface.h"
#include "sqlite3.h"

using std::cout;
using std::endl;
using std::string;
using std::vector;

int main(int argc, char* argv[])
{
   DatabaseInterface testNumArgs(argc);
   string databaseName = argv[1];
   vector<string> gammas;
   
   if(argc == 3){
      if(atoi(argv[2]) == 0) {
   	 string flag = argv[2];
   	 if(flag == "-e") {
	    //InputInterface  inputHandling(argv[1]);
	    //DatabaseEditor editing(argv[1]);
	    cout << "Database Editor goes here" << endl;
	    exit(1);
	 } else if(flag == "-n") {
	    //DatabaseInterface newDatabase();
	 }else{	    
	    DatabaseInterface errorMessage(0);
	 } 
      }else if (atoi(argv[2]) != 0) {
	 OutputInterface outputHandling(argv[1]);
	 gammas.push_back(argv[2]);
   	 outputHandling.OutputInformation(gammas, "generalInfo");
	 exit(1);
      }else {
	 DatabaseInterface errorMessage(0);
      }
   }else if(argc == 4) {
      if(atoi(argv[2]) != 0 &&  atoi(argv[3]) != 0) {
	 OutputInterface outputHandling(argv[1]);
	 gammas.push_back(argv[2]);
	 gammas.push_back(argv[3]);
	 outputHandling.OutputInformation(gammas, "range");
	 exit(1);
      }else if (atoi(argv[2]) == 0 &&  atoi(argv[3]) != 0) {
	 OutputInterface outputHandling(argv[1]);
	 gammas.push_back(argv[3]);
	 outputHandling.VerbosityHandler(gammas, argv[2]);
      }else {
	 DatabaseInterface errorMessage(0);
      }	 
   }else if(argc == 5) {
      if(atoi(argv[2]) == 0 && atoi(argv[3]) != 0 
	 && atoi(argv[4]) != 0) {
	 OutputInterface outputHandling(argv[1]);
	 gammas.push_back(argv[3]);
	 gammas.push_back(argv[4]);
	 outputHandling.VerbosityHandler(gammas, "generalInfo");
      }else {
	 DatabaseInterface errorMessage(0);
      }
   } else {
      DatabaseInterface errorMessage(0);
   }
}
