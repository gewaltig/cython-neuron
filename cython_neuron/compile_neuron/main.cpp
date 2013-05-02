// Copy a file
#include <stdlib.h>
#include <stdio.h>
#include <fstream>      // std::ifstream, std::ofstream
#include <string>
#include <algorithm>
#include <regex>
#include <iterator>

#ifdef WINDOWS
    #include <direct.h>
    #define GetCurrentDir _getcwd
#else
    #include <unistd.h>
    #define GetCurrentDir getcwd
 #endif

using namespace std;

string getCurrentWorkingDirectory() {
  char cCurrentPath[FILENAME_MAX];

  if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath)))
  {
     return NULL;
  }

  cCurrentPath[sizeof(cCurrentPath) - 1] = '\0'; /* not really required */
  return string(cCurrentPath);
}

string deleteExecName(string path) {
	std::regex e ("\\/.+?$");
	return regex_replace (path,e,"");
}

string getExecDirectory() {
  char pBuf[FILENAME_MAX];
#ifdef WINDOWS
	int bytes = GetModuleFileName(NULL, pBuf, FILENAME_MAX);
	if(bytes == 0)
		return NULL;
	else
		return deleteExecName(string(pBuf));

#else
	char szTmp[32];
	sprintf(szTmp, "/proc/%d/exe", getpid());
	int bytes = min((int)readlink(szTmp, pBuf, FILENAME_MAX), FILENAME_MAX - 1);
	if(bytes >= 0)
		pBuf[bytes] = '\0';
	return deleteExecName(string(pBuf));
#endif
}

string getFileContent(char* path) {
  ifstream infile (path);

  // get size of file
  infile.seekg (0,infile.end);
  long size = infile.tellg();
  infile.seekg (0);

  // allocate memory for file content
  char* buffer = new char[size];

  // read content of infile
  infile.read (buffer,size);

  string output(buffer);

  // release dynamically-allocated memory
  delete[] buffer;

  infile.close();

  return output;
}

void writeContentToFile(string text, char* path) {  
  ofstream outfile (path);

  // write to outfile
  outfile.write (text.c_str(), text.length());

  outfile.close();
}

int main (int argc, const char* argv[]) {
  printf("%s\n", getCurrentWorkingDirectory().c_str());
  return 0;
}
