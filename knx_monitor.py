import serial
from serial.tools.hexlify_codec import hex_decode

class KnxSerial:
    DEVICE = '/dev/ttyAMA0'
    BAUDRATE = 19200
    CHARACTER_SIZE = serial.EIGHTBITS
    PARITY = serial.PARITY_EVEN

class Bytes:
    STARTBYTE = 0
    DEST_HIGH_BYTE = 11
    DEST_LOW_BYTE = 12
    BOOL_VALUE = 15


class BytesValues:
    STARTBYTE = '68'
    STOPBYTE = '16'
    DIRECTION = {
        'read': ''
    }
    BOOL = {'81': 'on', '80': 'off'}


def init_dataframe():
    return {'current_byte': 'FF', 'frame': []}

def hex_to_string(hex_value):
    return hex_decode(hex_value)[0].strip()

def check_startbyte(frame):
    if frame and frame[Bytes.STARTBYTE] != BytesValues.STARTBYTE:
            frame.pop(Bytes.STARTBYTE)

    return frame

def get_groupaddress(frame):
    raw_address = f"{ frame[Bytes.DEST_HIGH_BYTE] } { frame[Bytes.DEST_LOW_BYTE] }"
    subaddress = int(f"0x{ frame[Bytes.DEST_LOW_BYTE] }", 16)
    high_byte = int(f"0x{ frame[Bytes.DEST_HIGH_BYTE] }", 16)
    midaddress_mask = int('0x07', 16)
    midaddress = high_byte & midaddress_mask
    mainaddress = high_byte >> 3

    groupaddress = f'{ mainaddress }/{ midaddress }/{ subaddress }'

    return {'raw': raw_address, 'formatted': groupaddress}

def get_value(frame):
    raw_value = frame[Bytes.BOOL_VALUE]
    formatted_value = BytesValues.BOOL[raw_value]

    return {'raw': raw_value, 'formatted': formatted_value}


with serial.Serial(
    KnxSerial.DEVICE, KnxSerial.BAUDRATE, KnxSerial.CHARACTER_SIZE, KnxSerial.PARITY
    ) as connection:

    while True:
        
        dataframe = init_dataframe()
        #get frame
        current_byte = dataframe.get('current_byte')
        frame = dataframe.get('frame')
        
        while current_byte != BytesValues.STOPBYTE:
            current_byte = hex_to_string(connection.read())
            frame = check_startbyte(frame)
            frame.append(current_byte)

        print(frame)
        # get groupaddress raw and formatted
        # check datapointtype in .csv
        # print(frame)
        print(get_groupaddress(frame).get('formatted'), get_value(frame).get('formatted'))
