import pygame

class Piece:
    def __init__(self, img_name: str, square_size: int):
        self.image = pygame.image.load(img_name)
        self.image = pygame.transform.scale(self.image, (square_size, square_size))
        self.empty = False

class EmptySquare:
    def __init__(self):
        self.empty = True



    