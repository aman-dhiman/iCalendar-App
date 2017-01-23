#!/usr/bin/python3

from tkinter import *
from tkinter.filedialog import askopenfilename
import Cal
import os

done = 0
filename = ""
filename2 = ""
arg = ""
result = []
comps = []
comp = []
todo = []
dtodo = []
nl = "\n"
rbv = [0]
dates = []
filt = [0]


def callback():
    done = 1

def disablestate():
    fileMenu.entryconfigure(1,state=DISABLED)
    fileMenu.entryconfigure(2,state=DISABLED)
    fileMenu.entryconfigure(3,state=DISABLED)
    fileMenu.entryconfigure(4,state=DISABLED)
    todoMenu.entryconfigure(0,state=DISABLED)
    todoMenu.entryconfigure(1,state=DISABLED)

def enablestate():
    fileMenu.entryconfigure(1,state=NORMAL)
    fileMenu.entryconfigure(2,state=NORMAL)
    fileMenu.entryconfigure(3,state=NORMAL)
    fileMenu.entryconfigure(4,state=NORMAL)

def aboutpopup():
    top = Toplevel()
    top.geometry('300x150')
    top.title("About...")

    msg = Message(top, text="Application: xcal\nAuthor: Aman Dhiman\n"
          "This app is compatible with iCalendar V2.0.", width=250)
    msg.pack(fill=BOTH,expand=YES,pady=20)

    button = Button(top, text="OK", command=top.destroy)
    button.pack(pady=10)
    top.grab_set()
    top.transient(master=root)
    root.wait_window(top)

def openfile():
    comps.clear()
    comp.clear()
    todo.clear()
    dtodo.clear()
    fvplist.delete(0,END)
    if len(result) != 0:
        success = Cal.freeFile (result[0])
    result.clear()
    filename = askopenfilename()
    if len(filename) == 0:
        return filename
    open("error.txt", "w").close()
    open("output.txt", "w").close()
    arg = "./caltool -info <" + filename + " > output.txt 2> error.txt"
    os.system(arg)
    ferr = open("error.txt", "r")
    fout = open("output.txt", "r")
    string = ferr.read() + fout.read()
    ferr.close()
    fout.close()
    lptext.insert (END, nl)
    lptext.insert (END, string)
    success = Cal.readFile (filename, result)
    index = 0
    for x in result[1]:
        comps.append(index)
        if x[0] == "VTODO":
           todo.append(index)
        fvplist.insert(END, "{0:5d} {1:15s} {2:10d} {3:10d} {4:10s}".format(index+1, x[0], x[1], x[2], x[3]))
        index += 1
    root.title("xcal-"+filename)
    fvplist.yview(END)
    enablestate()
    return filename

def exevent():
    open("error.txt", "w").close()
    open("output.txt", "w").close()
    open("input.txt", "w").close()
    success = Cal.writeFile ("input.txt", result[0], comps)
    arg = "./caltool -extract e < input.txt > output.txt 2> error.txt"
    os.system(arg)
    ferr = open("error.txt", "r")
    fout = open("output.txt", "r")
    string = ferr.read() + fout.read()
    ferr.close()
    fout.close()
    lptext.insert (END, nl)
    lptext.insert (END, string)
    lptext.yview(END)

def exprop():
    open("error.txt", "w").close()
    open("output.txt", "w").close()
    open("input.txt", "w").close()
    success = Cal.writeFile ("input.txt", result[0], comps)
    arg = "./caltool -extract x < input.txt > output.txt 2> error.txt"
    os.system(arg)
    ferr = open("error.txt", "r")
    fout = open("output.txt", "r")
    string = ferr.read() + fout.read()
    ferr.close()
    fout.close()
    lptext.insert (END, nl)
    lptext.insert (END, string)
    lptext.yview(END)

def combine():
    filename2 = askopenfilename()
    if len(filename2) == 0:
        return done
    open("error.txt", "w").close()
    open("output.txt", "w").close()
    open("input.txt", "w").close()
    success = Cal.writeFile ("input.txt", result[0], comps)
    arg = "./caltool -combine " + filename2 + " < input.txt > output.txt 2> error.txt"
    os.system(arg)
    success = Cal.freeFile (result[0])
    result.clear()
    comps.clear()
    comp.clear()
    todo.clear()
    dtodo.clear()
    success = Cal.readFile ("output.txt", result)
    fvplist.delete(0,END)
    index = 0
    for x in result[1]:
        comps.append(index)
        if x[0] == "VTODO":
           todo.append(index)
        fvplist.insert(END, "{0:5d} {1:25s} {2:5d} {3:5d} {4:5s}".format(index+1, x[0], x[1], x[2], x[3]))
        index += 1
    root.title("xcal-"+filename)
    fvplist.yview(END)

def filter():
#    top = Toplevel(master = root)
    top.deiconify()
    top.geometry('180x350')
    top.title("Filter")
    top.grab_set()
    bevent = Radiobutton(top, text="Event", variable=rb, value=1, command=getrb)
    btodo = Radiobutton(top, text="To-Do", variable=rb, value=2, command=getrb)
    bevent.pack(anchor = W, pady=10)
    bevent.deselect()
    btodo.pack(anchor=W, pady=10)
    btodo.deselect()
    lfrom = Label(top, text="From")
    lfrom.pack(anchor = W, pady=10)
    efrom = Entry(top, textvariable=fd)
    efrom.pack(anchor = W, pady=10)
    efrom.delete(0, END)
    lto = Label(top, text="To")
    lto.pack(anchor=W, pady=10)
    eto = Entry(top, textvariable=td)
    eto.pack(anchor = W, pady=10)
    eto.delete(0,END)
    filter = Button(top, text="Filter", command=getdate)
    filter.pack(pady=10, anchor = W)
    cancel = Button(top, text="Cancel", command=top.withdraw)
    cancel.pack(pady=10, anchor = W)

def showselect():
    index = fvplist.index(ACTIVE)
    index = int(index)
    comp.clear()
    comp.append(index)
    pcal = result[0]
    open("error.txt", "w").close()
    open("output.txt", "w").close()
    success = Cal.writeFile ("output.txt", pcal, comp)
    ferr = open("error.txt", "r")
    fout = open("output.txt", "r")
    string = ferr.read() + fout.read()
    ferr.close()
    fout.close()
    lptext.insert (END, nl)
    lptext.insert (END, string)
    lptext.yview(END)

def getrb():
    rbv[0] = (rb.get())

def getdate():
    dates.clear()
    dates.append(fd.get())
    dates.append(td.get())
    fd.set("")
    td.set("")
    top.withdraw()
    arg = "./caltool -filter "
    if rbv[0] == 1:
        arg += "e "
    elif rbv[0] == 2:
        arg += "t "
    if len(dates[0]) != 0:
        arg += "from \"" + dates[0] + "\" "
    if len(dates[1]) != 0:
        arg += "to \"" + dates[1] + "\" "
    open("error.txt", "w").close()
    open("output.txt", "w").close()
    success = Cal.writeFile ("input.txt", result[0], comps)
    arg +=  "< input.txt > output.txt 2> error.txt"
    os.system(arg)
    success = Cal.freeFile (result[0])
    result.clear()
    comps.clear()
    comp.clear()
    todo.clear()
    dtodo.clear()
    success = Cal.readFile ("output.txt", result)
    fvplist.delete(0,END)
    index = 0
    for x in result[1]:
        comps.append(index)
        if x[0] == "VTODO":
           todo.append(index)
        fvplist.insert(END, "{0:5d} {1:25s} {2:5d} {3:5d} {4:5s}".format(index+1, x[0], x[1], x[2], x[3]))
        index += 1
    root.title("xcal-"+filename)
    fvplist.yview(END)
    root.grab_set()

def save():
    if len(result) != 0:
        success = Cal.writeFile (filename, result[0], comps)
        if success == "OK":
            lptext.insert (END, nl)
            lptext.insert (END, "File Saved Sucessfully")
            lptext.yview(END)

def saveas():
    filename = askopenfilename()
    if len(filename) == 0:
        return filename
    if len(result) != 0:
        success = Cal.writeFile (filename, result[0], comps)
        if success == "OK":
            lptext.insert (END, nl)
            lptext.insert (END, "File Saved Sucessfully")
            lptext.yview(END)
            root.title("xcal-"+filename)
    return filename

def exitapp():
    if len(result) != 0:
        Cal.freeFile(result[0])
    root.destroy()
    result.clear()

def setdm():
    datemask = askopenfilename()
    if len(datemask) != 0:
        os.system("export DATEMSK=" + datemask)
        lptext.insert (END, nl)
        lptext.insert (END, "DATEMASK Set!")
        lptext.yview(END)

def clear():
    lptext.delete('1.0',END)

def closing():
    root.grab_set()
    top.withdraw()

root = Tk()
root.protocol("WM_DELETE_WINDOW", exitapp)

top = Toplevel()
top.protocol("WM_DELETE_WINDOW", closing)
top.withdraw()
rb = IntVar()
fd = StringVar()
td = StringVar()

mainMenu = Menu(root)
root.config(menu=mainMenu, bg="LightSteelBlue1")

fileMenu = Menu(mainMenu, tearoff=0)
mainMenu.add_cascade(label="File", menu=fileMenu)
fileMenu.add_command(label="Open...", command=openfile)
fileMenu.add_command(label="Save", command=save)
fileMenu.add_command(label="Save as...", command=saveas)
fileMenu.add_command(label="Combine...", command=combine)
fileMenu.add_command(label="Filter...", command=filter)
fileMenu.add_separator()
fileMenu.add_command(label="Exit", command=exitapp)

todoMenu = Menu(mainMenu, tearoff=0)
mainMenu.add_cascade(label="ToDo", menu=todoMenu);
todoMenu.add_command(label="To-do List...", command=callback)
todoMenu.add_command(label="Undo...", command=callback)

helpMenu = Menu(mainMenu, tearoff=0)
mainMenu.add_cascade(label="Help", menu=helpMenu)
helpMenu.add_command(label="Date Mask...", command=setdm)
helpMenu.add_command(label="About xcal...", command=aboutpopup)

complabel = Label(root, text="Components:", bg="LightSteelBlue1")
complabel.pack(side=TOP, padx = 20, pady=10)

fvp = Frame(root, width=800, height=500, bd=0, bg="LightSteelBlue1")
fvpar = Frame(fvp, width=600, height=500)
fvpscr = Scrollbar(fvpar)
fvplist = Listbox(fvpar, width=100, height = 8,
          yscrollcommand=fvpscr.set,borderwidth=1, highlightthickness=0)
fvpscr.config(command=fvplist.yview)
fvpbut = Frame(fvp, width=200, height=500,borderwidth=0, bg="LightSteelBlue1")
show = Button (fvpbut, text="Show Selected", command=showselect)
exevent = Button (fvpbut, text="Extract Events", command=exevent)
exprop = Button (fvpbut, text="Extract X-Props", command=exprop)
show.pack(side=TOP,pady=40,padx=30)
exevent.pack(side=TOP,pady=40,padx=30)
exprop.pack(side=TOP,pady=40,padx=30)
fvpbut.pack(side=RIGHT)
fvpscr.pack(side=RIGHT, fill=Y)
fvplist.pack(side=LEFT, expand=1, fill=BOTH)
fvpar.pack(side=LEFT,padx=20,pady=20, expand=1, fill=BOTH)
fvp.pack(side=TOP, expand=1, fill=BOTH)

loglabel = Label(root, text="Log:", bg="LightSteelBlue1")
loglabel.pack(side=TOP, padx = 20, pady=10)

lp = Frame(root, width=800, height=250, bd=0, bg="LightSteelBlue1")
lpar = Frame(lp, width=600, height=250)
lpscr = Scrollbar(lpar)
lptext = Text(lpar, width=100, height = 15, wrap="word",
         yscrollcommand=lpscr.set,borderwidth=1, highlightthickness=0)
lpscr.config(command=lptext.yview)
lpbut = Frame(lp, width=200, height=250,borderwidth=0, bg="LightSteelBlue1")
clear = Button (lpbut, text="Clear",width=12, command=clear)
clear.pack(side=TOP,pady=40,padx=30)
lpbut.pack(side=RIGHT)
lpscr.pack(side=RIGHT, fill=Y)
lptext.pack(side=LEFT, expand=1,fill=X)
lpar.pack(side=LEFT,padx=20,pady=20,expand=1, fill=X)
lp.pack(side=TOP, fill=X)

root.geometry('800x800')

disablestate()

root.mainloop()

if len(result) != 0:
    Cal.freeFile(result[0])
result.clear()
