# Parakeet Chess Engine

### Requirements 
#### C++ code
 - Windows probably, not tested on other OSs but might work
 - I'm using g++ to compile (C++ 14 or higher).

#### GUI (not necessary for just running the engine)
 - Definitely windows
 - Python 3 (I'm using 3.10)
    - pygame
    - pexpect

### Running
##### Engine (parakeet.exe)
Compile using ``.\compile.bat`` inside parakeet/src if you want and and run using ``.\run.bat``. Alternatively, just run parakeet.exe in parakeet/bin.

### GUI (parakeet-gui.py)
Compile using ``.\compile.bat`` inside parakeet/parakeet-gui if you want and run using ``.\run.bat``.
Keep in mind the GUI is a little buggy and may randomly decide not to work.

### Commands
| Command   | Function  |
|-----------|-----------|
| ``$quit``  | Quit parakeet |
| ``$reset`` | Resets board to normal starting position |
| ``$play``  | Calculates what it thinks the best move is, plays it and displays it |
| ``$testmovegen`` | Test move generation by counting the number of available moves in the position |
| ``$exitboard`` | Go back to the starting prompt where you can either ``$reset`` or enter a FEN |
| ``$getposition`` | Prints the current position. Capital letters are white, lowercase black, dots are empty. It goes from a1 to h1, a2 to h2 and so on to h8. |
| 