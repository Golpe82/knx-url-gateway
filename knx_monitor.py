import serial
from serial.tools.hexlify_codec import hex_decode

class KnxSerial:
    DEVICE = '/dev/ttyAMA0'
    BAUDRATE = 19200
    CHARACTER_SIZE = serial.EIGHTBITS
    PARITY = serial.PARITY_EVEN
    FIRST_BYTE = 0
    STARTBYTE = '68'
    STOPBYTE = '16'

def init_dataframe():
    return {'current_byte': 'FF', 'frame': []}

def hex_to_string(hex_value):
    return hex_decode(hex_value)[0].strip()

def check_startbyte(frame):
    if frame and frame[KnxSerial.FIRST_BYTE] != KnxSerial.STARTBYTE:
            frame.pop(KnxSerial.FIRST_BYTE)

    return frame


with serial.Serial(
    KnxSerial.DEVICE, KnxSerial.BAUDRATE, KnxSerial.CHARACTER_SIZE, KnxSerial.PARITY
    ) as connection:

    while True:
        dataframe = init_dataframe()
        current_byte = dataframe['current_byte']
        frame = dataframe['frame']

        while current_byte != KnxSerial.STOPBYTE:
            current_byte = hex_to_string(connection.read())
            frame = check_startbyte(frame)
            frame.append(current_byte)

        print(frame)
