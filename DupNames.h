// DupNames.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <iostream>
#include <conio.h>
#include <filesystem>
#include <string>
#include <vector>
//#include <windows.h>


#define MAX_TOKENS 6
#define MIN_YEAR   1875
#define MAX_YEAR   2020
#define DEFAULT_P_VAL   0
#define MAX_P_VAL       9
#define MIN_P_VAL       0
#define DEFAULT_N_VAL   1
#define MAX_N_VAL       9
#define MIN_N_VAL       1

using namespace std;

namespace fs = std::filesystem;

typedef std::filesystem::path fsPath;

struct tokenInfo_t {
	string  tokenValue;
	bool    matchFlag = false;
};

struct fileInfo_t {
	fsPath                 fileName;
	string                 trimmedFileName;
	size_t                 filePathIndex = 0;
	vector <tokenInfo_t>   tokens;
	int                    fileNameYear  = 0;
};

struct pathInfo_t {
	fsPath   pathName;
	size_t   pathNameIndex       = 0;
	size_t   parentPathNameIndex = 0;
	bool     protectedFlag       = false;
	bool     pathScanned         = false;
};

#ifdef DUP_NAMES_CPP
std::vector <struct fileInfo_t> FileStorage;
std::vector <struct pathInfo_t> PathStorage;

void commandLineError(string errorInfo);
void compareFileEntries();
void readDirBranch(const fsPath pathToShow, size_t currentPathIndex, bool currentProtFlag);
void removeSpaces(string& str);

bool tokenPositiveFlag = false;
bool allTokenMatchEnable = false;
bool countedMatchEnable = false;
bool trimmedFileNameMatchEnable = false;
bool episodeMatchEnable = false;
bool movieMatchEnable = false;

int  tokenNMatchValue = 1;
int  tokenPMatchValue = 0;
#else
extern std::vector <struct fileInfo_t> FileStorage;
extern std::vector <struct pathInfo_t> PathStorage;

extern bool tokenPositiveFlag;
extern bool allTokenMatchEnable;
extern bool countedMatchEnable;
extern bool trimmedFileNameMatchEnable;
extern bool episodeMatchEnable;
extern bool movieMatchEnable;

extern int  tokenNMatchValue;
extern int  tokenPMatchValue;
#endif

// TODO: Reference additional headers your program requires here.
