import pygame
from piece import Piece, EmptySquare

size = width, height = 1000, 600
board_size = 600
square_size = board_size/8

def get_file_rank(square: str):
    file = "abcdefgh".index(square[0])
    rank = 8-int(square[1])

    return file, rank

def get_coord(square: str):
    file, rank = get_file_rank(square)
    return file*square_size, rank*square_size

def get_coord_from_square_index(ind: int):
    return (ind%8)*square_size, (ind//8)*square_size

def get_square_index(square: str):
    file, rank = get_file_rank(square)
    return rank*8 + file

def main():
    pygame.init()
    
    # load and set the logo
    #logo = pygame.image.load("logo32x32.png")
    #pygame.display.set_icon(logo)
    pygame.display.set_caption("Parakeet Chess Engine")

    screen = pygame.display.set_mode(size)

    # Position array
    board_position = [EmptySquare() for i in range(64)]
    # Load of the pieces
    board_position[get_square_index("e1")] = Piece("res/white_king.png", square_size)

    running = True
    
    # main loop
    while running:
        # Draw board
        screen.fill((255, 255, 255))
        pygame.draw.rect(screen, (255, 232, 161), (0,0,board_size,board_size))
        for i in range(8):
            for j in range(4):
                pygame.draw.rect(screen, (223, 167, 0), 
                    ((1-i%2)*square_size + j*square_size*2, i*square_size, square_size, square_size))

        # Draw pieces
        for i in range(64):
            piece = board_position[i]
            if not piece.empty:
                screen.blit(piece.image, get_coord_from_square_index(i))


        # Ending stuff
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False

        pygame.display.flip()

if __name__=="__main__":
    main()