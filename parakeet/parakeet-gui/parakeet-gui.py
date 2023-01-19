import pygame
from piece import Piece, EmptySquare
from engine import Engine
import os

real_path = os.path.realpath(__file__)
dir_path = os.path.dirname(real_path)

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

def load_pieces(board_position: list):
    board_position[get_square_index("e1")] = Piece("K", square_size)
    board_position[get_square_index("d1")] = Piece("Q", square_size)
    board_position[get_square_index("f1")] = Piece("B", square_size)
    board_position[get_square_index("c1")] = Piece("B", square_size)
    board_position[get_square_index("b1")] = Piece("N", square_size)
    board_position[get_square_index("g1")] = Piece("N", square_size)
    board_position[get_square_index("a1")] = Piece("R", square_size)
    board_position[get_square_index("h1")] = Piece("R", square_size)

    board_position[get_square_index("e8")] = Piece("k", square_size)
    board_position[get_square_index("d8")] = Piece("q", square_size)
    board_position[get_square_index("f8")] = Piece("b", square_size)
    board_position[get_square_index("c8")] = Piece("b", square_size)
    board_position[get_square_index("b8")] = Piece("n", square_size)
    board_position[get_square_index("g8")] = Piece("n", square_size)
    board_position[get_square_index("a8")] = Piece("r", square_size)
    board_position[get_square_index("h8")] = Piece("r", square_size)

    for i in range(8):
        board_position[get_square_index("abcdefgh"[i]+"7")] = Piece("p", square_size)
        board_position[get_square_index("abcdefgh"[i]+"2")] = Piece("P", square_size)

def match_position(board_position: list, eng: Engine):
    pos_string = eng.get_position()
    for i in range(64):
        if pos_string[i] == ".":
            if not board_position[i].empty:
                board_position[i] = EmptySquare()
        else:
            board_position[i] = Piece(pos_string[i], square_size)

def highlight_square(screen: pygame.Surface, square: int):
    try:
        x, y = get_coord_from_square_index(square)
    except:
        print("Warning: trying to highlight a piece that doesn't exist.")
        return
    h = pygame.Surface((square_size, square_size))
    h.set_alpha(100)
    h.fill((200,0,0))
    screen.blit(h, (x, y))

def find_a_piece(piece_type: str, board_position: list):
    for i in range(64):
        if not board_position[i].empty:
            if board_position[i].type == piece_type:
                return i
    return None

def coords_within_rect(coords: tuple[int, int], rect: tuple[int, int, int, int]) -> bool:
    return (coords[0] > rect[0] and coords[0] < rect[0]+rect[2] and coords[1] > rect[1] and coords[1] < rect[1]+rect[3])

def main():
    pygame.init()

    pygame.font.init()
    pygame.font.get_init()
    font = pygame.font.SysFont('freesandbold.ttf', 50)
    
    # load and set the logo
    logo = pygame.image.load(dir_path+"/res/parakeet-logo.png")
    pygame.display.set_icon(logo)
    pygame.display.set_caption("Parakeet Chess Engine")

    screen = pygame.display.set_mode(size)

    # Position array
    board_position = [EmptySquare() for i in range(64)]
    
    selected_index = None   # will store the index of the selected piece
                            # (None if no piece is selected)

    # initialise engine
    parakeet = Engine("'"+dir_path+"/../bin/parakeet.out'")
    parakeet.reset_board()

    # Match board position to engine
    match_position(board_position, parakeet)
    
    generated_moves = {} # will store the move generation for this position
    move_suggested = False
    check_black = False
    check_white = False

    # $play button
    play_button_coords = (board_size + 50, 50, 100, 100)
    clicked = False

    # main loop
    running = True
    while running:  #========================== MAIN LOOP ==========================#
        # Draw board
        screen.fill((20, 20, 20))
        pygame.draw.rect(screen, (255, 232, 161), (0,0,board_size,board_size))
        for i in range(8):
            for j in range(4):
                pygame.draw.rect(screen, (223, 167, 0), 
                    ((1-i%2)*square_size + j*square_size*2, i*square_size, square_size, square_size))

        # Draw $play button
        pygame.draw.rect(screen, (255, 255, 255), play_button_coords)
        if pygame.mouse.get_pressed()[0] and coords_within_rect(pygame.mouse.get_pos(), play_button_coords) and not clicked:
            parakeet.play()
            match_position(board_position, parakeet)
            clicked = True
        if not pygame.mouse.get_pressed()[0] and clicked:
            clicked = False

        #==========================================================================================
        # Piece movement with mouse
        if pygame.mouse.get_pressed()[0] and selected_index is None:
            square_clicked = get_square_index_from_coord(pygame.mouse.get_pos())
            if square_clicked is not None:
                if not board_position[square_clicked].empty:
                    selected_index = square_clicked
                    board_position[selected_index].select()

        #==========================================================================================   
        # Actually moving
        if not pygame.mouse.get_pressed()[0] and selected_index is not None: 
            # i.e. not clicking but still selected

            board_position[selected_index].unselect() # this should go here

            new_square = get_square_index_from_coord(pygame.mouse.get_pos())
            if new_square is not None and "Enter a target square" in parakeet.last_communication():
                move_communication = parakeet.make_move(new_square)
                if new_square in generated_moves[selected_index]:
                    
                    check_white = "CHECK white" in move_communication
                    check_black = "CHECK black" in move_communication
                    generated_moves = {} # moves generated no longer relevant

                    
                    match_position(board_position, parakeet)
            
            selected_index = None
            move_suggested = False

        #==========================================================================================
        # Highlight possible moves and king if in check
        if selected_index is not None:
            if not move_suggested:
                possible_moves = parakeet.suggest_move_square(selected_index)
                move_suggested = True
                if selected_index not in generated_moves.keys():
                    generated_moves.update({selected_index : possible_moves})
            
            for sq in generated_moves[selected_index]:
                highlight_square(screen, int(sq))

        if check_white:
            highlight_square(screen, find_a_piece('K', board_position))

        if check_black:
            highlight_square(screen, find_a_piece('k', board_position))
        
        #==========================================================================================
        # Draw pieces
        # -> Not selected pieces
        for i in range(64):
            chess_piece = board_position[i]
            if not chess_piece.empty:
                if not chess_piece.selected:
                    screen.blit(chess_piece.image, get_coord_from_square_index(i))                    

        # -> Selected piece
        if selected_index is not None:
            x, y = pygame.mouse.get_pos()
            screen.blit(board_position[selected_index].image, (x-square_size/2, y-square_size/2))

        #==========================================================================================
        # Ending stuff
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False

        pygame.display.flip()
    pygame.quit()
    #parakeet.close()

#==========================================================================================
if __name__=="__main__":
    main()