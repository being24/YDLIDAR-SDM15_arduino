struct VersionInfo
{
    int model;
    int hardware_version;
    int firmware_version_major;
    int firmware_version_minor;
    String serial_number;
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

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.begin(9600);
    Serial1.begin(460800);

    delay(1000);
}

byte calculateChecksum(byte *recv, int size)
{
    byte checksum = 0;
    for (int i = 0; i < size - 1; i++)
    {
        checksum += recv[i];
    }
    checksum = checksum & 0xFF;
    return checksum;
}

VersionInfo ObtainVersionInfo()
{
    // clear buffer
    while (Serial1.available() > 0)
    {
        Serial1.read();
    }

    // send cmd : PACKET_HED1, PACKET_HED2, GET_DEVICE_INFO, NO_DATA, 0x61
    byte cmd[5] = {PACKET_HED1, PACKET_HED2, GET_DEVICE_INFO, NO_DATA, 0x61};
    Serial1.write(cmd, sizeof(cmd));
    Serial1.flush();

    byte recv[25];

    // receive byte data response : PACKET_HED1, PACKET_HED2, data_length, data[20], checksum
    Serial1.readBytes(recv, sizeof(recv));

    // calculate checksum
    byte checksum = calculateChecksum(recv, sizeof(recv));

    // check checksum
    if (checksum != recv[24])
    {
        Serial.println("checksum error");
        return VersionInfo();
    }

    VersionInfo info;

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
void loop()
{
    Serial.println("get data");

    // get version info
    VersionInfo info = ObtainVersionInfo();

    // print version info
    Serial.print("model: ");
    Serial.println(info.model);
    Serial.print("hardware_version: ");
    Serial.println(info.hardware_version);
    Serial.print("firmware_version_major: ");
    Serial.println(info.firmware_version_major);
    Serial.print("firmware_version_minor: ");
    Serial.println(info.firmware_version_minor);
    Serial.print("serial_number: ");
    Serial.println(info.serial_number);

    // wait 1s
    delay(1000);
}