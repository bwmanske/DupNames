// DupNames.cpp : Defines the entry point for the application.
//
#define DUP_NAMES_CPP

//#include <iostream>
#include <time.h>

#include <windows.h>

#include "IniFile.h"
#include "DupNames.h"
#include "DupCompare.h"

#pragma warning(disable : 26451)
#pragma warning(disable : 4267)

bool             exitRequest           = false;
bool             readPathsDone         = false;
bool             InitSuccess           = true;
bool             INI_NormalComplete    = false;
bool             INI_ProtectedComplete = false;
bool             verboseOutputFlag     = false;

// debug
bool   afterBreak = false;

int    INI_PathProtectedCount = 0;
int    INI_PathNormalCount = 0;
int    INI_TokenMatchCount = 0;
int    workingPathIndex = 0;
int    lastFileTotal = 0;
int    lastPathTotal = 0;

int       rotationIndex = 1;
const int matchRotation[] = { 0, 4, 2, 5, 6, 7, 3, 1 };
int       rotationBits;

// default Filename for the INI status file
const string     default_INI_FileName = "DupNames.ini";
const string     SectionName_InitState = "InitState";
const string     SectionName_PathList = "PathList";

string           Key_ProtectedPath = "ProtectedPath";
string           Key_NormalPath = "NormalPath";
string           Key_TokenMatchCount = "TokenMatchCount";
string           workingPath;
string           tokenMatchCountStr;

string           INI_FileName = "";

int main(int argc, char** argv)
{
	cout << "Command Line and " << argc - 1 << " arguments:" << endl;
	cout << "   " << argv[0] << endl;
	if (argc > 1) {
		int i = 1;
		while (argv[i] != NULL) {
			cout << "   " << i << ", " << argv[i] << endl;
			if (argv[i][0] == '-' && argv[i][2] != ' ') {
				argv[i][1] = tolower(argv[i][1]);
				switch (argv[i][1]) {
				case 'h':
					commandLineError("Command Line Help -");
					return 2;
				case 'i':
					INI_FileName = (string)&argv[i][2];
					cout << "INI file name set to " << INI_FileName << endl;
					break;
				case 'v':
					verboseOutputFlag = true;
					cout << "Verbose output enabled" << endl;
					break;
				default:
					commandLineError("invalid command line argument - " + (string)argv[i]);
					return 1;
				}
			}
			else {
				commandLineError("no dash found in command line - " + (string)argv[i]);
				return 1;
			}
			i++;
		}
	}
	else {
		INI_FileName = default_INI_FileName;
	}

	cout << "Press Q to quit and H for help" << endl;

	if (CIniFile::DoesFileExist(INI_FileName)) {
		std::cout << endl << INI_FileName << " - File exists" << endl << endl;      // time to read the file

		if (CIniFile::DoesSectionExist(SectionName_InitState, INI_FileName)) {
			tokenMatchCountStr = CIniFile::GetValue(Key_TokenMatchCount, SectionName_InitState, INI_FileName);
		}
		else {
			cout << SectionName_InitState << " - Section does NOT exist" << endl;
			InitSuccess = false;
		}

		if (CIniFile::DoesSectionExist(SectionName_PathList, INI_FileName)) {
			string RetValue = "";
			string INI_Path;

			std::cout << SectionName_InitState << " - Section exists" << endl;
			INI_Path = Key_ProtectedPath + to_string(INI_PathProtectedCount + 1);

			// read the input path name from the INI file
			workingPath = CIniFile::GetValue(INI_Path, SectionName_PathList, INI_FileName);
			if (CIniFile::DoesPathExist(workingPath)) {
				INI_PathProtectedCount++;
				cout << INI_Path << " successful read - " << workingPath << endl;
			}
			else {
				INI_ProtectedComplete = true;
				INI_Path = Key_NormalPath + to_string(INI_PathNormalCount + 1);
				workingPath = CIniFile::GetValue(INI_Path, SectionName_PathList, INI_FileName);
				if (CIniFile::DoesPathExist(workingPath)) {
					INI_PathNormalCount++;
					cout << INI_Path << " successful read - " << workingPath << endl;
				}
				else {
					INI_NormalComplete = true;
					cout << "Input Path Does Not Exist" << endl;
					InitSuccess = false;
				}
			}
			if (InitSuccess) {
				struct pathInfo_t tempPath;
				tempPath.pathName = workingPath;
				tempPath.protectedFlag = !INI_ProtectedComplete;
				tempPath.pathScanned = false;
				tempPath.parentPathNameIndex = 0;
				tempPath.pathNameIndex = PathStorage.size() + 1;

				PathStorage.push_back(tempPath);
				workingPathIndex = 1;
			}
		}
		else {
			cout << SectionName_PathList << " - Section does NOT exist" << endl;
			InitSuccess = false;
		}
	}
	else {
		cout << INI_FileName << " - INI File does NOT exist" << endl;
		InitSuccess = false;
	}

	while (!exitRequest) {
		if (InitSuccess) {
			if (!readPathsDone) {
				readDirBranch(workingPath, workingPathIndex, !INI_ProtectedComplete);

				string nextPath = "";
				for (auto it = PathStorage.begin(); it != PathStorage.end(); ++it) {
					if ((*it).pathScanned == false) {
						nextPath = (*it).pathName.string();
						workingPathIndex = (*it).pathNameIndex;
						break;
					}
				}

				if (nextPath == "") {
					string INI_Path;

					// report dir count and file count
					auto newFileTotal = FileStorage.size();
					auto newPathTotal = PathStorage.size();
					if (newPathTotal - lastPathTotal != 0) {
						cout << "    Paths read = " << (newPathTotal - lastPathTotal) << "   Files read = " << (newFileTotal - lastFileTotal) << endl;
					}
					lastPathTotal = newPathTotal;
					lastFileTotal = newFileTotal;

					if (!INI_ProtectedComplete) {
						INI_Path = Key_ProtectedPath + to_string(INI_PathProtectedCount + 1);
						workingPath = CIniFile::GetValue(INI_Path, SectionName_PathList, INI_FileName);
						if (CIniFile::DoesPathExist(workingPath)) {
							INI_PathProtectedCount++;
							cout << INI_Path << " successful read - " << workingPath << endl;
						}
						else {
							INI_ProtectedComplete = true;
						}
					}
					else {
						if (!INI_NormalComplete) {
							INI_Path = Key_NormalPath + to_string(INI_PathNormalCount + 1);
							workingPath = CIniFile::GetValue(INI_Path, SectionName_PathList, INI_FileName);
							if (CIniFile::DoesPathExist(workingPath)) {
								INI_PathNormalCount++;
								cout << INI_Path << " successful read - " << workingPath << endl;
							}
							else {
								INI_NormalComplete = true;
							}
						}
					}
					if (workingPath != "") {
						if (!INI_ProtectedComplete || !INI_NormalComplete) {
							struct pathInfo_t tempPath;
							tempPath.pathName = workingPath;
							tempPath.protectedFlag = !INI_ProtectedComplete;
							tempPath.pathScanned = false;
							tempPath.parentPathNameIndex = 0;
							tempPath.pathNameIndex = PathStorage.size() + 1;
							workingPathIndex = PathStorage.size() + 1;

							PathStorage.push_back(tempPath);
						}
						else {
							cout << "this shoulf never execute" << endl;
						}
					}
					else {
						if (INI_ProtectedComplete && INI_NormalComplete) {
							cout << "Read Path Done - " << workingPath << endl;
							readPathsDone = true;
						}
					}
				}
				else {
					workingPath = nextPath;
				}
			}
			if (!compareFilesDone) {
				compareFileEntries();
			}
		}
		if (readPathsDone && !CompareEnableFlag) {
			Sleep(500);
		}
		if (_kbhit() != 0) {
			char newChar = _getch();
			switch (newChar) {
			case 'q':
			case 'Q':
				exitRequest = true;
				break;
			case 'c':
				if (readPathsDone) {
					if (allTokenMatchEnable || countedMatchEnable || trimmedFileNameMatchEnable) {
						tokenPositiveFlag = true;
						CompareEnableFlag = !CompareEnableFlag;
						if (compareFilesDone) {
							initCompare();
						}
					}
					else {
						cout << "Compare not started - No Match Types Enabled" << endl;
					}
				}
				else {
					cout << "Compare not started - Read paths incomplete" << endl;
				}
				break;
			case 'C':
				if (readPathsDone) {
					if (allTokenMatchEnable || countedMatchEnable || trimmedFileNameMatchEnable) {
						tokenPositiveFlag = false;
						CompareEnableFlag = !CompareEnableFlag;
						if (compareFilesDone) {
							initCompare();
						}
					}
					else {
						cout << "Compare not started - No Match Types Enabled" << endl;
					}
				}
				else {
					cout << "Compare not started - Read paths incomplete" << endl;
				}
				break;
			case 'm':
			case 'M':
				rotationBits = matchRotation[rotationIndex];

				allTokenMatchEnable        = (rotationBits & 1) != 0;
				countedMatchEnable         = (rotationBits & 2) != 0;
				trimmedFileNameMatchEnable = (rotationBits & 4) != 0;
				cout << "All Word, Counted, Trimmed = "
					<< (allTokenMatchEnable ? "T" : "F") << ", "
					<< (countedMatchEnable ? "T" : "F") << ", "
					<< (trimmedFileNameMatchEnable ? "T" : "F") << "    " << rotationBits << " " << rotationIndex << endl;

				rotationIndex = ++rotationIndex & 7;
				if (rotationIndex == 0) rotationIndex++;
				break;
			case 'h':
			case 'H':
				cout << endl
					<< "Key Help" << endl
					<< " h, H - Show KEY help" << endl
					<< " q, Q - Exit this program" << endl
					<< " c, C - Compare Enable Toggle" << endl
					<< " f    - Files read listed by original name" << endl
					<< " F    - Files read listed by trimmed string" << endl
					<< " m, M - Match control" << endl
					<< " p, P - Paths read list" << endl
					<< " s, S - Status" << endl
					<< " -, + - change (+) match value (" << MIN_P_VAL << " to " << MAX_P_VAL << ")" << endl
					<< " <, > - change (-) match value (" << MIN_N_VAL << " to " << MAX_N_VAL << ")" << endl;
				break;
			case 'p':
			case 'P':
				for (auto it = PathStorage.begin(); it != PathStorage.end(); ++it) {
					cout << "Path = " << (*it).pathName.string() << "   " << (*it).pathNameIndex << ((*it).protectedFlag? "  Protected": "  Normal") << endl;
				};
				break;
			case 'f':
			case 'F':
				for (auto it = FileStorage.begin(); it != FileStorage.end(); ++it) {
					if (newChar == 'F') {
						cout << "Filename = " << (*it).fileName.string() << "   " << (*it).filePathIndex << endl;
					}
					else {
						cout << "Filename = " << (*it).trimmedFileName << "   " << (*it).filePathIndex << endl;
					}
				};
				break;
			case 's':
			case 'S':
				cout << "Protected Paths                = " << INI_PathProtectedCount << endl
					<< "Normal Paths                   = " << INI_PathNormalCount << endl
					<< "Paths Read                     = " << PathStorage.size() << endl
					<< "Files Read                     = " << FileStorage.size() << endl
					<< "All Word Match Enable          = " << (allTokenMatchEnable ? "True" : "False") << endl
					<< "Counted Match Enable           = " << (countedMatchEnable ? "True" : "False") << endl
					<< "Trimmed File Name Match Enable = " << (trimmedFileNameMatchEnable ? "True" : "False") << endl
					<< "Word Matching is set to (+) " << tokenPMatchValue << "   (-) " << tokenNMatchValue << endl;
				break;
			case '+':
				if (tokenPMatchValue < MIN_P_VAL || tokenPMatchValue > MAX_P_VAL) { 
					tokenPMatchValue = DEFAULT_P_VAL; 
					break;
				}
				if (tokenPMatchValue < MAX_P_VAL) {
					tokenPMatchValue++;
				}
				cout << "Word Matching is set to (+) " << tokenPMatchValue << "   (-) " << tokenNMatchValue << endl;
				break;
			case '-':
				if (tokenPMatchValue < MIN_P_VAL || tokenPMatchValue > MAX_P_VAL) {
					tokenPMatchValue = DEFAULT_P_VAL;
					break;
				}
				if (tokenPMatchValue > MIN_P_VAL) {
					tokenPMatchValue--;
				}
				cout << "Word Matching is set to (+) " << tokenPMatchValue << "   (-) " << tokenNMatchValue << endl;
				break;
			case '>':
				if (tokenNMatchValue < MIN_N_VAL || tokenNMatchValue > MAX_N_VAL) {
					tokenNMatchValue = DEFAULT_N_VAL;
					break;
				}
				if (tokenNMatchValue < MAX_N_VAL) {
					tokenNMatchValue++;
				}
				cout << "Word Matching is set to (+) " << tokenPMatchValue << "   (-) " << tokenNMatchValue << endl;
				break;
			case '<':
				if (tokenNMatchValue < MIN_N_VAL || tokenNMatchValue > MAX_P_VAL) {
					tokenNMatchValue = DEFAULT_N_VAL;
					break;
				}
				if (tokenNMatchValue > MIN_N_VAL) {
					tokenNMatchValue--;
				}
				cout << "Word Matching is set to (+) " << tokenPMatchValue << "   (-) " << tokenNMatchValue << endl;
				break;
			default:
				break;
			}
		}
	}

	return 0;
}   // main

void commandLineError(string errorInfo)
{
	cout << errorInfo << endl;
	cout << "DupNames.exe -f<file name> -i<file name -h" << endl;
	cout << "    -h help (this message) then exit" << endl
		<< "    -i name of the INI file" << endl
		<< "    -v verbose console output" << endl;
	cout << " Return Codes 0-Success" << endl
		<< "              1-command line error" << endl
		<< "              2-user request exit \"-h\" option OR \"q\" keystroke" << endl;
}   // commandLineError

void readDirBranch(const fsPath currentPath, size_t currentPathIndex, bool currentProtFlag)
{
	if (fs::exists(currentPath) && fs::is_directory(currentPath))
	{
		for (const auto& entry : fs::directory_iterator(currentPath))
		{
			fsPath filename = entry.path().filename();
			if (fs::is_directory(entry.status())) {
				// don't save the dot and dot-dot dir
				if ((filename == ".") || (filename == ".."))
					continue;

				// verify the dir is unique
				bool uniquePath = true;
				for (auto it = PathStorage.begin(); it != PathStorage.end(); ++it) {
					if ((currentPath.string() + filename.string()) == (*it).pathName.string()) {
						// if not unique only change the original prot status from false to true if needed
						if (!((*it).protectedFlag) && currentProtFlag) {
							(*it).protectedFlag = true;
							(*it).parentPathNameIndex = currentPathIndex;
						}
						uniquePath = false;
					}
				}
				if (!uniquePath) {
					continue;
				}

				// save dir info
				struct pathInfo_t tempPath;
				tempPath.parentPathNameIndex = currentPathIndex;
				tempPath.protectedFlag = currentProtFlag;
				tempPath.pathScanned = false;
				tempPath.pathNameIndex = PathStorage.size() + 1;
				tempPath.pathName = currentPath.string() + "\\" + filename.string();

				if (verboseOutputFlag) {
					cout << "Path = " << tempPath.pathName << "   " << tempPath.pathNameIndex << endl;
				}

				PathStorage.push_back(tempPath);
			}
			else if (fs::is_regular_file(entry.status()))
			{
				// weed out all non-movie file types / extensions
				string tmpExtension = entry.path().extension().string();
				std::for_each(tmpExtension.begin(), tmpExtension.end(), [](char& c) {
					c = ::tolower(c);                            // make the ext lower case
				});

				if ((tmpExtension != ".avi") && (tmpExtension != ".mkv") && (tmpExtension != ".m4v") && (tmpExtension != ".mp4"))
					continue;

				// replace punctuation with spaces
				string tmpFN = entry.path().filename().string();
				for (int i = 0; i < tmpFN.length(); i++) {
					char c = tmpFN[i];
					if ((c > 0) && (c < 255)) {
						if ((c == 39) && (tmpFN[i + 1] == 's') && !isalnum(tmpFN[i + 2])) {    // chech for "'s " or apostrophe, 's', non-alphanumeric
							tmpFN[i] = 's';
							tmpFN[i + 1] = ' ';
						}
						if (!isalnum(c)) {
							tmpFN[i] = ' ';
						}
					}
				};

				// remove sequences of multiple spaces
				removeSpaces(tmpFN);
				std::for_each(tmpFN.begin(), tmpFN.end(), [](char& c) {
					c = ::tolower(c);                            // make the ext lower case
				});

				struct fileInfo_t  tempFile;
				struct tokenInfo_t tempToken;

				// parse tokens stop after finding a valid year or MAX_TOKENS
				int j = -1;
				int i = 0;
				bool trimFlag = false;
				for (i = 0; (i < tmpFN.size()) && !trimFlag; i++) {
					if (tmpFN[i] != ' ') {
						if (j == -1) {
							j = i;
						}
					}
					else if ((i > 0) && (j != -1)) {
						bool numFlag = true;
						tempToken.tokenValue = tmpFN.substr(j, i - j);
						tempToken.matchFlag = false;
						tempFile.tokens.push_back(tempToken);
						for (int si = j; (si < i) && numFlag; si++) {
							if (!isdigit(tmpFN[si])) {
								numFlag = false;
							}
						}
						if (numFlag && (tempFile.tokens.size() > 1)) {
							int tempYear = stoi(tmpFN.substr(j, i));
							if ((tempYear >= MIN_YEAR) && (tempYear <= MAX_YEAR)) {
								tempFile.fileNameYear = tempYear;
								trimFlag = true;
							}
						}
						j = -1;
					}
				};
				tempFile.trimmedFileName = tmpFN.substr(0, i-1) + tmpExtension;

				// trim name following valid year and save 
				// save the path index to
				tempFile.filePathIndex = currentPathIndex;

				// save file info
				tempFile.fileName = filename;
				if (verboseOutputFlag) {
					cout << "Filename = " << filename << endl;
				}

				FileStorage.push_back(tempFile);
			}
		}

		for (auto it = PathStorage.begin(); it != PathStorage.end(); ++it) {
			if ((*it).pathName.string() == currentPath) {
				(*it).pathScanned = true;
				break;
			}
		}
	}
}   // readDirBranch

// Function to in-place trim all spaces in the string such that all words
// should contain only a single space between them. 
void removeSpaces(string& str)
{
	// n is length of the original string 
	size_t n = str.length();

	// i points to next position to be filled in output string
	// j points to next character in the original string 
	int i = 0;
	int j = -1;

	// flag that sets to true is space is found 
	bool spaceFound = false;

	// Handles leading spaces 
	while (++j < n && str[j] == ' ');

	// read all characters of original string 
	while (j < n) {
		// if current characters is non-space 
		if (str[j] != ' ') {
			// remove preceding spaces before dot, comma & question mark 
			if ((str[j] == '.' || str[j] == ',' ||
				str[j] == '?') && i - 1 >= 0 &&
				str[i - 1] == ' ') {
				str[i - 1] = str[j++];
			}
			else {
				str[i++] = str[j++];
			}

			// set space flag to false when any non-space character is found 
			spaceFound = false;
		}
		// if current character is a space 
		else if (str[j++] == ' ') {
			// If space is encountered for the first time after a word,
			// put one space in the output and set space flag to true 
			if (!spaceFound) {
				str[i++] = ' ';
				spaceFound = true;
			}
		}
	}

	// Remove trailing spaces 
	if (i <= 1)
		str.erase(str.begin() + i, str.end());
	else
		str.erase(str.begin() + i - 1, str.end());
}   // removeSpaces
