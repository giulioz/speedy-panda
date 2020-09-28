cmake_minimum_required(VERSION 3.10)
project(speedy-panda)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall --pedantic -O3 -g -finline-functions -march=native")
#set(CMAKE_CXX_FLAGS "-Wall --pedantic -g -finline-functions -march=native")

set(LIB_SOURCE_FILES)

add_executable(speedy-panda src/main.cc ${LIB_SOURCE_FILES})



# TESTING
include(ExternalProject)
find_package(Git REQUIRED)
ExternalProject_Add(
    doctest
    PREFIX ${CMAKE_BINARY_DIR}/doctest
    GIT_REPOSITORY https://github.com/onqtam/doctest.git
    TIMEOUT 10
    UPDATE_COMMAND ${GIT_EXECUTABLE} pull
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    LOG_DOWNLOAD ON
)
ExternalProject_Get_Property(doctest source_dir)
set(DOCTEST_INCLUDE_DIR ${source_dir}/doctest CACHE INTERNAL "Path to include folder for doctest")
enable_testing()

add_executable(Panda_test tests/Panda.test.cc ${LIB_SOURCE_FILES} tests/doctest.cc)
target_compile_features(Panda_test PRIVATE cxx_std_17)
target_include_directories(Panda_test PUBLIC ${DOCTEST_INCLUDE_DIR})
add_test(Panda Panda_test)

add_executable(TransactionList_test tests/TransactionList.test.cc ${LIB_SOURCE_FILES} tests/doctest.cc)
target_compile_features(TransactionList_test PRIVATE cxx_std_17)
target_include_directories(TransactionList_test PUBLIC ${DOCTEST_INCLUDE_DIR})
add_test(TransactionList TransactionList_test)

add_executable(PatternList_test tests/PatternList.test.cc ${LIB_SOURCE_FILES} tests/doctest.cc)
target_compile_features(PatternList_test PRIVATE cxx_std_17)
target_include_directories(PatternList_test PUBLIC ${DOCTEST_INCLUDE_DIR})
add_test(PatternList PatternList_test)
