import os
import sys

# I used the code google's search AI, Gemini, suggested to try to get the environment for p5 to work. 
# I don't think I need this anymore since it works outside the environment with this, but whatever
# 1. Provide the path directly to the folder containing 'glfw3.dll'
glfw_dir = r"C:\Users\gooch\OneDrive\Documents\GitHub\inventing-interactive-systems\inventsys-2026f-main\py-apps\p5py\glfw-3.5.1.bin.WIN64\lib-mingw-w64"

# 2. Tell Python 3.12 to search this folder for DLL dependencies
if sys.platform == "win32":
    os.add_dll_directory(glfw_dir)
    # Fallback backup for older packages/environments
    os.environ["PATH"] = glfw_dir + os.pathsep + os.environ.get("PATH", "")

# 3. Now import p5 safely

from p5 import *

# y = 100 , this would need to be called as global y in all later instances, so we make a class instead
class AppParams:
    y = 100
    x =200

PARAMS = AppParams() # declare object globally

def setup():
    size(300,300) # size must be the first statement
    background(0,0,0) #fills the whole screen with a color
    stroke(255) #sets the drawing color to white
    PARAMS.y = 100
    print("Setup is complete.")




#works kind of like arduino ide, the rest just loops(?)
    #  The statements in draw() are executed until the
    # program is stopped. Each statement is executed in
    # sequence and after the last line is read, the first
    # line is executed again.

def draw():
    background(255,100,100)
    fill(255,255,255)
    
    # global y
    # print("hello")
    # x, y, w, h
    rect((PARAMS.x,PARAMS.y), 100, 100)
    # rect((50,PARAMS.y), rand(20,100), rand(20,100))
    PARAMS.y+=2 #animates it downward
    PARAMS.x+=3

    if(mouse_is_pressed): #makes a circle appear around where the cursor clicked, for the duration of the click
        fill(0,255,200)
        circle((mouse_x, mouse_y), 10)

#Bring back square to top
    if(PARAMS.y>=height):
        PARAMS.y=0
#Bring back to left
    if(PARAMS.x>=width):
        PARAMS.x=0

def key_pressed():
    if key == 'b':

        print("b was pressed")

#Run the P5 application
if __name__ == '__main__':
    print("Starting application...")
    run(renderer="vispy")
