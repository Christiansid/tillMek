#import xlsxwriter
#import serial
import argparse
import time
from datetime import datetime
#workbook = xlsxwriter.Workbook("Hello.xlsx")
#worksheet = workbook.add_worksheet()
#worksheet.write('A1','Hello World')
#workbook.close()

x = 0
def test():
    parser = argparse.ArgumentParser()
    parser.add_argument('-test', type = str)
    arg = parser.parse_args()
    return arg.test
if __name__ == "__main__":
    test = test()
    #test = arg.test
    #print(test)
    print(test)