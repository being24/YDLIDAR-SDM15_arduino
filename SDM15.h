#ifndef SDM15_H
#define SDM15_H

#include <Arduino.h>

struct VersionInfo
{
    bool checksum_error;
    int model;
    int hardware_version;
    int firmware_version_major;
    int firmware_version_minor;
    String serial_number;
};

struct TestResult
{
    bool checksum_error;
    bool self_check_result;
    byte self_check_error_code;
    byte test_data[32];
};

struct ScanData
{
    bool checksum_error;
    int distance;
    int intensity;
    int disturb;
};

const int PACKET_HED1 = 0xAA;
const int PACKET_HED2 = 0x55;
const int START_SCAN = 0x60;
const int STOP_SCAN = 0x61;
const int GET_DEVICE_INFO = 0x62;
const int SELF_TEST = 0x63;
const int SET_OUTPUT_FREQ = 0x64;
const int SET_FILTER = 0x65;
const int SET_SERIAL_BAUD = 0x66;
const int SET_FORMAT_OUTPUT_DATA = 0x67;
const int RESTORE_FACTORY_SETTINGS = 0x68;
const int NO_DATA = 0x00;

class SDM15
{
public:
    SDM15(HardwareSerial &serial) : _sensor_serial(serial) {}
    VersionInfo ObtainVersionInfo();
    TestResult SelfCheckTest();
    bool StartScan();
    bool StopScan();
    ScanData GetScanData();

private:
    byte CalculateChecksum(byte *recv, int size);
    void ClearBuffer();
    HardwareSerial &_sensor_serial;
};

#endif // SDM15_H