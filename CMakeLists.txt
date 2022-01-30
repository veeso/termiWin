cmake_minimum_required(VERSION 3.7.2)
project(termiwin VERSION 1.2.1)
set("PROJECT_DESCRIPTION" "termiWin: a termios porting for Windows")
set("PROJECT_HOMEPAGE_URL" "https://github.com/veeso/termiWin")

set(CPACK_PACKAGE_VENDOR "Christian Visintin")

set(OUR_CMAKE_MODULES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(OUR_CMAKE_3PARTY_MODULES_DIR "${OUR_CMAKE_MODULES_DIR}/thirdParty")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${OUR_CMAKE_MODULES_DIR}" "${OUR_CMAKE_3PARTY_MODULES_DIR}" "${OUR_CMAKE_3PARTY_MODULES_DIR}/Boilerplate" "${OUR_CMAKE_3PARTY_MODULES_DIR}/DoxygenUtils")

include(Boilerplate)
#include(DoxygenUtils)

# Dirs
set (MAIN_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/")
set (MAIN_INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include/")

add_subdirectory("${MAIN_SRC_DIR}")

# Install rules
#install(TARGETS termiwin_shared CONFIGURATIONS Release LIBRARY DESTINATION lib PUBLIC_HEADER DESTINATION include)
#install(TARGETS termiwin_static CONFIGURATIONS Release ARCHIVE DESTINATION lib)
#install(FILES ${INCLUDE_FILES} DESTINATION include)

include(CPack)
