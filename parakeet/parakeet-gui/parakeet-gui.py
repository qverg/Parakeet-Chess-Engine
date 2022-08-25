import pygame
from piece import Piece, EmptySquare
from engine import Engine

size = width, height = 1000, 600
board_size = 600
square_size = board_size/8

def get_file_rank(square: str):
    file = "abcdefgh".index(square[0])
    rank = int(square[1])-1

    return file, rank

def get_coord(square: str):
    file, rank = get_file_rank(square)
    return file*square_size, (7-rank)*square_size

def get_coord_from_square_index(ind: int):
    return (ind%8)*square_size, (7-(ind//8))*square_size

def get_square_index(square: str):
    file, rank = get_file_rank(square)
    return rank*8 + file

def get_square_index_from_coord(coord: tuple):
    if coord[0] <= board_size:
        return int((7-coord[1]//square_size)*8 + coord[0]//square_size)
    else:
        return None

def main():
    pygame.init()
    
    # load and set the logo
    #logo = pygame.image.load("logo32x32.png")
    #pygame.display.set_icon(logo)
    pygame.display.set_caption("Parakeet Chess Engine")

    screen = pygame.display.set_mode(size)

    # Position array
    board_position = [EmptySquare() for i in range(64)]

    # Load pieces
    board_position[get_square_index("e1")] = Piece("res/white_king.png", square_size)
    board_position[get_square_index("d1")] = Piece("res/white_queen.png", square_size)
    board_position[get_square_index("f1")] = Piece("res/white_bishop.png", square_size)
    board_position[get_square_index("c1")] = Piece("res/white_bishop.png", square_size)
    board_position[get_square_index("b1")] = Piece("res/white_knight.png", square_size)
    board_position[get_square_index("g1")] = Piece("res/white_knight.png", square_size)
    board_position[get_square_index("a1")] = Piece("res/white_rook.png", square_size)
    board_position[get_square_index("h1")] = Piece("res/white_rook.png", square_size)

    board_position[get_square_index("e8")] = Piece("res/black_king.png", square_size)
    board_position[get_square_index("d8")] = Piece("res/black_queen.png", square_size)
    board_position[get_square_index("f8")] = Piece("res/black_bishop.png", square_size)
    board_position[get_square_index("c8")] = Piece("res/black_bishop.png", square_size)
    board_position[get_square_index("b8")] = Piece("res/black_knight.png", square_size)
    board_position[get_square_index("g8")] = Piece("res/black_knight.png", square_size)
    board_position[get_square_index("a8")] = Piece("res/black_rook.png", square_size)
    board_position[get_square_index("h8")] = Piece("res/black_rook.png", square_size)

    for i in range(8):
        board_position[get_square_index("abcdefgh"[i]+"7")] = Piece("res/black_pawn.png", square_size)
        board_position[get_square_index("abcdefgh"[i]+"2")] = Piece("res/white_pawn.png", square_size)

    
    selected_index = None

    # initialise engine
    parakeet = Engine("../src/parakeet")
    parakeet.reset()

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

        # Piece movement with mouse
        if pygame.mouse.get_pressed()[0] and selected_index is None:
            square_clicked = get_square_index_from_coord(pygame.mouse.get_pos())
            if square_clicked is not None:
                if not board_position[square_clicked].empty:
                    selected_index = square_clicked
                    board_position[selected_index].select()
        if not pygame.mouse.get_pressed()[0] and selected_index is not None:
            board_position[selected_index].unselect()

            new_square = get_square_index_from_coord(pygame.mouse.get_pos())
            if new_square is not None:
                chess_piece = board_position.pop(selected_index)
                board_position.insert(selected_index, EmptySquare())
                board_position.pop(new_square)
                board_position.insert(new_square, chess_piece)
                
            selected_index = None

        # Draw pieces
        for i in range(64):
            chess_piece = board_position[i]
            if not chess_piece.empty:
                if not chess_piece.selected:
                    screen.blit(chess_piece.image, get_coord_from_square_index(i))
                else:
                    x, y = pygame.mouse.get_pos()
                    screen.blit(chess_piece.image, (x-square_size/2, y-square_size/2))

        # Ending stuff
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False

        pygame.display.flip()

    parakeet.close()

if __name__=="__main__":
    main()