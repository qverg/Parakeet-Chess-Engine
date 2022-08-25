import pygame

pieceTypes = {
    "K" : pygame.image.load("res/white_king.png"),
    "Q" : pygame.image.load("res/white_queen.png"),
    "B" : pygame.image.load("res/white_bishop.png"),
    "N" : pygame.image.load("res/white_knight.png"),
    "R" : pygame.image.load("res/white_rook.png"),
    "P" : pygame.image.load("res/white_pawn.png"),
    "k" : pygame.image.load("res/black_king.png"),
    "q" : pygame.image.load("res/black_queen.png"),
    "b" : pygame.image.load("res/black_bishop.png"),
    "n" : pygame.image.load("res/black_knight.png"),
    "r" : pygame.image.load("res/black_rook.png"),
    "p" : pygame.image.load("res/black_pawn.png")
}

class Piece:
    def __init__(self, piece_type: str, square_size: int):
        self.image = pieceTypes[piece_type]
        self.image = pygame.transform.scale(self.image.convert_alpha(), (square_size, square_size))
        self.empty = False
        self.selected = False

    def select(self):
        self.selected = True

    def unselect(self):
        self.selected = False

class EmptySquare:
    def __init__(self):
        self.empty = True



    