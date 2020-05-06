#pragma once

#ifdef DUP_COMPARE_CPP
bool   compareFilesDone = true;
bool   CompareEnableFlag = false;

void compareFileEntries();
void initCompare();
std::vector<pathInfo_t>::iterator getFilePath_it(std::vector<fileInfo_t>::iterator file_it);
string fileProtectStr(std::vector<pathInfo_t>::iterator path_it);
string filePathStr(std::vector<pathInfo_t>::iterator path_it);

#else
extern void initCompare();

extern bool   compareFilesDone;
extern bool   CompareEnableFlag;
#endif