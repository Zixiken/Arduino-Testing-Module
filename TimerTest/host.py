import serial
import threading
import time
from tkinter import *

def sendCommands():
    senseCommandA = 0
    senseCommandB = 0
    maskCommand = 0

    global int2State
    global int3State
    global int4State
    global int5State
    global int2AbsTime
    global int3AbsTime
    global int4AbsTime
    global int5AbsTime
    int2State = []
    int3State = []
    int4State = []
    int5State = []
    int2AbsTime = []
    int3AbsTime = []
    int4AbsTime = []
    int5AbsTime = []

    if not ser.isOpen() and not serialConnect(port.get()): return
    
    selection = captEdge2.get()
    if selection == options[0]: senseCommandB |= 0x3
    elif selection == options[1]: senseCommandB |= 0x2
    else: senseCommandB |= 0x1
    selection = captEdge3.get()
    if selection == options[0]: senseCommandB |= 0xC
    elif selection == options[1]: senseCommandB |= 0x8
    else: senseCommandB |= 0x4
    
    selection = captEdge18.get()
    if selection == options[0]: senseCommandA |= 0xC0
    elif selection == options[1]: senseCommandA |= 0x80
    else: senseCommandA |= 0x40
    selection = captEdge19.get()
    if selection == options[0]: senseCommandA |= 0x30
    elif selection == options[1]: senseCommandA |= 0x20
    else: senseCommandA |= 0x10

    if chan2.get(): maskCommand |= 0x10
    if chan3.get(): maskCommand |= 0x20
    if chan18.get(): maskCommand |= 0x8
    if chan19.get(): maskCommand |= 0x4

    ser.write(senseCommandA.to_bytes(1, byteorder='little', signed=False))
    ser.write(senseCommandB.to_bytes(1, byteorder='little', signed=False))
    ser.write(maskCommand.to_bytes(1, byteorder='little', signed=False))

def serialReadLoop():
    while(loop):
        newState = int.from_bytes(ser.read(1), byteorder='little', signed=False)
        newTime = int.from_bytes(ser.read(4), byteorder='little', signed=False)
        newPin = int.from_bytes(ser.read(1), byteorder='little', signed=False)

        if(newPin == 19):
            state = int2State
            absTime = int2AbsTime
        elif(newPin == 18):
            state = int3State
            absTime = int3AbsTime
        elif(newPin == 2):
            state = int4State
            absTime = int4AbsTime
        else:
            state = int5State
            absTime = int5AbsTime
        state.append(newState)
        absTime.append(newTime)

        text.config(state=NORMAL)
        if(len(state) == 1):
            text.insert(END, '{:d}, {:d}, first capture\n'
                .format(newState, newPin));
        else:
            difference = newTime-absTime[-2];
            text.insert(END, '{:d}, {:d}, {:f} seconds    (raw {:d})\n'
                .format(newState, newPin, float(difference)/16000000, difference));
        text.config(state=DISABLED)
        text.see(END)

def serialConnect(port):
    ser.port = port
    ser.baudrate = 1000000
    try: ser.open()
    except serial.SerialException as err:
        print("Cannot open serial connection:")
        print(err)
        ser.close()
        return False
    time.sleep(1)
    readThread.start()
    return True

options = ["rising edges", "falling edges", "both edges"]

root = Tk()

captEdge2 = StringVar(value=options[0])
captEdge3 = StringVar(value=options[0])
captEdge18 = StringVar(value=options[0])
captEdge19 = StringVar(value=options[0])
chan2 = BooleanVar()
chan3 = BooleanVar()
chan18 = BooleanVar()
chan19 = BooleanVar()

int2State = []
int3State = []
int4State = []
int5State = []
int2AbsTime = []
int3AbsTime = []
int4AbsTime = []
int5AbsTime = []

ser = serial.Serial()
loop = True
readThread = threading.Thread(target=serialReadLoop)

f = Frame(root)
Label(f, text="Serial Port:").pack(side=LEFT)
port = Entry(f)
port.insert(0, "COM3")
port.pack(side=LEFT)
f.pack()

f = Frame(root)
Checkbutton(f, variable=chan2).pack(side=LEFT)
Label(f, text="Capture ").pack(side=LEFT)
OptionMenu(f, captEdge2, *options).pack(side=LEFT)
Label(f, text="on channel 2").pack(side=LEFT)
f.pack()

f = Frame(root)
Checkbutton(f, variable=chan3).pack(side=LEFT)
Label(f, text="Capture ").pack(side=LEFT)
OptionMenu(f, captEdge3, *options).pack(side=LEFT)
Label(f, text="on channel 3").pack(side=LEFT)
f.pack()

f = Frame(root)
Checkbutton(f, variable=chan18).pack(side=LEFT)
Label(f, text="Capture ").pack(side=LEFT)
OptionMenu(f, captEdge18, *options).pack(side=LEFT)
Label(f, text="on channel 18").pack(side=LEFT)
f.pack()

f = Frame(root)
Checkbutton(f, variable=chan19).pack(side=LEFT)
Label(f, text="Capture ").pack(side=LEFT)
OptionMenu(f, captEdge19, *options).pack(side=LEFT)
Label(f, text="on channel 19").pack(side=LEFT)
f.pack()

Button(root, text="Go!", command=sendCommands).pack()

f = Frame(root)
text = Text(f, state=DISABLED)
text.pack(side=LEFT)
scrollbar = Scrollbar(f, command=text.yview)
scrollbar.pack(side=RIGHT, fill=Y)
text.config(yscrollcommand=scrollbar.set)
f.pack()

root.mainloop()

loop = False
if readThread.isAlive(): readThread.join()
if ser.isOpen(): ser.close()
