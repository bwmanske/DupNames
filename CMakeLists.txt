cmake_minimum_required(VERSION 3.8.0)
project(DupNames VERSION 0.1.0)

include(CTest)
enable_testing()

add_executable(DupNames "DupNames.cpp"   "DupNames.h"   "IniFile.cpp" "IniFile.h" 
                        "DupCompare.cpp" "DupCompare.h" "ScrUtil.cpp" "ScrUtil.h")
set_property (TARGET DupNames
  PROPERTY
    # Enable C++17 standard compliance
    CXX_STANDARD 17
)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
