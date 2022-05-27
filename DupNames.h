// DupNames.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <iostream>
#include <conio.h>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>


#define MAX_TOKENS      6
#define MIN_YEAR     1875
#define MAX_YEAR     2020
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
	bool                   fileDeleted   = false;
};

struct pathInfo_t {
	fsPath   pathName;
	size_t   pathNameIndex       = 0;
	size_t   parentPathNameIndex = 0;
	bool     protectedFlag       = false;
	bool     pathScanned         = false;
};

#ifdef DUP_NAMES_CPP
#define EXTERN          // conditional extern for the file named DupNames.cpp
#else
#define EXTERN extern
#endif

EXTERN void commandLineError(string errorInfo);
EXTERN void compareFileEntries();
EXTERN void readDirBranch(const fsPath pathToShow, size_t currentPathIndex, bool currentProtFlag);
EXTERN void removeSpaces(string& str);
EXTERN void textOut(ostringstream& buf);

EXTERN std::vector <struct fileInfo_t> FileStorage;
EXTERN std::vector <struct pathInfo_t> PathStorage;

EXTERN ofstream logFileStream;
EXTERN ostringstream textOutBuf;

// global data with initializations
#ifdef DUP_NAMES_CPP
bool allowFileDelete = false;
bool allowSkip2ProtFiles = false;
bool logFileAppend = false;
bool logFileCreate = false;
bool tokenPositiveFlag = false;
bool allTokenMatchEnable = false;
bool countedMatchEnable = false;
bool trimmedFileNameMatchEnable = false;
bool episodeMatchEnable = false;
bool movieMatchEnable = false;

int  tokenNMatchValue = 1;
int  tokenPMatchValue = 0;
#else
extern bool allowFileDelete;
extern bool allowSkip2ProtFiles;
extern bool logFileAppend;
extern bool logFileCreate;
extern bool tokenPositiveFlag;
extern bool allTokenMatchEnable;
extern bool countedMatchEnable;
extern bool trimmedFileNameMatchEnable;
extern bool episodeMatchEnable;
extern bool movieMatchEnable;

extern int  tokenNMatchValue;
extern int  tokenPMatchValue;
#endif
