@echo off
cd ..\src
g++ .\main.cpp .\board.cpp .\move.cpp .\engine.cpp .\utility.cpp .\timer.cpp .\types\movecounter.cpp -o ..\bin\parakeet
cd ..\parakeet-gui