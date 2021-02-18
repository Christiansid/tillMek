import threading
import serial
import sys
import time
import struct
from datetime import datetime
import argparse
import xlsxwriter

timeCol = 0
flowrateCol = 1
floatSigCol = 2
stopSigCol = 3
start_time = datetime.now()
parser = argparse.ArgumentParser('Parser to determine XLSX file')
parser.add_argument('-xlsx_name', type = str, default= 'simRun.xlsx')
parser.add_argument('-port', type = str, default = 'COM3')
args = parser.parse_args()
name = args.xlsx_name
port = args.port
def setup():

    #port = 'COM3'
    #port = '/dev/cu.usbmodem143401'

    print('\nINITIALIZING')
    #Arduino
    arduino = serial.Serial(port, baudrate = 9600, timeout=3.000)
    handshake(arduino)


    #Excel file
    #name = 'test.xlsx'
    workbook = xlsxwriter.Workbook(name)
    return arduino, workbook

def xlsxSetup(workbook):
    print('- Worksheed created.')
    bold = workbook.add_format({'bold': True})
    date_format = workbook.add_format({'num_format': 'yyyy-mm-dd hh:mm:ss.000'})
    worksheet = workbook.add_worksheet()

    worksheet.write_datetime('A1', start_time, date_format)
    worksheet.write('A3', 'Timestamp', bold)
    worksheet.write('B3', 'Flow rate', bold)
    worksheet.write('C3', 'Float switch', bold)
    worksheet.write('D3', 'Stop signal', bold)

    return worksheet


def handshake(arduino):
    time.sleep(1)                 # SIMPLE HANDSHAKE
    arduino.reset_input_buffer()
    while True:
        msg = arduino.read().decode()

        if (msg == 'M'):                    #Marco
            print("- Handshake successful.")
            arduino.write('P'.encode())     #Polo
            arduino.reset_input_buffer()
            break
        else:
            print('Handshake failed. Damn.')
            arduino.reset_input_buffer()


def time_diff():
    return (datetime.now() - start_time).total_seconds()

def thread_func(arduino):
        while True:
            print('\nType flow rate (15ml/m - 350ml/m):')
            rate = int(input())

            if (rate < 15):
                rate = 15
            elif (rate > 350):
                rate = 350

            # WRITE CODE FOR SENDING FLOW RATE TO Arduino HERE


    #print('Flow rate set to: {} ml \n'.format(rate) )


if __name__ == "__main__":
    try:
        row = 3
        lista = []
        [arduino, workbook] = setup()
        worksheet = xlsxSetup(workbook)

        print("- Flow rate input thread starting.")
        #t1 = threading.Thread(target = thread_func, args=(arduino,))
        #t1.daemon = True
        #t1.start()


        while True:
            val = arduino.read().decode()

            check = worksheet.write(row, timeCol, time_diff() )
            if(check != 0):
                raise xlsxwriter.exceptions.XlsxWriterException

            check = worksheet.write(row,flowrateCol, val)
            if(check != 0):
                raise xlsxwriter.exceptions.XlsxWriterException

            check = worksheet.write(row, floatSigCol, val)
            if(check != 0):
                raise xlsxwriter.exceptions.XlsxWriterException
            val = arduino.read().decode()

            check = worksheet.write(row, stopSigCol, val)
            if(check != 0):
                raise xlsxwriter.exceptions.XlsxWriterException

            row = row+1

    except KeyboardInterrupt:
        workbook.close()
        raise KeyboardInterrupt

    except xlsxwriter.exceptions.XlsxWriterException:
        print('Failed to write to XLSX document.')
        workbook.close()
