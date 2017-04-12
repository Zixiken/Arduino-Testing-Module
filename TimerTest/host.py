import serial

state = [];
pin = [];
absTime = [];

with serial.Serial() as ser:
    ser.port = 'COM3'
    ser.baudrate = 115200
    ser.open()
    for i in range(8):
        state.append(int.from_bytes(ser.read(1), byteorder='little', signed=False))
        pin.append(int.from_bytes(ser.read(1), byteorder='little', signed=False))
        absTime.append(int.from_bytes(ser.read(4), byteorder='little', signed=False))
    

print('{:d}, {:d}, first capture'.format(state[0], pin[0]));
for i in range(1,8):
    print('{:d}, {:d}, {:f} seconds    (raw {:d})'.format(state[i], pin[i], ((float)(absTime[i]-absTime[i-1]))/16000000, absTime[i]-absTime[i-1]));
