// Search Books Directory Structure By Author
// Simple version, Searchs the hard way
// Jim Skon, Kenyon College, 2017

#include <iostream>
// Stuff for AJAX
#include "cgicc/Cgicc.h"
#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTMLClasses.h"
#include <string>
#include <iostream>
#include <algorithm> // Needed for transform
#include <fstream>
#include <dirent.h> // Needed for directory search

// Time
#include <ctime>
#include <unistd.h>
#include <sys/time.h>

#define XML_USE_STL

using namespace std;
using namespace cgicc; // Needed for AJAX functions.

void ProcessDirectory(string directory,string word);
void ProcessFile(string file,string word);
void ProcessEntity(struct dirent* entity,string word);
bool hasEnding (string const &fullString, string const &ending);
bool checkMatch(string file, string word);

string path = "/home/skon/books";
int fileCount = 0;
int matchCount = 0;
int fileMatchCount = 0;
long long wordCount = 0;
string delimiters = " ,.;:?'\"()[]";
timeval start;
timeval current;
int  max_Time;

ofstream logFile;

int main() {
  Cgicc cgi;    // Ajax object    
  string bookFile = "/home/class/SoftDev/cppXMLAJAX/books.xml";
  string directory = "";

  // Get the current time
  gettimeofday(&start, 0);
  
  
  // Create AJAX objects to recieve information from web page.
  form_iterator termIt = cgi.getElement("term");
  form_iterator timeIt = cgi.getElement("time");
  string term = **termIt;
  string stime = **timeIt;

  max_Time = atoi(stime.c_str());
  // Normalize string to lower case
  transform(term.begin(), term.end(), term.begin(), ::tolower);
  
  // Debug to file example
  //logFile.open("/tmp/skon.log",ios::out | ios::app);
  //logFile << "Got: " <<  term << ":" << endl;
  //logFile.close();  
  

  // output required message for AJAX
  cout << "Content-Type: text/plain\n\n";
  try {
    ProcessDirectory(directory,term);
  }
  catch (int e) {
    cout << "<br /> Timeout! ";
  }

  cout << "<br />The author " << term << " found in " << fileMatchCount << " books." << endl;
  cout << "<br />Total Books Checked: " << fileCount << endl;

  // Get the current time
  gettimeofday(&current, 0);
  
  int delta = (current.tv_sec-start.tv_sec);
  
  cout << "<br />Total Time: " << delta << " seconds";

return 0;
}

// Check if a string has a given ending
bool hasEnding (string const &fullString, string const &ending) {
  if (fullString.length() >= ending.length()) {
    return (0 == fullString.compare (fullString.length() - ending.\
				     length(), ending.length(), ending));
  } else {
    return false;
  }
}

// Process a given directory
// This routine in mutually recursive with process entry
void ProcessDirectory(string directory, string word)
{
  string dirToOpen = path + directory;
  DIR *dir;
  dir = opendir(dirToOpen.c_str());

  //set the new path for the content of the directory
  path = dirToOpen + "/";

  //  cout << "Process directory: " << dirToOpen.c_str() << endl;

  if(NULL == dir)
    {
      cout << "could not open directory: " << dirToOpen.c_str() << endl;
      return;
    }
  struct dirent *entity;
  entity = readdir(dir);

  while(entity != NULL)
    {
      ProcessEntity(entity,word);
      entity = readdir(dir);
    }

  //we finished with the directory so remove it from the path
  path.resize(path.length() - 1 - directory.length());
  closedir(dir);
}

// Process a given entry in a directory
void ProcessEntity(struct dirent* entity, string word)
{
  //find entity type
  if(entity->d_type == DT_DIR)
    {//it's an directory
     //don't process the  '..' and the '.' directories
      if(entity->d_name[0] == '.')
	{
	  return;
	}

      //it's an directory so process it
      ProcessDirectory(string(entity->d_name),word);
      return;
    }

  if(entity->d_type == DT_REG)
    {//regular file
      //logFile.open("/tmp/skon.log",ios::out | ios::app);
      //logFile << "File: " << path << entity->d_name << endl;
      //logFile.close();  

      ProcessFile(string(entity->d_name), word);
      return;
    }

  //there are some other types
  //read here http://linux.die.net/man/3/readdir
  cout << "Not a file or directory: " << entity->d_name << endl;
}

// Process a given file
// Only look at it if it has a ".txt" extension
// Searches the file fot the given word, counting the matches
void ProcessFile(string file, string word)
{
  string fileType = ".txt";
  if (hasEnding(file,fileType)) {
    fileCount++;
    if (word.length()>0) {
      bool matched = checkMatch(file,word);
      if (matched) {
	fileMatchCount++;

	//	cout << "<h4>Matches: " << matches << "</h4>";
      }
    }
  }
  // Get the current time
  gettimeofday(&current, 0);
  
  int delta = current.tv_sec - start.tv_sec;
  if (delta >= max_Time) {
    cout << "<h1>Time out!!!</h1>" << endl;
    throw (delta);
  }
  //if you want to do something with the file add your code here
}

// get the next "word" from line, removing it from line,
// and returning the word
string getNext(string & line) {
  string next;
  //cout << "$" << line.length();
  size_t start = line.find_first_not_of(delimiters);
  if (start != string::npos) {
    //cout << ":" << start;
    size_t end = line.find_first_of(delimiters,start);
    if (end != string::npos) {
      //cout << "#" << end;
      // word with delimiters on both sides
      next = line.substr(start,end-start);
      line.erase(0,end+1);
    } else {
      // word with delimiter only at start
      next = line.substr(start);
      line = "";
    }
  } else {
    // no delimiters found at all
    next = line;
    line = "";
  }
  return next;
}

std::istream& safeGetline(std::istream& is, std::string& t)
{
  t.clear();

  // The characters in the stream are read one-by-one using a std::streambuf.
  // That is faster than reading them one-by-one using the std::istream.
  // Code that uses streambuf this way must be guarded by a sentry object.
  // The sentry object performs various tasks,
  // such as thread synchronization and updating the stream state.

  std::istream::sentry se(is, true);
  std::streambuf* sb = is.rdbuf();

  for(;;) {
    int c = sb->sbumpc();
    switch (c) {
    case '\n':
      return is;
    case '\r':
      if(sb->sgetc() == '\n')
	sb->sbumpc();
      return is;
    case std::streambuf::traits_type::eof():
      // Also handle the case when the last line has no line ending
      if(t.empty())
	is.setstate(std::ios::eofbit);
      return is;
    default:
      t += (char)c;
    }
  }
}

// Look for metadata until you find *END*
void getMetaData(ifstream &infile, string &title, string &author, string &language){
  bool foundTitle=false, foundAuthor=false, foundLanguage=false;;
  bool foundEnd=false;
  string line;
  size_t pos, pos2;
  const int MAXLINES=20;
  
  while (!safeGetline(infile,line).eof() && !foundEnd) {

    //    if (line.length()>500) {
    //  cout << line.length() << " "; 
    //}
    pos = line.find("Title:");
    if (pos!=string::npos) {
      title = line.substr(pos+7);
      foundTitle=true;
    }
    
    pos = line.find("Author:");
    if (pos!=string::npos) {
      author = line.substr(pos);
      foundAuthor=true;
    }
    
    pos = line.find("Language:");
    if (pos!=string::npos) {
      language = line.substr(pos);
      foundLanguage=true;
    }
    
    pos = line.find("*END*");
    size_t pos2 = line.find("*** START");
    size_t pos3 = line.find("***START");
    if (pos!=string::npos || pos2!=string::npos || pos3!=string::npos) {
      foundEnd=true;}

    if (infile.tellg()>50000) foundEnd=true;
  };
  //  if (infile.eof()) {
  //  cout << "<h3>Meta Data not found</h3>" << title << ":" << author << ":" << language << "<br />";
  //}
}

// Open up file "file", First search for Title, Author, and Langauge
// and then search for author
// Return ture if match
bool  checkMatch(string file, string word) {
  ifstream infile;
  bool fileMatches = false;
  string line,w;
  string title, author, language;
  try{
    string fileWithPath = path+file;
    infile.open(fileWithPath.c_str());
    getMetaData(infile, title, author, language);
    if (infile.eof()) {
      //cout << fileWithPath << "<br />";
      // Reopen file is we searched to the end for meta data
      infile.close();
      infile.open(fileWithPath.c_str());
    }
    //    cout << "open:" << fileWithPath << endl;
    // Check if line contains null, if so it's not ACSII, so skip
    string authorNormal = author;
    transform(authorNormal.begin(), authorNormal.end(), authorNormal.begin(), ::tolower);
    
    if (authorNormal.find(word)!=string::npos) {
      //      cout << "<h4>" << title << ", " << author << ", " << language << "</h4>";
      cout << "<a class='clickable' file='" << fileWithPath << "'>" << title << ", " << author << ", " << language << "</a><br />";
      fileMatches = true;
    }

  //    logFile.close();  
  infile.close();
  } catch(ifstream::failure e){
    //cout<<e<<endl;
    return false;
  }
  return fileMatches;;
}
