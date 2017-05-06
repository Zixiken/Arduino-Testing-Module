import serial
import threading
import time
from tkinter import *

def sendCommands():
    senseCommandA = 0
    senseCommandB = 0
    maskCommand = 0

    global int2Stamps
    global int3Stamps
    global int4Stamps
    global int5Stamps
    global numCaptures
    int2Stamps = []
    int3Stamps = []
    int4Stamps = []
    int5Stamps = []
    numCaptures = 0

    text.config(state=NORMAL)
    text.delete(1.0, END)
    text.config(state=DISABLED)

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
    
    if burstMode.get(): threading.Thread(target=burstLoop).start()

def burstLoop():
    startTime = time.mktime(time.localtime())
    while numCaptures < 50 and time.mktime(time.localtime())-startTime < 10:
        continue
    sendStop()
    text.config(state=NORMAL)
    if len(int2Stamps) >= 1: dumpStamps(int2Stamps, 0)
    if len(int3Stamps) >= 1: dumpStamps(int3Stamps, 1)
    if len(int4Stamps) >= 1: dumpStamps(int4Stamps, 2)
    if len(int5Stamps) >= 1: dumpStamps(int5Stamps, 3)
    text.config(state=DISABLED)
    text.see(END)

def dumpStamps(stamps, pin):
    text.insert(END, '{:d}, {:d}, first capture\n'
        .format(stamps[0][0], pin));
    prevTime = stamps[0][1]
    for i in range(1, len(stamps)-1):
        curState = stamps[i][0]
        curTime = stamps[i][1]
        difference = curTime-prevTime;
        text.insert(END, '{:d}, {:d}, {:f} seconds    (raw {:d})\n'
            .format(curState, pin, float(difference)/16000000, difference));
        prevTime = curTime

def sendStop():
    if ser.isOpen(): ser.write(b'\x00')

def serialReadLoop():
    global numCaptures
    while(loop):
        timeStamp = ser.read(5)
        statePinByte = timeStamp[0]
        newTime = int.from_bytes(timeStamp[1:4], byteorder='little', signed=False)
        newPin = statePinByte & 0x03
        newState = statePinByte & 0xFC

        if newState != 0 and\
           newState != 4 and\
           newState != 8 and\
           newState != 16 and\
           newState != 32:
            ser.reset_input_buffer()
            continue

        if newPin == 0: stamps = int2Stamps
        elif newPin == 1: stamps = int3Stamps
        elif newPin == 2: stamps = int4Stamps
        else: stamps = int5Stamps
        stamps.append((newState, newTime))

        if burstMode.get(): numCaptures += 1
        else:
            text.config(state=NORMAL)
            if(len(stamps) == 1):
                text.insert(END, '{:d}, {:d}, first capture\n'
                    .format(newState, newPin));
            else:
                difference = newTime-stamps[-2][1];
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
burstMode = BooleanVar()

int2Stamps = []
int3Stamps = []
int4Stamps = []
int5Stamps = []
numCaptures = 0

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

f = Frame(root)
Checkbutton(f, variable=burstMode).pack(side=LEFT)
Label(f, text="Burst Mode").pack(side=LEFT)
f.pack()

f = Frame(root)
Button(f, text="Go!", command=sendCommands).pack(side=LEFT)
Button(f, text="Stop", command=sendStop).pack(side=LEFT)
f.pack()

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
