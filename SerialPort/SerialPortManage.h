#ifndef SERIALPORTMANAGE_H
#define SERIALPORTMANAGE_H
#include <string>

class SerialPortManage
{
    public:
        SerialPortManage(const char serialPortName[]);        // Constructor
        ~SerialPortManage();                                  // Destructor
        void SerialWrite(const char *content);                // Write on serial port
        void SerialWrite(const std::string &content);         // Write on serial port
        std::string SerialRead();                             // Read from serial port
        int GetSerialPortAddress(void);                       // Serial port address getter

    private:
        int fd = 0;                                           // Serial port address
};

#endif // SERIALPORTMANAGE_H
