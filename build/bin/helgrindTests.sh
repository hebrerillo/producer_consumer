#! /bin/bash

#Execute the tests using the Valgrind tool helgrind with a suppression file to get rid of errors produced by the Gtest suite.
valgrind --tool=helgrind --suppressions=helgrind.supp ./pctest --gtest_break_on_failure
