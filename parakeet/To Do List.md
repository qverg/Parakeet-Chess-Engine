# TO DO:
 - Limit when sideInCheck is used
 - pregenerate king/knight moves when a move is made?
 - Checkmate/stalemate detection - this'll have to be through the engine (once we're looping over every square to generate moves we can check if the total of possible moves is 0)


<<<<<<< HEAD
 # Optimisations for looking for check
  - Tell sideInCheck() what piece moved and where and adjust the logic accordingly (to eliminate redundant stuff)
=======
 ## Optimisations for looking for check
  - Tell sideInCheck() what piece moved and where and adjust the logic accordingly (this eliminates redundant stuff)
   - this is only really useful when there is no chance of discovered attack
>>>>>>> 6cfa2cd31ff9cd1b6c409b2c24f9ba1a2346e91f
