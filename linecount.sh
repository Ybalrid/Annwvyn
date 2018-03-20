#!/bin/bash
cat src/* include/* renderer/*.txt renderer/Oculus/* renderer/OpenVR/* example/*.chai example/*.cpp example/*.hpp template/*.cpp template/*.hpp template/*.chai tests/*.cpp test/*.hpp 2>/dev/null | wc -l
