import socket
import pygame
from pygame.locals import *
from pygame import gfxdraw
import time
import base64
import zlib
import threading
import math
import sys
import array

## Display Settings ##
font_color = (255, 255, 255)
bg_color = (159, 182, 205)
image_size = (80, 60)
screen_size = (1280, 960)
font = None

## Network Settings ##
robot_ip = "192.168.43.99"
#robot_ip = "localhost"
control_port = 80
view_port = 81
img_buffer_size = 65536

# Global Variables #
run_event = threading.Event()
display_text = None
frame = None
overlay = pygame.Surface(screen_size, pygame.SRCALPHA, 32)
CMD = ' '

millis = lambda: int(round(time.time() * 1000))
prev_millis = millis()

pygame.init()

tri_height = 100
tri_width = 30
tri_color = (255,0,0,150)
triangle = pygame.Surface((tri_width, tri_height), pygame.SRCALPHA, 32)

pygame.gfxdraw.filled_trigon(triangle, 0, tri_height, tri_width, tri_height, tri_width//2, 0, tri_color)

overlay_off = (-50, 400)
overlay.blit(triangle, (250 + overlay_off[0], 200 + overlay_off[1]))
overlay.blit(pygame.transform.rotate(triangle, 45), (145 + overlay_off[0], 240 + overlay_off[1]))
overlay.blit(pygame.transform.rotate(triangle, 90), (110 + overlay_off[0], 345 + overlay_off[1]))
overlay.blit(pygame.transform.rotate(triangle, 135), (145 + overlay_off[0], 390 + overlay_off[1]))
overlay.blit(pygame.transform.rotate(triangle, 180), (250 + overlay_off[0], 420 + overlay_off[1]))
overlay.blit(pygame.transform.rotate(triangle, 225), (300 + overlay_off[0], 390 + overlay_off[1]))
overlay.blit(pygame.transform.rotate(triangle, 270), (333 + overlay_off[0], 345 + overlay_off[1]))
overlay.blit(pygame.transform.rotate(triangle, 315), (300 + overlay_off[0], 240 + overlay_off[1]))

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

scaleframe = pygame.transform.scale

def update():
    global display_text
    global frame
    global CMD
    screen.fill(bg_color)
    if frame is not None:
        frame = scaleframe(frame, screen_size)
        screen.blit(frame, (0, 0))
    screen.blit(overlay, (0, 0))
    blitDirection(screen, CMD)
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
            return check_angle(keys)
    else:
        return check_angle(keys)

def check_angle(keys):
    if (keys[K_o]):
        return "O"
    elif (keys[K_l]):
        return "L"
    elif (keys[K_p]):
        return "P"
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

rotateimage = pygame.transform.rotate

def blitDirection(screen, cmd):
    cmd = cmd.lower();
    if (cmd == ' ' ):
        return
    if (cmd == 'w'):
        screen.blit(triangle, (250 + overlay_off[0], 200 + overlay_off[1]))
    elif (cmd == 'b'):
        screen.blit(rotateimage(triangle, 45), (145 + overlay_off[0], 240 + overlay_off[1]))
    elif (cmd == 'a'):
        screen.blit(rotateimage(triangle, 90), (110 + overlay_off[0], 345 + overlay_off[1]))
    elif (cmd == 'f'):
        screen.blit(rotateimage(triangle, 135), (145 + overlay_off[0], 390 + overlay_off[1]))
    elif (cmd == 's'):
        screen.blit(rotateimage(triangle, 180), (250 + overlay_off[0], 420 + overlay_off[1]))
    elif (cmd == 'g'):
        screen.blit(rotateimage(triangle, 225), (300 + overlay_off[0], 390 + overlay_off[1]))
    elif (cmd == 'd'):
        screen.blit(rotateimage(triangle, 270), (333 + overlay_off[0], 345 + overlay_off[1]))
    elif (cmd == 'c'):
        screen.blit(rotateimage(triangle, 315), (300 + overlay_off[0], 240 + overlay_off[1]))

pumpevents = pygame.event.pump
getkeys = pygame.key.get_pressed

## Main functions##
def run_controller(hostname, port, interrupt):
    global display_text
    global CMD
    #print("Started.")
    sck = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sck.connect((hostname, port))
    prev_key = ""
    while interrupt.is_set():
        ## Get Keys Pressed
        pumpevents()
        keys = getkeys()

        ## Switch case for cmd
        ## Reserved Keys are ['x', ' ', 'esc']
        cmd = check(keys)
            
        display_text = "Control with WASD. Press Q to exit. CMD = {}".format(cmd.upper())

        ## Update only if key has changed
        if (cmd is not prev_key):
            sck.send(cmd.lower().encode())
            answer = sck.recv(1024)
            CMD = cmd
            print("Send " + cmd)

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
    global img_buffer_size
    sck = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sck.connect((hostname, port))    
    sck.settimeout(100)
    message = None
    valid = False
    while(interrupt.is_set()):
    #for i in range(20):
        if (valid):
            buffer = message
            t = threading.Thread(target=decodeImg, args=(buffer,))
            #profile("Worker thread started...")
            t.start()
        answer = b" "
        answers = []
        message = b""
        sck.send(b"READY")
        #profile("Sent READY signal")
        answer = sck.recv(4)
        num = int(answer[0] << 24) + int(answer[1] << 16) + int(answer[2] << 8) + int(answer[3])
        print("Received num: " + str(num))
        if (num >= 60000):
            print("Invalid Frame")
            num = 60000
            
        packet_count = math.ceil(num/img_buffer_size)
        try:
            answers = [sck.recv(img_buffer_size) for i in range(packet_count)]
                #message += sck.recv(img_buffer_size)
                #answers.append(sck.recv(img_buffer_size))
        except socket.timeout:
            pass
        message = b"".join(answers)
        #message = array.array('B', answers).tostring()
        #profile("Received message")
        
        #print("###########################################")
        #print("##### Received! message (len=", len(message),") #####")
        #print("###########################################")
        #print("Waiting for thread join...")
        #if (valid):
        #    t.join()
        #fps()
        valid = (len(message) == num)
    sck.close()
    print("run_image exited")

decompress = zlib.decompress
decode = base64.b64decode
toframe = pygame.image.fromstring

lock = threading.Lock()

def decodeImg(raw):
    if (raw is None or raw == ""):
        return
    global frame
    try:
        #print("Message length     :", len(raw))
        raw = decode(raw)
        #print("Decoded length     :", len(raw))
        raw = decompress(raw)
        #print("Decompressed length:", len(raw))
        #raw = raw[::-1]         #bgr to rgb
        if lock.acquire(False):
            try:
                frame = toframe(raw, image_size, 'RGB')
                #frame = pygame.transform.flip(frame, True, True)
                if (frame is None):
                    print("ERR None frame")
            finally:
                lock.release()
    except zlib.error:
        print("Decompression Error")
        return

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
        time.sleep(0.1)
except KeyboardInterrupt:
    run_event.clear()
    print("Threads exited succussfully")
finally:
    if (t1.is_alive()):
        t1.join()
    if (t2.is_alive()):
        t2.join()
    pygame.quit()

print("Finished Execution")


