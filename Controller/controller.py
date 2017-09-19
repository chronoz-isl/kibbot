import socket
import pygame
import time
import base64
import zlib
from pygame.locals import *
import threading
import math
import sys


## Display Settings ##
font_color = (255, 255, 255)
bg_color = (159, 182, 205)
image_size = (640, 480)
screen_size = image_size
font = None

## Network Settings ##
robot_ip = "localhost"
control_port = 80
view_port = 81

# Global Variables #

run_event = threading.Event()
display_text = None
frame = None

millis = lambda: int(round(time.time() * 1000))
prev_millis = millis()

def profile(iden = ""):
    global prev_millis
    now = millis()
    if (iden is not None and iden != None):
        iden = iden + ":"
    print(iden, now - prev_millis, "ms")
    prev_millis = now

def fps():
    global prev_millis
    now = millis()
    _fps = round(1000 / (now - prev_millis), 2)
    print(_fps, "FPS")
    prev_millis = now
    return _fps

def update():
    global display_text
    global frame
    if frame is not None:
        screen.fill(bg_color)
        screen.blit(frame, (0, 0))
    else:
        screen.fill(bg_color)
    if display_text is not None:
        global font
        text = font.render(display_text, True, font_color)
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

## Main functions##
def run_controller(hostname, port, interrupt):
    global display_text
    #print("Started.")
    sck = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sck.connect((hostname, port))

    prev_key = ""
    while interrupt.is_set():
        ## Get Keys Pressed
        pygame.event.pump()
        keys = pygame.key.get_pressed()

        ## Switch case for cmd
        ## Reserved Keys are ['x', ' ', 'esc']
        cmd = check(keys)
            
        display_text = "Control with WASD. Press Q to exit. CMD = {}".format(cmd.upper())

        ## Update only if key has changed
        if (cmd is not prev_key):
            sck.send(cmd.lower().encode())
            answer = sck.recv(1024)
            #print(answer)

        ## exit if ESC pressed
        if (cmd.upper() == "Q"):
            interrupt.clear()
            break

        ## Update prev_key
        prev_key = cmd
    sck.close()
    print("run_controlled exited")

def run_image(hostname, port, interrupt):
    global frame
    sck = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sck.connect((hostname, port))
    sck.settimeout(10)
    message = None
    while(interrupt.is_set()):
        buffer = message
        t = threading.Thread(target=decodeImg, args=(buffer,))
        t.start()
        answer = b" "
        message = b""
        sck.send(b"READY")
        #print("Sent READY signal")
        answer = sck.recv(4)
        num = int(answer[0] << 24) + int(answer[1] << 16) + int(answer[2] << 8) + int(answer[3])
        
        try:
            for i in range(math.ceil(num/1024)):
                #print("receiving...")
                answer = sck.recv(1024) # row size
                message += answer
                if answer == b"":
                    #print("exited via break")
                    break
        except socket.timeout:
            pass

        t.join()
        #print("Updated")
    sck.close()
    print("run_image exited")

decompress = zlib.decompress
decode = base64.b64decode
toframe = pygame.image.fromstring

def decodeImg(raw):
    if (raw is None or raw == ""):
        return
    global frame
    #print("Message length:", len(message))
    raw = decode(raw)
    #print("Decoded length:", len(message))
    raw = decompress(raw)
    frame = toframe(raw, image_size, 'RGB')
    if (frame is None):
        print("ERR None frame")



pygame.init()
screen = pygame.display.set_mode(screen_size)
pygame.display.set_caption('Robot Controller')
screen.fill(bg_color)
pygame.display.update()

font = pygame.font.Font(None, 24)

run_event.set()
t1 = threading.Thread(target = run_controller, args = (robot_ip, control_port, run_event))
t2 = threading.Thread(target = run_image, args = (robot_ip, view_port, run_event))

t1.start()
t2.start()
try:
    while run_event.is_set():
        update()
        for event in pygame.event.get():
            if event.type==pygame.QUIT:
                run_event.clear()
                print("Exited succussfully")
                break
            elif event.type == MOUSEBUTTONUP:
                pass
        else:
            continue
        break
        time.sleep(0.01)
except KeyboardInterrupt:
    print("Threads exited succussfully")
finally:
    if (t1.is_alive()):
        t1.join()
    if (t2.is_alive()):
        t2.join()
    pygame.quit()

print("Finished Execution")


