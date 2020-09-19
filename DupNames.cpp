// DupNames.cpp : Defines the entry point for the application.
//
#define DUP_NAMES_CPP

//#include <iostream>
#include <time.h>

#include <stdio.h>
#include <windows.h>

#include "IniFile.h"
#include "DupNames.h"
#include "DupCompare.h"
#include "ScrUtil.h"

#pragma warning(disable : 26451)
#pragma warning(disable : 4267)

bool   exitRequest            = false;
bool   readPathsDone          = false;
bool   InitSuccess            = true;
bool   INI_NormalComplete     = false;
bool   INI_ProtectedComplete  = false;
bool   verboseOutputFlag      = false;

int    INI_PathProtectedCount = 0;
int    INI_PathNormalCount    = 0;
int    INI_TokenMatchCount    = 0;
int    workingPathIndex       = 0;
int    lastFileTotal          = 0;
int    lastPathTotal          = 0;

// default Filename for the INI status file
const string     default_INI_FileName    = "DupNames.ini";
const string     default_LOG_FileName    = "DupNames.log";
const string     SectionName_InitState   = "InitState";
const string     SectionName_PathList    = "PathList";

string           Key_ProtectedPath       = "ProtectedPath";
string           Key_NormalPath          = "NormalPath";
string           Key_TokenNMatchValue    = "TokenNMatchValue";
string           Key_TokenPMatchValue    = "TokenPMatchValue";
string           Key_TrimmedMatchEnable  = "TrimmedMatchEnable";
string           Key_AllTokenMatchEnable = "AllTokenMatchEnable";
string           Key_CountedMatchEnable  = "CountedMatchEnable";
string           Key_MovieMatchEnable    = "MovieMatchEnable";
string           Key_EpisodeMatchEnable  = "EpisodeMatchEnable";
string           workingPath;
string           tokenMatchStr;

string           INI_FileName = "";
string           LOG_FileName = "";

int main (int argc, char** argv)
{
    ScrUtil::setColors( ScrUtil::Yellow, ScrUtil::Blue );

	cout << "\nCommand Line and " << argc - 1 << " arguments:";

	cout << "\n   " << argv[0];
	if (argc > 1) {
		int i = 1;
		while (argv[i] != NULL) {
			cout << "\n   " << i << ", " << argv[i];
			if (argv[i][0] == '-' && argv[i][2] != ' ') {
				argv[i][1] = tolower(argv[i][1]);
				switch (argv[i][1]) {
				case 'd':
					allowFileDelete = true;
					cout << "\nDelete file queries";
					break;
				case 'h':
					commandLineError("Command Line Help -");
    				ScrUtil::setColors( ScrUtil::White, ScrUtil::Black );
					cout << endl;
					return 2;
				case 'i':
					INI_FileName = (string)&argv[i][2];
					cout << "\nINI file name set to " << INI_FileName;
					break;
				case 'L':
					logFileAppend = true;
				case 'l':
					logFileCreate = true;
					if (&argv[i][2] != 0)
						LOG_FileName = (string)&argv[i][2];
					else {
						fsPath tmpINI = argv[0];
						LOG_FileName = tmpINI.parent_path().string() + "\\" + default_LOG_FileName;
					}
					cout << "\nLOG file name set to " << LOG_FileName;
					if (!logFileAppend) {
						remove( const_cast<char*>(LOG_FileName.c_str()) );
					}
					break;
				case 's':
					allowSkip2ProtFiles = true;
					cout << "\nSkip query if both files are protected";
					break;
				case 'v':
					verboseOutputFlag = true;
					cout << "\nVerbose output enabled";
					break;
				default:
					commandLineError("invalid command line argument - " + (string)argv[i]);
    				ScrUtil::setColors( ScrUtil::White, ScrUtil::Black );
					cout << endl;
					return 1;
				}
			}
			else {
				commandLineError("no dash found in command line - " + (string)argv[i]);
    			ScrUtil::setColors( ScrUtil::White, ScrUtil::Black );
				cout << endl;
				return 1;
			}
			i++;
		}
	}

    ScrUtil::setColors( ScrUtil::White, ScrUtil::Black );
	cout << endl;

	if (INI_FileName == "") {
		if (CIniFile::DoesFileExist(default_INI_FileName)) {
			INI_FileName = default_INI_FileName;
		}
		else {
			fsPath tmpINI = argv[0];
			cout << tmpINI.root_path().string() << endl << tmpINI.parent_path().string() << endl;
			INI_FileName = tmpINI.parent_path().string() + "\\" + default_INI_FileName;
			cout << INI_FileName << endl;
		}
	}

	if (LOG_FileName != "") {
		logFileStream.open(LOG_FileName, std::ios::out | std::ios::app); // append instead of overwrite
		logFileStream << "Data";  // add arguments - time and date
	}

	cout << "Press Q to quit and H for help" << endl;

	if (CIniFile::DoesFileExist(INI_FileName)) {
		std::cout << endl << INI_FileName << " - File exists" << endl << endl;      // time to read the file

		if (CIniFile::DoesSectionExist(SectionName_InitState, INI_FileName)) {
			tokenMatchStr = CIniFile::GetValue(Key_TokenNMatchValue, SectionName_InitState, INI_FileName);
			if (tokenMatchStr != "") {
				try {
					tokenNMatchValue = stoi(tokenMatchStr);
				}
				catch (invalid_argument const &e) {
					cout << "Invalid Argument - tokenNMatchValue - " << e.what() << endl;
					tokenNMatchValue = DEFAULT_N_VAL;
				}
				catch (out_of_range const &e) {
					cout << "Out Of Range - tokenNMatchValue - " << e.what() << endl;
					tokenNMatchValue = DEFAULT_N_VAL;
				}
				if ((tokenNMatchValue < MIN_N_VAL) || (tokenNMatchValue > MAX_N_VAL)) {
					cout << "TokenNMatchValue - Out of Range - setting it to default\n";
					tokenNMatchValue = DEFAULT_N_VAL;
				}
			}
			tokenMatchStr = CIniFile::GetValue(Key_TokenPMatchValue, SectionName_InitState, INI_FileName);
			if (tokenMatchStr != "") {
				try {
					tokenPMatchValue = stoi(tokenMatchStr);
				}
				catch (invalid_argument const &e) {
					cout << "Invalid Argument - tokenPMatchValue - " << e.what() << endl;
					tokenPMatchValue = DEFAULT_P_VAL;
				}
				catch (out_of_range const &e) {
					cout << "Out Of Range - tokenPMatchValue - " << e.what() << endl;
					tokenPMatchValue = DEFAULT_P_VAL;
				}
				if ((tokenPMatchValue < MIN_P_VAL) || (tokenPMatchValue > MAX_P_VAL)) {
					cout << "TokenPMatchValue - Out of Range - setting it to default\n";
					tokenPMatchValue = DEFAULT_P_VAL;
				}
			}
			tokenMatchStr = CIniFile::GetValue(Key_TrimmedMatchEnable, SectionName_InitState, INI_FileName);
			if (tokenMatchStr != "") {
				std::for_each(tokenMatchStr.begin(), tokenMatchStr.end(), [](char& c) {
					c = ::tolower(c);                            // make the str lower case
				});
				if (tokenMatchStr == "enabled") {
					trimmedFileNameMatchEnable = true;
				}
				else if (tokenMatchStr == "disabled") {
					trimmedFileNameMatchEnable = false;
				}
			}
			tokenMatchStr = CIniFile::GetValue(Key_AllTokenMatchEnable, SectionName_InitState, INI_FileName);
			if (tokenMatchStr != "") {
				std::for_each(tokenMatchStr.begin(), tokenMatchStr.end(), [](char& c) {
					c = ::tolower(c);                            // make the str lower case
				});
				if (tokenMatchStr == "enabled") {
					allTokenMatchEnable = true;
				}
				else if (tokenMatchStr == "disabled") {
					allTokenMatchEnable = false;
				}
			}
			tokenMatchStr = CIniFile::GetValue(Key_CountedMatchEnable, SectionName_InitState, INI_FileName);
			if (tokenMatchStr != "") {
				std::for_each(tokenMatchStr.begin(), tokenMatchStr.end(), [](char& c) {
					c = ::tolower(c);                            // make the str lower case
				});
				if (tokenMatchStr == "enabled") {
					countedMatchEnable = true;
				}
				else if (tokenMatchStr == "disabled") {
					countedMatchEnable = false;
				}
			}
			tokenMatchStr = CIniFile::GetValue(Key_MovieMatchEnable, SectionName_InitState, INI_FileName);
			if (tokenMatchStr != "") {
				std::for_each(tokenMatchStr.begin(), tokenMatchStr.end(), [](char& c) {
					c = ::tolower(c);                            // make the str lower case
				});
				if (tokenMatchStr == "enabled") {
					movieMatchEnable = true;
				}
				else if (tokenMatchStr == "disabled") {
					movieMatchEnable = false;
				}
			}
			tokenMatchStr = CIniFile::GetValue(Key_EpisodeMatchEnable, SectionName_InitState, INI_FileName);
			if (tokenMatchStr != "") {
				std::for_each(tokenMatchStr.begin(), tokenMatchStr.end(), [](char& c) {
					c = ::tolower(c);                            // make the str lower case
				});
				if (tokenMatchStr == "enabled") {
					episodeMatchEnable = true;
				}
				else if (tokenMatchStr == "disabled") {
					episodeMatchEnable = false;
				}
			}
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
				while (_kbhit() != 0) {       // clear the keyboard queue
					char newChar = _getch();
				}
				cout << "1-Movies Match  2=Episodes Match  3=Trimmed Name Match  4=All Token Match  5=Counted Match  >" ;
				newChar = _getch();
				cout << endl;
				switch (newChar) {
					case '1':
						movieMatchEnable = !movieMatchEnable;
						break;
					case '2':
						episodeMatchEnable = !episodeMatchEnable;
						break;
					case '3':
						trimmedFileNameMatchEnable = !trimmedFileNameMatchEnable;
						break;
					case '4':
						allTokenMatchEnable = !allTokenMatchEnable;
						break;
					case '5':
						countedMatchEnable = !countedMatchEnable;
						break;
					default:
						break;
				}
				while (_kbhit() != 0) {       // clear the keyboard queue
					char newChar = _getch();
				}

				cout << "Movie Match        = " << (movieMatchEnable           ? "Enabled" : "Disabled") << endl;
				cout << "Episode Match      = " << (episodeMatchEnable         ? "Enabled" : "Disabled") << endl;
				cout << "Trimmed Name Match = " << (trimmedFileNameMatchEnable ? "Enabled" : "Disabled") << endl;
				cout << "All Token Match    = " << (allTokenMatchEnable        ? "Enabled" : "Disabled") << endl;
				cout << "Counted Match      = " << (countedMatchEnable         ? "Enabled" : "Disabled") << endl;

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
					<< "        1=Movie Match Toggle" << endl
					<< "        2=Episode Match Toggle" << endl
					<< "        3=Trimmed Name Match Toggle" << endl
					<< "        4=All Token Match Match Toggle" << endl
					<< "        5=Counted Match Toggle" << endl
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
					<< "Movie Match                    = " << (movieMatchEnable           ? "Enabled" : "Disabled") << endl
					<< "Episode Match                  = " << (episodeMatchEnable         ? "Enabled" : "Disabled") << endl
					<< "Trimmed Name Match             = " << (trimmedFileNameMatchEnable ? "Enabled" : "Disabled") << endl
					<< "All Token Match                = " << (allTokenMatchEnable        ? "Enabled" : "Disabled") << endl
					<< "Counted Match                  = " << (countedMatchEnable         ? "Enabled" : "Disabled") << endl
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
			while (_kbhit() != 0) {       // clear the keyboard queue
				char newChar = _getch();
			}
		}
	}

	return 0;
}   // main

void commandLineError(string errorInfo)
{
    ScrUtil::Attributes atr = ScrUtil::getCurrentAttributes();
    ScrUtil::setColors( ScrUtil::Cyan, ScrUtil::Black );
	cout << endl;

	cout << errorInfo << endl;
	cout << "DupNames.exe [options]" << endl;
	cout << "    -d  delete file queries" << endl
		<< "    -h  help (this message) then exit" << endl
		<< "    -i  INI file name \"-i<filename>\"" << endl
		<< "           default (no -i) = DupNames.ini" << endl
		<< "    -l  create a Log file\"-l<filename>\"" << endl
		<< "           default (no -l) = no log file" << endl
		<< "           default (-l)    = DupNames.log" << endl
		<< "    -L  append to Log file (see -l above for more)" << endl
		<< "    -s  skip query if both files are protected" << endl
		<< "    -v  verbose console output" << endl;
	cout << " Return Codes 0-Success" << endl
		<< "              1-command line error" << endl
		<< "              2-user request exit \"-h\" option OR \"q\" keystroke" << endl;

    ScrUtil::setColors( atr.ink, atr.paper );
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

				if (movieMatchEnable || episodeMatchEnable) {
					FileStorage.push_back(tempFile);
				}
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
