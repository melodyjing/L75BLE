import pytest
import serial
import time
import subprocess
import pyvisa

com_ctrl = 'COM4'
com_slave = 'COM3'
com_master = 'COM6'
digi_meter = 'USB0::0x1AB1::0x09C4::DM3R201600831::INSTR'

def test_upgrade_slave():
    ser_ctrl = serial.Serial(com_ctrl, 115200, timeout=1)
    ser_ctrl.write(b'out(3,1)\n')
    time.sleep(5)

    ser_ctrl.write(b'out(4,1)\n')
    ser_ctrl.write(b'out(5,0)\n')
    ser_ctrl.write(b'out(3,0)\n')
    ser_ctrl.write(b'out(3,1)\n')
    time.sleep(5)

    cmd_sdk = '.\\download.exe ' + com_slave + ' sdk 00800000 .\\outbin.bin'
    exitcode_sdk, output_sdk = subprocess.getstatusoutput(cmd_sdk)
    ser_ctrl.write(b'out(4,0)\n')
    ser_ctrl.write(b'out(3,0)\n')
    ser_ctrl.write(b'out(3,1)\n')
    time.sleep(5)
    
    ser_ctrl.close()
    assert 'Success' in output_sdk

def test_upgrade_master():
    ser_ctrl = serial.Serial(com_ctrl, 115200, timeout=1)
    ser_ctrl.write(b'out(11,1)\n')
    time.sleep(5)

    ser_ctrl.write(b'out(12,1)\n')
    ser_ctrl.write(b'out(13,0)\n')
    ser_ctrl.write(b'out(11,0)\n')
    ser_ctrl.write(b'out(11,1)\n')
    time.sleep(5)

    cmd_sdk = '.\\download.exe ' + com_master + ' sdk 00800000 .\\outbin.bin'
    exitcode_sdk, output_sdk = subprocess.getstatusoutput(cmd_sdk)
    ser_ctrl.write(b'out(12,0)\n')
    ser_ctrl.write(b'out(11,0)\n')
    ser_ctrl.write(b'out(11,1)\n')
    time.sleep(5)

    ser_ctrl.close()
    assert 'Success' in output_sdk
    
def test_direct_current_no_sleep():
    rm = pyvisa.ResourceManager()
    dm = rm.open_resource(digi_meter)
    dc_list = []
    n = 10
    for i in range(0,n):
        dc = eval(dm.query(':MEASure:CURRent:DC?'))
        dc_list.append(dc)
        time.sleep(1)
    print(dc_list)

    dc_sum = 0
    for dc in dc_list:
        dc_sum = dc_sum + dc
    dc_average = dc_sum/len(dc_list)

    assert dc_average <= 0.010 and dc_average >= 0.005

    
if __name__ == '__main__':
    pytest.main()
    
