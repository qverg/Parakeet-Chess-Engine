@echo off
cd ..\src
g++ .\main.cpp .\board.cpp .\move.cpp .\engine.cpp .\utility.cpp -o parakeet
cd ..\parakeet-gui