# TO DO:
 - No castling through check
 - Limit when sideInCheck is used
 - Checkmate/stalemate detection - this'll have to be through the engine (once we're looping over every square to generate moves we can check if the total of possible moves is 0)


 ## Optimisations for looking for check
  - Tell sideInCheck() what piece moved and where and adjust the logic accordingly (this eliminates redundant stuff)
   - this is only really useful when there is no chance of discovered attack
