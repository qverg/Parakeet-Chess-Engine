"""
This script will generate the bitboards needed in the C++ code

Possible moves (64 element arrays of bitboards):
 - King
 - Queen
 - Bishop
 - Knight
 - Rook
 - Pawn
    - normal
    - captures

Starting positions (12 bitboards):
White: King, Queen, Bishops, Knights, Rooks, Pawns
Black: King, Queen, Bishops, Knights, Rooks, Pawns

Bitboards are stored in C++ as bitsets with 64 elements, indexed from right to left.
The indices correspond to the following squares on the chess board:

rank 8   56 57 58 59 60 61 62 63
     7   48 49 50 51 52 53 54 55
     6   40 41 42 43 44 45 46 47
     5   32 33 34 35 36 37 38 39
     4   24 25 26 27 28 29 30 31
     3   16 17 18 19 20 21 22 23
     2   8  9  10 11 12 13 14 15
     1   0  1  2  3  4  5  6  7

file     1  2  3  4  5  6  7  8
"""

def squareToCoords(square: int) -> tuple:
    return square % 8, int(square / 8)

def coordsToSquare(coords: tuple) -> int:
    return coords[1] * 8 + coords[0]

def north(coord: tuple) -> tuple:
    return coord[0], coord[1]+1

def south(coord: tuple) -> tuple:
    return coord[0], coord[1]-1

def east(coord: tuple) -> tuple:
    return coord[0]+1, coord[1]

def west(coord: tuple) -> tuple:
    return coord[0]-1, coord[1]

def northeast(coord: tuple) -> tuple:
    return north(east(coord))

def southeast(coord: tuple) -> tuple:
    return south(east(coord))

def northwest(coord: tuple) -> tuple:
    return north(west(coord))

def southwest(coord: tuple) -> tuple:
    return south(west(coord))

def withinBounds(coord: tuple) -> bool:
    return coord[0] >= 0 and coord[0] < 8 and coord[1] >= 0 and coord[1] < 8

def squareListToBitString(squares: list[int]) -> str:
    bitString = ""
    for i in range(64):
        if i in squares:
            bitString = "1" + bitString
        else:
            bitString = "0" + bitString
    return bitString

def bitStringOr(b1: str, b2: str) -> str:
    bitString = ""
    for i, c in enumerate(b1):
        if c == "1" or b2[i] == "1":
            bitString += "1"
        else:
            bitString += "0"
    return bitString

def printBitString(b: str):
    out = ""
    for i in range(8):
        out += (b[8*i] + b[8*i+1] + b[8*i+2] + b[8*i+3] + b[8*i+4] + b[8*i+5] + b[8*i+6] + b[8*i+7])[::-1] + "\n"

    print(out)

def printOutput(moveDict: dict, pos: int):
    printBitString(moveDict[squareListToBitString([pos])])


#======================================================================================================

def generateKingMoves() -> dict[str: str]:
    moves = {}
    
    for i in range(64): # for every square
        c = squareToCoords(i)
        candidates = [north(c), south(c), east(c), west(c), northeast(c), southeast(c), northwest(c), southwest(c)]
        possible = []
        for candidate in candidates:
            if withinBounds(candidate):
                possible.append(coordsToSquare(candidate))
        
        moves[squareListToBitString([i])] = squareListToBitString(possible)

    return moves

def generateRookMoves() -> dict[str: str]:
    moves = {}

    for i in range(64):
        c = squareToCoords(i)
        candidates = []
        last = [c, c, c, c]
        for j in range(8):
            last = [north(last[0]), south(last[1]), east(last[2]), west(last[3])]
            candidates.extend(last)

        possible = []
        for candidate in candidates:
            if withinBounds(candidate):
                possible.append(coordsToSquare(candidate))

        moves[squareListToBitString([i])] = squareListToBitString(possible)

    return moves

def generateBishopMoves() -> dict[str: str]:
    moves = {}

    for i in range(64):
        c = squareToCoords(i)
        candidates = []
        last = [c, c, c, c]
        for j in range(8):
            last = [northeast(last[0]), southeast(last[1]), northwest(last[2]), southwest(last[3])]
            candidates.extend(last)

        possible = []
        for candidate in candidates:
            if withinBounds(candidate):
                possible.append(coordsToSquare(candidate))
        
        moves[squareListToBitString([i])] = squareListToBitString(possible)

    return moves

def generateQueenMoves() -> dict[str: str]:
    moves = {}
    bishopMoves = generateBishopMoves()
    rookMoves = generateRookMoves()
    
    for i in range(64):
        pos = squareListToBitString([i])

        moves[pos] = bitStringOr(bishopMoves[pos], rookMoves[pos])

    return moves

def generateKnightMoves() -> dict[str: str]:
    moves = {}

    for i in range(64): # for every square
        c = squareToCoords(i)
        candidates = [
            north(north(east(c))), north(north(west(c))), north(east(east(c))), north(west(west(c))),
            south(south(east(c))), south(south(west(c))), south(east(east(c))), south(west(west(c)))]
        possible = []
        for candidate in candidates:
            if withinBounds(candidate):
                possible.append(coordsToSquare(candidate))
        
        moves[squareListToBitString([i])] = squareListToBitString(possible)

    return moves

def generateBlackPawnMovesNormal() -> dict[str: str]:
    moves = {}

    for i in range(64):
        c = squareToCoords(i)
        candidates = [south(c)]
        if c[1] == 6:
            candidates.append(south(south(c)))

        possible = []
        for candidate in candidates:
            if withinBounds(candidate):
                possible.append(coordsToSquare(candidate))
        
        moves[squareListToBitString([i])] = squareListToBitString(possible)

    return moves

def generateWhitePawnMovesNormal() -> dict[str: str]:
    moves = {}

    for i in range(64):
        c = squareToCoords(i)
        candidates = [north(c)]
        if c[1] == 1:
            candidates.append(north(north(c)))

        possible = []
        for candidate in candidates:
            if withinBounds(candidate):
                possible.append(coordsToSquare(candidate))
        
        moves[squareListToBitString([i])] = squareListToBitString(possible)

    return moves

def generateBlackPawnMovesCaptures() -> dict[str: str]:
    moves = {}

    for i in range(64):
        c = squareToCoords(i)
        candidates = [southeast(c), southwest(c)]

        possible = []
        for candidate in candidates:
            if withinBounds(candidate):
                possible.append(coordsToSquare(candidate))
        
        moves[squareListToBitString([i])] = squareListToBitString(possible)

    return moves

def generateWhitePawnMovesCaptures() -> dict[str: str]:
    moves = {}

    for i in range(64):
        c = squareToCoords(i)
        candidates = [northeast(c), northwest(c)]

        possible = []
        for candidate in candidates:
            if withinBounds(candidate):
                possible.append(coordsToSquare(candidate))
        
        moves[squareListToBitString([i])] = squareListToBitString(possible)

    return moves

NORTH = 0
SOUTH = 1
EAST = 2
WEST = 3

NORTHEAST = 4
SOUTHEAST = 5
NORTHWEST = 6
SOUTHWEST = 7

DIRECTIONS = ["NORTH", "SOUTH", "EAST", "WEST", "NORTHEAST", "SOUTHEAST", "NORTHWEST", "SOUTHWEST"]

def getMovesInDirection(c: tuple, direction) -> str:
    moves = []
    last = c
    while withinBounds(direction(last)):
        last = direction(last)
        moves.append(coordsToSquare(last))
    return squareListToBitString(moves)

def generateCardinals() -> list[list[str]]:
    rays = [[], [], [], [], [], [], [], []]
    for i in range(64):
        c = squareToCoords(i)

        rays[NORTH].append(getMovesInDirection(c, north))
        rays[SOUTH].append(getMovesInDirection(c, south))
        rays[EAST].append(getMovesInDirection(c, east))
        rays[WEST].append(getMovesInDirection(c, west))
        rays[NORTHEAST].append(getMovesInDirection(c, northeast))
        rays[SOUTHEAST].append(getMovesInDirection(c, southeast))
        rays[NORTHWEST].append(getMovesInDirection(c, northwest))
        rays[SOUTHWEST].append(getMovesInDirection(c, southwest))

    return rays


"""
kingMoves = generateKingMoves()
queenMoves = generateQueenMoves()
bishopMoves = generateBishopMoves()
knightMoves = generateKnightMoves()
rookMoves = generateRookMoves()

blackPawnsNormal = generateBlackPawnMovesNormal()
blackPawnsCaptures= generateBlackPawnMovesCaptures()
whitePawnsNormal = generateWhitePawnMovesNormal()
whitePawnsCaptures= generateWhitePawnMovesCaptures()


for key, value in whitePawnsCaptures.items():
    print(f"whitePawnMovesCaptures[bitboard(\"{key}\")] = bitboard(\"{value}\");")
"""

print("const bitboard RAYS[8][64] = {")
for dir, movesInDir in enumerate(generateCardinals()):
    print("\t{")
    for ray in movesInDir:
        print(f"\tbitboard(\"{ray}\"),")
    print("\t},")