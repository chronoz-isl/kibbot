import socket
import pygame
from pygame.locals import *

font_color = (255, 255, 255)
bg_color = (159, 182, 205)
screen_size = (640, 480)

## Initialize pygame ##
pygame.init()
screen = pygame.display.set_mode(screen_size)
pygame.display.set_caption('Robot Controller')
screen.fill(bg_color)
pygame.display.update()

font = pygame.font.Font(None, 24)

def show(s):
    screen.fill((159, 182, 205))
    text = font.render(s, True, font_color, bg_color)
    textRect = text.get_rect()
    textRect.centerx = screen.get_rect().centerx
    textRect.centery = screen.get_rect().centery

    screen.blit(text, textRect)
    pygame.display.update()


def check(keys):
    if (keys[K_ESCAPE] or keys[K_q] ):
        return "Q" # quit
    elif (keys[K_h]):
        return "H" # help
    else:
        return check_alt(keys)

def check_alt(keys):
    if (keys[K_RALT] or keys[K_LALT]):
        if (keys[K_0]):
            return "0"   # alt-0
        elif (keys[K_1]):
            return "1"   # alt-1
        elif (keys[K_2]):
            return "2"   # alt-2
        elif (keys[K_3]):
            return "3"   # alt-3
        elif (keys[K_4]):
            return "4"   # alt-4
        else:
            return check_direction(keys)
    else:
        return check_direction(keys)

def check_direction(keys):
    if (keys[K_w] and keys[K_a]):
        cmd = "B" # left-up
    elif (keys[K_w] and keys[K_s]):
        cmd = " " # stop
    elif (keys[K_w] and keys[K_d]):
        cmd = "C" # right-up
    elif (keys[K_a] and keys[K_d]):
        cmd = " " # stop
    elif (keys[K_a] and keys[K_s]):
        cmd = "F" # left-down
    elif (keys[K_s] and keys[K_d]):
        cmd = "G" # right-down
    
    elif (keys[K_w]):
        cmd = "W" # up
    elif (keys[K_a]):
        cmd = "A" # left
    elif (keys[K_s]):
        cmd = "S" # down
    elif (keys[K_d]):
        cmd = "D" # right
    else:
        cmd = " " # stop (default)

    return cmd

## Main function ##
def run(hostname = "localhost", port = 80):
    print("Started.")
    sck = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sck.connect((hostname, port))

    prev_key = ""
    while 1:
        ## Get Keys Pressed
        pygame.event.pump()
        keys = pygame.key.get_pressed()

        ## Switch case for cmd
        ## Reserved Keys are ['x', ' ', 'esc']
        cmd = check(keys)
            
        show("Control with WASD. Press Q to exit. CMD = {}".format(cmd.upper()))

        ## Update only if key has changed
        if (cmd is not prev_key):
            sck.send(cmd.lower().encode())
            answer = sck.recv(1024)
            print(answer)

        ## exit if ESC pressed
        if (cmd.upper() == "Q"):
            print("QUIT")
            break

        ## Update prev_key
        prev_key = cmd
    sck.close()
    pygame.quit()
    print("Finished.")

run()

