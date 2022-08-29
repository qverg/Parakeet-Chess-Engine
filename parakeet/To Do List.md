# TO DO:
 - inside Board::generateMovesInDirection(), somehow pass in the right extra directions to determine whether a move will lead to check
 - add willBeCheck parameter to all emplace_backs in Board::generateMoves()
     - for promos & promo captures (they are all marked as false for now)
 - Check for checks when moving out of the way (to reveal an attack) inside Board::generateMoves()
 - Prevent moving into check
 - Restrict moves when in check
      - Checkmate detection
 - No castling through check
 - Limit when sideInCheck is used