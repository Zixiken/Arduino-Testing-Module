import serial

int2State = [];
int3State = [];
int4State = [];
int5State = [];
int2AbsTime = [];
int3AbsTime = [];
int4AbsTime = [];
int5AbsTime = [];

with serial.Serial() as ser:
    ser.port = 'COM3'
    ser.baudrate = 1000000
    ser.open()
    while(1):
        newState = int.from_bytes(ser.read(1), byteorder='little', signed=False)
        newTime = int.from_bytes(ser.read(4), byteorder='little', signed=False)
        newPin = int.from_bytes(ser.read(1), byteorder='little', signed=False)

        if(newPin == 19):
            state = int2State;
            absTime = int2AbsTime;
        elif(newPin == 18):
            state = int3State;
            absTime = int3AbsTime;
        elif(newPin == 2):
            state = int4State;
            absTime = int4AbsTime;
        else:
            state = int5State;
            absTime = int5AbsTime;
        state.append(newState);
        absTime.append(newTime);
        
        if(len(state) == 1):
            print('{:d}, {:d}, first capture'.format(newState, newPin));
        else:
            difference = newTime-absTime[-2];
            print('{:d}, {:d}, {:f} seconds    (raw {:d})'
                .format(newState, newPin, float(difference)/16000000, difference));
