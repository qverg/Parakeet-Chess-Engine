# TO DO:
 - use .at() for unordered maps!!! and then adjust consts
 - add material diff attribute to move
 - pregenerate king/knight moves when a move is made?
 - Checkmate/stalemate detection - this'll have to be through the engine (once we're looping over every square to generate moves we can check if the total of possible moves is 0)
 - Implement halfmove clock & fullmove number in FEN reader

https://www.chessprogramming.org/Perft_Results

 ## Optimisations for looking for check
  - Tell sideInCheck() what piece moved and where and adjust the logic accordingly (this eliminates redundant stuff)
    - this is only really useful when there is no chance of discovered attack
