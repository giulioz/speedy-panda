cmake_minimum_required(VERSION 3.12)
project(speedy-panda)

include(ExternalProject)
find_package(Git REQUIRED)

find_package(OpenMP REQUIRED)

ExternalProject_Add(
    args
    PREFIX ${CMAKE_BINARY_DIR}/args
    GIT_REPOSITORY https://github.com/Taywee/args
    TIMEOUT 10
    UPDATE_COMMAND ${GIT_EXECUTABLE} pull
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    LOG_DOWNLOAD ON
)
ExternalProject_Get_Property(args source_dir)
set(ARGS_INCLUDE_DIR ${source_dir} CACHE INTERNAL "Path to include folder for args")



set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_FLAGS "-Wall --pedantic -O3 -g -finline-functions -march=native")
#set(CMAKE_CXX_FLAGS "-Wall --pedantic -O3 -gdwarf -finline-functions -march=native -fopenmp")
set(CMAKE_CXX_FLAGS "-Wall --pedantic -O3 -gdwarf -finline-functions -march=native -Xpreprocessor -fopenmp")
#set(CMAKE_CXX_FLAGS "-Wall --pedantic -O3 -gdwarf -finline-functions -march=native -Xpreprocessor -fopenmp -fsanitize=address")
#set(CMAKE_CXX_FLAGS "-Wall --pedantic -g -finline-functions -march=native")

set(LIB_SOURCE_FILES)

add_executable(speedy-panda src/main.cc ${LIB_SOURCE_FILES})
target_link_libraries(speedy-panda PRIVATE OpenMP::OpenMP_CXX)
add_dependencies(speedy-panda args)
target_include_directories(speedy-panda PUBLIC ${ARGS_INCLUDE_DIR})



# TESTING
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
target_link_libraries(Panda_test PRIVATE OpenMP::OpenMP_CXX)
add_dependencies(Panda_test doctest)
target_include_directories(Panda_test PUBLIC ${DOCTEST_INCLUDE_DIR})
add_test(Panda Panda_test)

add_executable(TransactionList_test tests/TransactionList.test.cc ${LIB_SOURCE_FILES} tests/doctest.cc)
target_link_libraries(TransactionList_test PRIVATE OpenMP::OpenMP_CXX)
add_dependencies(TransactionList_test doctest)
target_include_directories(TransactionList_test PUBLIC ${DOCTEST_INCLUDE_DIR})
add_test(TransactionList TransactionList_test)

add_executable(PatternList_test tests/PatternList.test.cc ${LIB_SOURCE_FILES} tests/doctest.cc)
target_link_libraries(PatternList_test PRIVATE OpenMP::OpenMP_CXX)
add_dependencies(PatternList_test doctest)
target_include_directories(PatternList_test PUBLIC ${DOCTEST_INCLUDE_DIR})
add_test(PatternList PatternList_test)
