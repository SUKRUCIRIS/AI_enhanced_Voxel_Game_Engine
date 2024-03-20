from src_py import depins
import os
import subprocess

depins.install_dependencies()

from src_py import genhm
from tkinter import *


def intvalfunc(P):
    if str.isdecimal(P) or P == "":
        return True
    else:
        return False


def floatvalfunc(P):
    if P == "":
        return True
    try:
        x = float(P)
        return True
    except ValueError:
        return False


window = Tk()
window.geometry("500x600")
window.config(bg="#26242f")

window.title("AI enhanced voxel game engine simple settings menu")

intregister = window.register(intvalfunc)
floatregister = window.register(floatvalfunc)

lbl = Label(window, text="Screen width", bg="#26242f", fg="white")
lbl.grid(column=0, row=0)
Screenwidth = Entry(
    window, width=20, validate="all", validatecommand=(intregister, "%P")
)
Screenwidth.insert(0, "0")
Screenwidth.grid(column=1, row=0)
lbl = Label(window, text="Enter 0 for default size", bg="#26242f", fg="white")
lbl.grid(column=2, row=0)

lbl = Label(window, text="Screen height", bg="#26242f", fg="white")
lbl.grid(column=0, row=1)
Screenheight = Entry(
    window, width=20, validate="all", validatecommand=(intregister, "%P")
)
Screenheight.insert(0, "0")
Screenheight.grid(column=1, row=1)
lbl = Label(window, text="Enter 0 for default size", bg="#26242f", fg="white")
lbl.grid(column=2, row=1)

lbl = Label(window, text="Sea level", bg="#26242f", fg="white")
lbl.grid(column=0, row=2)
Sealevel = Entry(
    window, width=20, validate="all", validatecommand=(floatregister, "%P")
)
Sealevel.insert(0, "5.3")
Sealevel.grid(column=1, row=2)

lbl = Label(window, text="Chunk size", bg="#26242f", fg="white")
lbl.grid(column=0, row=3)
Chunksize = Entry(window, width=20, validate="all", validatecommand=(intregister, "%P"))
Chunksize.insert(0, "16")
Chunksize.grid(column=1, row=3)

lbl = Label(window, text="Chunk range", bg="#26242f", fg="white")
lbl.grid(column=0, row=4)
Chunkrange = Entry(
    window, width=20, validate="all", validatecommand=(intregister, "%P")
)
Chunkrange.insert(0, "32")
Chunkrange.grid(column=1, row=4)

lbl = Label(window, text="World size x", bg="#26242f", fg="white")
lbl.grid(column=0, row=5)
Worldsizex = Entry(
    window, width=20, validate="all", validatecommand=(intregister, "%P")
)
Worldsizex.insert(0, "2048")
Worldsizex.grid(column=1, row=5)

lbl = Label(window, text="World size z", bg="#26242f", fg="white")
lbl.grid(column=0, row=6)
Worldsizez = Entry(
    window, width=20, validate="all", validatecommand=(intregister, "%P")
)
Worldsizez.insert(0, "2048")
Worldsizez.grid(column=1, row=6)


lblb = Label(window, text="", bg="#26242f", fg="white")
lblb.grid(column=2, row=8)


def clicked():
    lblb.configure(text="Compiling...")
    try:
        os.mkdir("build")
        print("Directory 'build' created successfully")
    except FileExistsError:
        print("Directory 'build' already exists")
    os.chdir("build")
    subprocess.run("cmake ..", shell=True)
    lblb.configure(text="Compiled")


btn = Button(window, text="Compile", command=clicked)

btn.grid(column=2, row=7)

window.mainloop()
