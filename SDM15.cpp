#include "SDM15.h"

byte SDM15::CalculateChecksum(byte *recv, int size)
{
    byte checksum = 0;
    for (int i = 0; i < size - 1; i++)
    {
        checksum += recv[i];
    }
    checksum = checksum & 0xFF;
    return checksum;
}

void SDM15::ClearBuffer()
{
    while (_sensor_serial.available() > 0)
    {
        _sensor_serial.read();
    }
}

VersionInfo SDM15::ObtainVersionInfo()
{
    // clear buffer
    ClearBuffer();

    // send cmd : PACKET_HED1, PACKET_HED2, GET_DEVICE_INFO, NO_DATA, 0x61
    byte cmd[5] = {PACKET_HED1, PACKET_HED2, GET_DEVICE_INFO, NO_DATA, 0x61};
    _sensor_serial.write(cmd, sizeof(cmd));
    _sensor_serial.flush();

    byte recv[25];

    // receive byte data response : PACKET_HED1, PACKET_HED2, GET_DEVICE_INFO, data_length, data[20], checksum
    _sensor_serial.readBytes(recv, sizeof(recv));

    // calculate checksum
    byte checksum = CalculateChecksum(recv, sizeof(recv));

    VersionInfo info;

    // check checksum
    if (checksum != recv[24])
    {
        info.checksum_error = true;
        return info;
    }

    info.checksum_error = false;

    // int型で取得
    info.model = int(recv[4]);
    info.hardware_version = recv[5];
    info.firmware_version_major = recv[6];
    info.firmware_version_minor = recv[7];

    // serial_numberはrecvの8~23番目のデータを結合して取得
    // 1バイト1桁として結合する
    info.serial_number = "";
    for (int i = 8; i < 24; i++)
    {
        info.serial_number += String(recv[i], HEX);
    }

    // バージョン情報を返す
    return info;
}

TestResult SDM15::SelfCheckTest()
{
    // clear buffer
    ClearBuffer();

    // send cmd : PACKET_HED1, PACKET_HED2, SELF_TEST, NO_DATA, 0x62
    byte cmd[5] = {PACKET_HED1, PACKET_HED2, SELF_TEST, NO_DATA, 0x62};
    _sensor_serial.write(cmd, sizeof(cmd));
    _sensor_serial.flush();

    byte recv[39];

    // receive byte data response : PACKET_HED1, PACKET_HED2, SELF_TEST, data_length, data[34], checksum
    _sensor_serial.readBytes(recv, sizeof(recv));

    // calculate checksum
    byte checksum = CalculateChecksum(recv, sizeof(recv));

    TestResult test;

    // check checksum
    if (checksum != recv[38])
    {
        test.checksum_error = true;
        return test;
    }

    test.checksum_error = false;

    if (recv[4] == 0x01)
        test.self_check_result = true;
    else
        test.self_check_result = false;

    // int型で取得
    test.self_check_error_code = recv[5];

    return test;
}

bool SDM15::StartScan()
{
    // clear buffer
    ClearBuffer();

    // send cmd : PACKET_HED1, PACKET_HED2, START_SCAN, NO_DATA, 0x5F
    byte cmd[5] = {PACKET_HED1, PACKET_HED2, START_SCAN, NO_DATA, 0x5F};
    _sensor_serial.write(cmd, sizeof(cmd));
    _sensor_serial.flush();

    byte recv[5];

    // receive byte data response : PACKET_HED1, PACKET_HED2, START_SCAN, NO_DATA, checksum
    _sensor_serial.readBytes(recv, sizeof(recv));

    // calculate checksum
    byte checksum = CalculateChecksum(recv, sizeof(recv));

    // check checksum
    if (checksum != recv[4])
        return false;
    else
        return true;
}

bool SDM15::StopScan()
{
    // clear buffer
    ClearBuffer();

    // send cmd : PACKET_HED1, PACKET_HED2, STOP_SCAN, NO_DATA, 0x60
    byte cmd[5] = {PACKET_HED1, PACKET_HED2, STOP_SCAN, NO_DATA, 0x60};
    _sensor_serial.write(cmd, sizeof(cmd));
    _sensor_serial.flush();

    byte recv[5];

    // receive byte data response : PACKET_HED1, PACKET_HED2, STOP_SCAN, NO_DATA, checksum
    _sensor_serial.readBytes(recv, sizeof(recv));

    // calculate checksum
    byte checksum = CalculateChecksum(recv, sizeof(recv));

    // check checksum
    if (checksum != recv[4])
        return false;
    else
        return true;
}

ScanData SDM15::GetScanData()
{
    // clear buffer
    ClearBuffer();

    // read data
    byte recv[9];

    // receive byte data response : PACKET_HED1, PACKET_HED2, START_SCAN, data_length, data[4], checksum
    _sensor_serial.readBytes(recv, sizeof(recv));

    // calculate checksum
    byte checksum = CalculateChecksum(recv, sizeof(recv));

    ScanData data;

    // check checksum
    if (checksum != recv[8])
    {
        data.checksum_error = true;
        return data;
    }

    data.checksum_error = false;

    data.distance = int(recv[4]) + (int(recv[5]) << 8);
    data.intensity = int(recv[6]);
    data.disturb = int(recv[7]);

    return data;
}