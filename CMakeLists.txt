cmake_minimum_required(VERSION 3.1.0)
project (exec)
include(CTest)
set(CMAKE_VERBOSE_MAKEFILE ON)
enable_testing()

# path variables

set(LIB_DIR ${PROJECT_SOURCE_DIR}/deps/lib)
set(INCL_DIR ${PROJECT_SOURCE_DIR}/deps/include)

# prevent for in-source building
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_DISABLE_SOURCE_CHANGES  ON)
if ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
message(SEND_ERROR "In-source builds are not allowed.")
endif ()

add_definitions(-DPROJECTPATH=\"${PROJECT_SOURCE_DIR}\")

#set(CMAKE_BUILD_TYPE Debug) #Release
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_DEBUG_INIT "-Wall")
set(CMAKE_CXX_FLAGS_DEBUG "-Wall -g -D DEBUG")
set(CMAKE_CXX_FLAGS_RELEASE_INIT "-Wall -O2")
set(CMAKE_COLOR_MAKEFILE TRUE)

if(WIN32)
    if(${CMAKE_BUILD_TYPE} MATCHES Release)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mwindows")
    endif()
    #set(SFML_DIR "${PROJECT_SOURCE_DIR}/deps/lib/cmake/SFML")
endif(WIN32)

# dependency area
#set(SFML_STATIC_LIBRARIES TRUE)
#find_package(SFML 2.5.1 COMPONENTS audio graphics window system REQUIRED)

#find_package(Freetype PATHS ${PROJECT_SOURCE_DIR}/deps_sfml)

find_package(OpenGL)
find_package(wxWidgets REQUIRED gl core base OPTIONAL_COMPONENTS net PATHS "C:/Program Files (x86)/wxWidgets")
#include(${wxWidgets_USE_FILE})

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()




if(${CMAKE_BUILD_TYPE} MATCHES Debug)
    message("Debug build.")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/debug)
elseif(${CMAKE_BUILD_TYPE} MATCHES Release)
    message("Release build.")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/release)
endif()
set(EXE_PATH ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME})


set(HEADERS ${PROJECT_SOURCE_DIR}/include)
include_directories(${HEADERS})

file(GLOB SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")

add_executable(${PROJECT_NAME} main.cpp ${SOURCES})

#-lfreetype -lopengl32 -lwinmm -lgdi32 -lstdc++
target_link_libraries(${PROJECT_NAME} -static -fopenmp -lwinpthread -static-libgcc -static-libstdc++  wx::core wx::base wx::gl ${wxWidgets_LIBRARIES} -lm)
target_include_directories(${PROJECT_NAME} PRIVATE ${INCL_DIR} )

if(${CMAKE_BUILD_TYPE} MATCHES Debug)
	if(WIN32)
		add_test(NAME launch COMMAND ${EXE_PATH}.exe)
		add_test(NAME leak COMMAND drmemory -exit_code_if_errors 1 -results_to_stderr -batch -- ${EXE_PATH}.exe)
	elseif(UNIX)
		add_test(NAME launch COMMAND ${EXE_PATH})
		add_test(NAME leak COMMAND valgrind --leak-check=full --track-origins=yes --error-exitcode=1 ${EXE_PATH})
	endif()
endif()



option(BUILD_DOXY_DOC "build doxygen documentation" ON)
if(BUILD_DOXY_DOC)
    find_package(Doxygen)
    if(NOT DOXYGEN_FOUND)
        message(WARNING "Doxygen not found, unable to generate documentation")
    else()
        set(DOXY_DOC_DEST_DIR ${CMAKE_BINARY_DIR} CACHE PATH "Path to the doxygen output")                      ## used in the doxyfile.in
        set(DOXY_DOC_INPUT_ROOT_DIRS ${CMAKE_HOME_DIRECTORY} CACHE PATH "Path from where to start to parse")    ## used in the doxyfile.in
        set(DOXY_DOC_EXCLUDE_PATTERNS_DIRS ${CMAKE_BINARY_DIR} CACHE STRING "Regex patterns exclusion")          ## used in the doxyfile.in
        set(DOXY_DOC_COMMON_IMG_PATH ${CMAKE_CURRENT_SOURCE_DIR} CACHE PATH "Path from which doxygen will be able to found referenced images")## used in the doxyfile.in
        configure_file(Doxyfile ${CMAKE_BINARY_DIR}/Doxyfile @ONLY)
        add_custom_target(doxyDoc COMMAND ${DOXYGEN_EXECUTABLE} "${CMAKE_BINARY_DIR}/doxyfile"
            WORKING_DIRECTORY ${CMAKE_HOME_DIRECTORY}
            COMMENT "Building user's documentation into doxyDoc build dir..."
        )
    endif()
endif()