import pygame

def main():
    pygame.init()
    
    # load and set the logo
    #logo = pygame.image.load("logo32x32.png")
    #pygame.display.set_icon(logo)
    pygame.display.set_caption("Parakeet Chess Engine")

    size = width, height = 1000, 600
    board_width = 600
    screen = pygame.display.set_mode(size)

    running = True
    
    # main loop
    while running:
        screen.fill((255, 255, 255))
        pygame.draw.rect(screen, (255, 232, 161), (0,0,board_width,board_width))
        for i in range(8):
            for j in range(4):
                pygame.draw.rect(screen, (223, 167, 0), 
                    ((1-i%2)*board_width/8 + j*board_width/4, i*board_width/8, board_width/8, board_width/8))

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False

        pygame.display.flip()

if __name__=="__main__":
    main()