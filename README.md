# Producers consumers - An advanced and extensive example of the producer and consumer problem.

## Table of contents

- [Build](#build)
- [Executables](#executables)
  - [Bin folder](#bin-folder)
  - [Main shell](#main-shell)
  - [Tests][#executable-test-target]
    - [Valgrind][#valgrind]


## Build

The code is located in the 'pc' folder, being the file 'IPC.h' the interface entry point to create producers and consumers.

To build the project and the executable shells, go to folder 'build' and type:
- cmake ..
- make


## Executables

### Bin folder

If the build is successful, a 'build/bin/' folder is created with all the executable targets.

### Main shell

The main shell is an executable target to test the producer and consumer code.  
To execute the main shell, go to the bin folder and execute './pcshell'.  
The shell accepts the following commands:

0: Quits the application.  
1: Add a producer.  
2: Add a consumer.  
3: Remove a producer.  
4: Remove a consumer.  
5: Remove all producers.  
6: Remove all consumers.  
7: Add 5 producers.  
8: Add 5 consumers.  

The code of the shell is located in 'testApps/src/main.cpp'.

### Executable test target

This is an executable that implements some integration tests using the C++ Google Test Framework Gtest.
The code of the main test application is located in 'testApps/src/test.cpp'.

#### Valgrind

The implemented tests also use Valgrind client requests to check for memory leaks in each test case. 
To do so, the number of memory leaks is checked before and after each test case. If the number of memory leaks has increased after the test case, it means the code of the test case added memory leaks. 

The source files 'pc/src/valgrind_check.h' and 'pc/src/valgrind_check.cpp' shows how the Valgrind client requests are used.
To execute the test target under Valgrind, go to the bin folder and execute:

valgrind --leak-check=full ./pctest

