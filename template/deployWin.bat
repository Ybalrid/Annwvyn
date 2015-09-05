@echo off
@echo Cleaning deployment directory...
del /S /Q deploy
rmdir deploy\media\
rmdir deploy\plugin\
rmdir deploy
@echo done!
@echo deploying game...
mkdir deploy
copy Game.exe deploy\
copy *.dll deploy\
copy *.cfg deploy\
xcopy plugin deploy\plugin\
xcopy media deploy\media\
@echo done!