@echo off
cd ..\src
g++ .\main.cpp .\board.cpp .\log.cpp .\move.cpp .\engine.cpp .\utility.cpp -o parakeet
cd ..\parakeet-gui