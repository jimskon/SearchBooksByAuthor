#include <iostream>
#include <fstream>
#include <string>
#include <dirent.h>
#include <algorithm>

using namespace std;

void ProcessDirectory(string directory,string word);
void ProcessFile(string file,string word);
void ProcessEntity(struct dirent* entity,string word);
bool hasEnding (string const &fullString, string const &ending);
int stringMatchCount(string file, string word);
bool getMetaData(string &title, string &Author, string &language);

string path = "/home/skon/books/";
int fileCount = 0;
int matchCount = 0;
int fileMatchCount = 0;
long long wordCount = 0;
string delimiters = " ,.;:?'\"()[]";

int main()
{
  string word;
  string directory = "";
  cout << "Word to search for: ";
  cin >> word;
  // Convert to lower case
  transform(word.begin(), word.end(), word.begin(), ::tolower);
  ProcessDirectory(directory,word);
  cout << "The word \"" << word << "\" found " << matchCount << " times in " << fileMatchCount << " books and " << wordCount << " words" << endl; 
  cout << "Total Books:" << fileCount << endl;
  return 0;
}

bool getMetaData(string &title, string &Author, string &language) {
  bool success = true;

  return success;
}

bool hasEnding (string const &fullString, string const &ending) {
  if (fullString.length() >= ending.length()) {
    return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
  } else {
    return false;
  }
}

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

void ProcessEntity(struct dirent* entity, string word)
{
  //find entity type
  if(entity->d_type == DT_DIR)
    {//it's an direcotry
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
      ProcessFile(string(entity->d_name), word);
      return;
    }

  //there are some other types
  //read here http://linux.die.net/man/3/readdir
  cout << "Not a file or directory: " << entity->d_name << endl;
}

void ProcessFile(string file, string word)
{
  string fileType = ".txt";
  if (hasEnding(file,fileType)) {
      fileCount++;
      if (word.length()>0) {
	long long saveWC = wordCount;
	int matches = stringMatchCount(file,word);
	if (matches > 0) {
	  fileMatchCount++;
	  matchCount += matches;
	  cout << "   " << path << file;
	  cout << " words: " << wordCount - saveWC;
	  cout << " matches: " << matches << endl;;
	}
      }
  }
  //if you want to do something with the file add your code here
}

string getNext(string & line) {
  string next;

  size_t start = line.find_first_not_of(delimiters);
  if (start != string::npos) {

    size_t end = line.find_first_of(delimiters,start);
    if (end != string::npos) {

      // word with delimiters on both sides
      next = line.substr(start,end-start);
      line.erase(0,end+1);
    } else {
      // word with delimiter only at start
      next = line.substr(start);
      line = "";
    }
  } else {
    // not delimiters found at all
    next = line;
    line = "";
  }

  return next;
}

int stringMatchCount(string file, string word) {
  ifstream infile;
  int count = 0;
  string line,w;

  try{
    string fileWithPath = path+file;
    infile.open(fileWithPath.c_str());
    //    cout << "open:" << fileWithPath << endl;
    getline(infile,line);
    int lines = 0; // Used to limit number of lines per file
    int words; // used to limit number of words per line    
    while(!infile.eof() && lines++ < 10000){
      // normalize to lower case
      words = 0;
      while (line.length()>0 && words++ < 100) {
	w = getNext(line);
	transform(w.begin(), w.end(), w.begin(), ::tolower);
	// Check for non alpha character, and only process if none found
	// this serves to filter out non-english strings
	// also throw out words less than 3 characters
	size_t found = w.find_first_not_of("abcdefghijklmnopqrstuvwxyz ");
	if (w.length() > 2 && found==string::npos) { 
	  wordCount++;
	  if(word.compare(w) == 0) ++count;
	}
      }
    getline(infile,line);

    }

    infile.close();
  }catch(ifstream::failure e){

  }
  return count;
}
