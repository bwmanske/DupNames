#define DUP_COMPARE_CPP

//#include <stdio.h>
#include <windows.h>

#include "DupNames.h"
#include "DupCompare.h"
#include "ScrUtil.h"

using namespace std;

int fileNameMatchCount = 0;
int matchesNeeded = 0;
int matchFlagCount = 0;

std::vector<fileInfo_t>::iterator it;

void initCompare() {
	it = FileStorage.begin();
	fileNameMatchCount = 0;
	compareFilesDone = false;
}

void compareFileEntries() {
	bool trimmedMatch  = false;
	bool allTokenMatch = false;
	bool countedMatch  = false;

	if (CompareEnableFlag) {
		std::vector<fileInfo_t>::iterator local_it;
		if (it != FileStorage.end()) {
			size_t p2_size = (*it).tokens.size();
			for (local_it = it + 1; local_it != FileStorage.end(); ++local_it) {
				size_t p1_size = (*local_it).tokens.size();

				if (allowFileDelete) {
					// skip comparison if one of the files if one has been deleted
					if (((*local_it).fileDeleted == true) ||
					    ((*it).fileDeleted       == true))
						continue;
				}

				if (allowSkip2ProtFiles) {
					// skip comparison if both files are protected
					if ((fileProtectStr(getFilePath_it(local_it)) == "P") &&
					    (fileProtectStr(getFilePath_it(it))       == "P")) {
						continue;
					}
				}

				if (trimmedFileNameMatchEnable) {
					// trimmed file name match
					if ((*local_it).trimmedFileName == (*it).trimmedFileName) {
						// trimmed file name match - success
						trimmedMatch = true;
					}
					else {
						// trimmed file name match - failed
						trimmedMatch = false;
					}
				}

				if (allTokenMatchEnable) {
					// all token match
					if (p1_size == p2_size) {
						allTokenMatch = true;
						auto p1 = (*local_it).tokens.begin();
						auto p2 = (*it).tokens.begin();
						for (; p2 != (*it).tokens.end(); ++p2) {
							(*p2).matchFlag = false;
						}

						for (; p1 != (*local_it).tokens.end(); ++p1) {
							bool p2_matchFound = false;
							p2 = (*it).tokens.begin();
							for (; p2 != (*it).tokens.end(); ++p2) {
								if (((*p1).tokenValue == (*p2).tokenValue) && ((*p2).matchFlag == false)) {
									(*p2).matchFlag = true;
									p2_matchFound = true;
									break;
								}
							}

							if (p2_matchFound == false) {
								allTokenMatch = false;
								break;
							}
						}
						// all token match - success
					}
					else {
						// all token match - failed
						allTokenMatch = false;
					}
				}

				if (countedMatchEnable) {
					// count matching tokens
					countedMatch = true;
					int sizeDifference = 0;
					int smallestTokenSetSize = 0;
					auto p1 = (*local_it).tokens.begin();
					auto p2 = (*it).tokens.begin();
					auto s1 = (*local_it).tokens.size();
					auto s2 = (*it).tokens.size();
					matchesNeeded = 0;

					if (tokenPositiveFlag) {
						matchesNeeded = tokenPMatchValue;
					}
					else {
						// match all except for tokenMatchValue
						sizeDifference = abs(int(s1 - s2));
						if (sizeDifference <= tokenNMatchValue) {
							matchesNeeded = max(int(s1), int(s2)) - tokenNMatchValue;
							if (matchesNeeded < tokenPMatchValue) {
								matchesNeeded = tokenPMatchValue;
							}
						}
						else {
							countedMatch = false;     // can't mathematically get the required number
						}
					}

					// match the tokens 
					if (countedMatch) {
						for (; p2 != (*it).tokens.end(); ++p2) {     // first clear the match flags
							(*p2).matchFlag = false;
						}

						for (; p1 != (*local_it).tokens.end(); ++p1) {
							p2 = (*it).tokens.begin();
							for (; p2 != (*it).tokens.end(); ++p2) {
								if (((*p1).tokenValue == (*p2).tokenValue) && ((*p2).matchFlag == false)) {
									(*p2).matchFlag = true;
									break;
								}
							}
						}

						// count up the matches
						matchFlagCount = 0;
						p2 = (*it).tokens.begin();
						for (; p2 != (*it).tokens.end(); ++p2) {
							if ((*p2).matchFlag == true) {
								matchFlagCount++;
							}
						}

						if (matchFlagCount < matchesNeeded) {     // do we have sufficient matches
							countedMatch = false;
						}
					}
				}

				// Notify of a match
				if (trimmedMatch || allTokenMatch || countedMatch) {
					auto filePath1 = getFilePath_it(local_it);
					auto filePath2 = getFilePath_it(it);
					string protStr1 = fileProtectStr(filePath1);
					string protStr2 = fileProtectStr(filePath2);
					string pathStr1 = filePathStr(filePath1);
					string pathStr2 = filePathStr(filePath2);

					cout << "Possible Match ";
					if (trimmedMatch) {
						cout << "(Trimmed Match) ";
					}
					if (allTokenMatch) {
						cout << "(All Token Match) ";
					}
					if (countedMatch) {
						cout << "(Counted Match " << (tokenPositiveFlag?"+":"-") << ") Needed=" << matchesNeeded << " Counted=" << matchFlagCount << "-";
						auto p2 = (*it).tokens.begin();
						for (; p2 != (*it).tokens.end(); ++p2) {
							if ((*p2).matchFlag == true) {
								cout << "\"" << (*p2).tokenValue << "\" ";
							}
						}
					}
					cout << endl;

					if (allowFileDelete) {
						ScrUtil::Attributes atr = ScrUtil::getCurrentAttributes();

						ScrUtil::Color colour = ( protStr1 == "N" ) ? ScrUtil::Green : ScrUtil::Red;
					    ScrUtil::setColors( colour, ScrUtil::Black );
						cout << (colour == ScrUtil::Green) ? "(1) " : "    ";
						cout << protStr1 << " " << (*local_it).fileName.string() << endl;
						cout << "      - " << pathStr1 << endl;

						colour = ( protStr2 == "N" ) ? ScrUtil::Green : ScrUtil::Red;
					    ScrUtil::setColors( colour, ScrUtil::Black );
						cout << (colour == ScrUtil::Green) ? "(2) " : "    ";
						cout << protStr2 << " " << (*it).fileName.string() << endl;
						cout << "      - " << pathStr2 << endl;

					    ScrUtil::setColors( atr.ink, atr.paper );
						cout << "Delete file (1) (2) ";
						if (logFileCreate)
							cout << "(space) to skip or (L) to log\n";
						else
							cout << "or (space) to skip\n";
						
						char newChar;
						while (_kbhit() != 0) {             // clear the input buffer
							newChar = _getch();
						}
						cout << endl;
//						cout << "input buffer cleared" << endl;

						bool waitForIt = true;
						while (waitForIt) {
							if (_kbhit() != 0) {
								newChar = _getch();
								switch (newChar) {
								case '1':
									cout << newChar;
									if (protStr1 == "N") {
								    	waitForIt = false;
									}
									break;
								case '2':
									cout << newChar;
									if (protStr2 == "N") {
										waitForIt = false;
									}
									break;
								case ' ':
									waitForIt = false;
									break;
								case 'l':
								case 'L':
									waitForIt = false;
									break;
								default:
									cout << newChar;
									Sleep(250);
									break;
								}
							}
							else {
								Sleep(250);
							}
						}
						if (newChar == '1') {
							string completeFileName1 = pathStr1 + '\\' + (*local_it).fileName.string();
#if 0								
							if ( remove( completeFileName1.c_str() ) != 0 )
								cout << "Error deleting - " << completeFileName1;
							else
								cout << completeFileName1 << " - successfully deleted";
#else								
							cout << "Deleting..." << completeFileName1 << endl;
#endif									
							(*local_it).fileDeleted = true;
						}
						if (newChar == '2') {
							string completeFileName2 = pathStr2 + '\\' + (*it).fileName.string();
#if 0
							if ( remove( completeFileName2.c_str() ) != 0 )
								cout << "Error deleting - " << completeFileName2;
							else
								cout << completeFileName2 << " - successfully deleted";
#else								
							cout << "Deleting..." << completeFileName2 << endl;
#endif									
							(*it).fileDeleted = true;
						}
					}
					else {
						cout << "    " << protStr1 << " " << (*local_it).fileName.string() << endl;
						cout << "      - " << pathStr1 << endl;
						cout << "    " << protStr2 << " " << (*it).fileName.string() << endl;
						cout << "      - " << pathStr2 << endl;
						cout << endl;
					}

					if (logFileCreate) {
						logFileStream << "    " << protStr1 << " " << (*local_it).fileName.string() << endl;
						logFileStream << "      - " << pathStr1 << endl;
						logFileStream << "    " << protStr2 << " " << (*it).fileName.string() << endl;
						logFileStream << "      - " << pathStr2 << endl;
						logFileStream << endl;
					}
					fileNameMatchCount++;
				}
			}
			it++;
		}
		else {
			cout << "Compare Done - Match count " << fileNameMatchCount << endl;
			compareFilesDone = true;
			CompareEnableFlag = false;
		}
	}
}   // compareFileEntries

std::vector<pathInfo_t>::iterator getFilePath_it(std::vector<fileInfo_t>::iterator file_it) {
	auto path_it = PathStorage.begin();

	for (; path_it != PathStorage.end(); path_it++) {
		if ((*path_it).pathNameIndex == (*file_it).filePathIndex) {
			break;
		}
	}
	return(path_it);
}   // getFilePath_it

string fileProtectStr(std::vector<pathInfo_t>::iterator path_it) {
	string retVal = "";

	if (path_it == PathStorage.end()) {
		retVal = "X";
	}
	else {
		if ((*path_it).protectedFlag) {
			retVal = "P";
		} else {
			retVal = "N";
		}
	}
	return(retVal);
}   // fileProtectStr

string filePathStr(std::vector<pathInfo_t>::iterator path_it) {
	string retVal = "";

	if (path_it == PathStorage.end()) {
		retVal = "Path logic ERROR";
	}
	else {
		retVal = (*path_it).pathName.string();
	}
	return(retVal);
}   // filePathStr
