#!/usr/bin/env python3
import os
import re
import csv
import serial

class KnxSerial:
    DEVICE = '/dev/ttyAMA0'
    BAUDRATE = 19200
    CHARACTER_SIZE = serial.EIGHTBITS
    PARITY = serial.PARITY_EVEN

class Bytes:
    STARTBYTE = 0
    DEST_HIGH_BYTE = 11
    DEST_LOW_BYTE = 12
    PAYLOAD = {'Byte0': 15}

class BytesValues:
    STARTBYTE = '68'
    PAYLOAD = {
        'DPT1': {'81': 'on', '80': 'off'}
    }
    STOPBYTE = '16'


def read_frame(connection):
    frame = []
    current_byte = 'FF'

    while current_byte != BytesValues.STOPBYTE:
        current_byte = connection.read().hex()
        frame = check_startbyte(frame)
        frame.append(current_byte)

    return frame

def check_startbyte(frame):
    first_byte_not_startbyte = frame and frame[Bytes.STARTBYTE] != BytesValues.STARTBYTE

    if first_byte_not_startbyte:
            frame.pop(Bytes.STARTBYTE)

    return frame

def get_status(frame):
    return {
            'Groupaddress': get_groupaddress(frame).get('formatted'),
            'Status': get_value(frame).get('formatted'),
        }

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
    raw_value = frame[Bytes.PAYLOAD.get('Byte0')]

    formatted_value = BytesValues.PAYLOAD.get('DPT1')[raw_value]

    return {'raw': raw_value, 'formatted': formatted_value}

def writer(filename, status):
    with open(filename, "w", newline="") as csv_file:
        stati = csv.DictWriter(csv_file, fieldnames=status.keys())
        stati.writeheader()
        stati.writerow(status)

def updater(filename, status):
    with open(filename, newline="") as file:
        readData = [row for row in csv.DictReader(file)]
        found = False
        new_status = {} 

        for index, raw in enumerate(readData):
            if status.get('Groupaddress') in readData[index]['Groupaddress']:
                readData[index]['Status'] = status.get('Status')
                found = True
                break

        if not found:
            new_status = status

    with open(filename, "w", newline="") as csv_file:
        writer = csv.DictWriter(csv_file, fieldnames = status.keys())
        writer.writeheader()
        writer.writerows(readData)

        if new_status:
            writer.writerow(new_status)

def save_status(status):
    FILENAME = 'KNX_stati.csv'
    print(status)
    file_exists = os.path.isfile(FILENAME)

    if file_exists:
        updater(FILENAME, status)

    else:
        writer(FILENAME, status)

def main():
    with serial.Serial(
        KnxSerial.DEVICE, KnxSerial.BAUDRATE, KnxSerial.CHARACTER_SIZE, KnxSerial.PARITY
        ) as connection:

        while True:
            frame = read_frame(connection)
            status = get_status(frame)
            save_status(status)

            # get groupaddress raw and formatted
            # check datapointtype in .csv

if __name__ == "__main__":
    main()
