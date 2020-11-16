cmake_minimum_required(VERSION 3.0)
project(termiwin VERSION 1.2.1)

# Dirs
set (MAIN_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/")
set (MAIN_INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include/")

# Source dirs
file (GLOB MAIN_SRC
  "${MAIN_SRC_DIR}/*.c"
)
#Includes
file(GLOB INCLUDE_FILES
  "${MAIN_INC_DIR}/*.h"
)

include_directories(
  "${MAIN_INC_DIR}/"
)

if (TERMIWIN_DONOTREDEFINE)
add_definitions("-DTERMIWIN_DONOTREDEFINE")
endif (TERMIWIN_DONOTREDEFINE)

# Add target
add_library(termiwin_shared SHARED ${MAIN_SRC})
set_target_properties(termiwin_shared PROPERTIES OUTPUT_NAME termiwin)
set_target_properties(termiwin_shared PROPERTIES VERSION ${PROJECT_VERSION})
add_library(termiwin_static STATIC ${MAIN_SRC})
set_target_properties(termiwin_static PROPERTIES OUTPUT_NAME termiwin)

# Install rules
install(TARGETS termiwin_shared CONFIGURATIONS Release LIBRARY DESTINATION lib PUBLIC_HEADER DESTINATION include)
install(TARGETS termiwin_static CONFIGURATIONS Release ARCHIVE DESTINATION lib)
install(FILES ${INCLUDE_FILES} DESTINATION include)
