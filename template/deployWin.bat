@echo off
@echo Cleaning deployment directory...
del /S /Q deploy
rmdir deploy\media\
rmdir deploy\plugin\
rmdir deploy\compositor\
rmdir deploy\hlms
rmdir deploy
@echo done!
@echo deploying game...
mkdir deploy
copy Game.exe deploy\
copy rift_launcher.bat deploy\
copy vive_launcher.bat deploy\
copy novr_launcher.bat deploy\
copy *.dll deploy\
copy *.cfg deploy\
xcopy plugin deploy\plugin\ /s /e
xcopy media deploy\media\ /s /e
xcopy compositor deploy\compositor\ /s /e
xcopy hlms deploy\hlms\ /s /e


@echo done!
