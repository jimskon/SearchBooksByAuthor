// Display a file
// 
// Jim Skon, Kenyon College, 2017

#include <iostream>
// Stuff for AJAX
#include "cgicc/Cgicc.h"
#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTMLClasses.h"
#include <string>
#include <iostream>
#include <fstream>

#define XML_USE_STL

using namespace std;
using namespace cgicc; // Needed for AJAX functions.

const string path = "/home/skon/books";

int main() {
  Cgicc cgi;    // Ajax object    
  ifstream bookFile;


  // Create AJAX objects to recieve information from web page.
  form_iterator fileIt = cgi.getElement("file");
  string file = **fileIt;

  bookFile.open(file);
  cout << "Content-Type: text/plain\n\n";

  if (!bookFile.good()) {
    cout << "<h1> Error, file not found: " << file << "</h1>" << endl;
    exit(0);
  }

  string line;
  
  while (!bookFile.eof()) {
    getline(bookFile,line);
    cout << line << "<br />" << endl;
  }
}

