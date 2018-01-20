#!/bin/bash

clang-format -style=file -i src/* include/* example/*.cpp example/*.hpp template/*.cpp template/*.hpp tests/*.cpp tests/*.hpp
