# TO DO:
 - Limit when sideInCheck is used
 - Checkmate/stalemate detection - this'll have to be through the engine (once we're looping over every square to generate moves we can check if the total of possible moves is 0)


 # Optimisations for looking for check
  - Tell sideInCheck() what piece moved and where and adjust the logic accordingly (to eliminate redundant stuff)