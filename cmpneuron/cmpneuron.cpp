// Copy a file
#include <stdlib.h>
#include <stdio.h>
#include <fstream>      // std::ifstream, std::ofstream
#include <string>
#include <string.h>
#include <algorithm>

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
  return string(cCurrentPath) + string("/");
}

string deleteExecName(string path) {
	int slashPointer = path.length() - 1;
	while(slashPointer >= 0 && path[slashPointer] != '/') {
		slashPointer--;
	}
	return path.substr(0, slashPointer + 1);
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

string getFileContent(const char* path) {
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

void writeContentToFile(string text, const char* path) {  
  ofstream outfile (path);

  // write to outfile
  outfile.write (text.c_str(), text.length());

  outfile.close();
}

void copyIntermediateFiles(char* neuronName) {
  string execDir = getExecDirectory();
  string workDir = getCurrentWorkingDirectory();
  string cmdCython_neuron = string("cp '") + execDir + string("cython_neuron.pyx' '") + workDir + string(neuronName) + string(".pyx'");
  string cmdSetup = string("cp '") + execDir + string("setup.py' '") + workDir + string("setup.py'");

  system(cmdCython_neuron.c_str());
  system(cmdSetup.c_str());
}

void deleteIntermediateFiles(char* neuronName) {
  string workDir = getCurrentWorkingDirectory();
  string cmdCython_neuron = string("rm '") + workDir + string(neuronName) + string(".pyx'");
  string cmdSetup = string("rm '") + workDir + string("setup.py'");

  system(cmdCython_neuron.c_str());
  system(cmdSetup.c_str());
}

void updateSetup(char* neuronName) {
  string setup = getFileContent("setup.py");
  string extMod = string("ext_modules = [Extension(\"cython_neuron\", [\"cython_neuron.pyx\"])]");
  string newExtMod = string("ext_modules = [Extension(\"") + string(neuronName) + string("\", [\"") + string(neuronName) + string(".pyx\"])]");
  int pos = setup.find(extMod);
  string output = setup.substr(0, pos) + newExtMod + setup.substr(pos + extMod.length());
  writeContentToFile(output, "setup.py");
}

void updatePyx(char* neuronName) {
  string pyxNeuron = string(neuronName) + string(".pyx");
  string pyNeuron = string(neuronName) + string(".py");
  string py = getFileContent(pyNeuron.c_str());
  string pyx = getFileContent(pyxNeuron.c_str());
  string anchor1 = string("<!f>zg4\"*$");
  string anchor2 = string("<h4Da10làIIg>");
  int pos1 = pyx.find(anchor1);
  int pos2 = pyx.find(anchor2);

  string output = pyx.substr(0, pos1 + anchor1.length() + 1) + py + pyx.substr(pos1 + anchor1.length() + 1, pos2 - (pos1 + anchor1.length() + 2)) + string("\n    n = ") + string(neuronName) + string("()\n") + pyx.substr(pos2 + anchor2.length() + 1);

  writeContentToFile(output, pyxNeuron.c_str());
}

void compile() {
  system("python setup.py build_ext --inplace");
}

void printHelp() {
  printf("\ncmpneuron : this tool has been created in order to add user custom python neurons to CyNEST.\nFor correct working, at least Cython 0.18 must be installed on the machine.\n\nThe syntax is :\n\tcmpneuron <filename>\nor\n\tcmpneuron <option>\n\nNote that when typing the filename, the .py must be omitted (ex: 'cmpneuron myneuron' and NOT 'cmpneuron myneuron.py')\nAlso keep in mind that in order the program to correctly run, your shell must be situated in the same directory as the .py file.\n\nThe options are:\n--help :  Prints this help\n--doc  :  Opens a pdf file containing the documentation (please read before creating any neuron!)\n\n");
}

int main (int argc, char* argv[]) {
  if(argc != 2) {
  	printf("Error: argument not valid. Please type 'cmpneuron --help' for more information\n");
	return -1;
  }
  else if(strcmp(argv[1], "--help") == 0) {
	printHelp();
  }
  else if(strcmp(argv[1], "--doc") == 0) {
	string cmd = string("xdg-open ") + getExecDirectory() + string("cmpneuron_doc.pdf");
	system(cmd.c_str());
  }
  else {
  	copyIntermediateFiles(argv[1]);
  	updateSetup(argv[1]);
  	updatePyx(argv[1]);
  	compile();
  	deleteIntermediateFiles(argv[1]);
  }
  return 0;
}



