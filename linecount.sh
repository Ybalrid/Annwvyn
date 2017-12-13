#!/bin/bash
cat src/* include/* example/*.chai example/*.cpp example/*.hpp template/*.cpp template/*.hpp template/*.chai tests/*.cpp test/*.hpp 2>/dev/null | wc -l
