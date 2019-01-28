#include "SerialPortManage.h"
#include <stdio.h>   /* Standard input/output definitions */
#include <iostream>
#include <fcntl.h>   /* File control definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <string.h>
#include <errno.h>

#define MAXBYTES 200

SerialPortManage::SerialPortManage(const char serialPortName[])
{
    // Open serial port
    fd = open(serialPortName, O_RDWR | O_NOCTTY);

    // Check for opened port
    if (fd < 0)
    {
        std::cout << "Error opening port" << std::endl;
        exit(-1);
    }
    else
    {
        // Create structure for port settings
        struct termios tty;
        memset(&tty, '\0', sizeof tty);

        // Check current port settings
        if (tcgetattr(fd, &tty) != 0)
            std::cout << "Error, cannot get parameters, error: " << strerror(errno) << std::endl;

        // Set baud rate
        cfsetispeed(&tty, (speed_t)B9600);
        cfsetospeed(&tty, (speed_t)B9600);

        // Set port parameterts as 8N1
        tty.c_cflag &= ~PARENB;         // 8N1
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CSIZE;
        tty.c_cflag |= CS8;
        tty.c_cflag &= ~CRTSCTS;        // No flow control
        tty.c_cc[VMIN] = 1;             // Read doesn't block
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout
        tty.c_cflag |= CREAD | CLOCAL;  // Turn on READ and ignore ctrl lines

        // Make setting structure raw
        cfmakeraw(&tty);

        // Flush port then applies attributes
        tcflush(fd, TCIFLUSH);
        if (tcsetattr(fd, TCSANOW, &tty) != 0)
            std::cout << "Error, cannot set port parameters, error: " << strerror(errno) << std::endl;

    }
}

SerialPortManage::~SerialPortManage()
{
    // Close serial port
    close(fd);
}

void SerialPortManage::SerialWrite(const char *content)
{
    // Write on serial port
    int n = write(fd, content, strlen(content));

    // Wait for the buffer to be sent completely
    usleep(100000);

    // Check for wrote content
    if (n <= 0)
    {
        std::cout << "Cannot write on serial port" << std::endl;
        exit(-1);
    }
}

void SerialPortManage::SerialWrite(const std::string &content)
{
    // Write on serial port
    int n = write(fd, content.c_str(), content.size());

    // Wait for the buffer to be sent completely
    usleep(100000);

    // Check for wrote content
    if (n <= 0)
    {
        std::cout << "Cannot write on serial port" << std::endl;
        exit(-1);
    }
}

std::string SerialPortManage::SerialRead()
{
    int n = 0;
    char response[MAXBYTES] = "";

    // Read from serial port
    n = read(fd, &response, MAXBYTES);

    // Check for errors
    if (n < 0)
    {
        std::cout << "Cannot read from serial port, error: " << strerror(errno) << std::endl;
        exit(-1);
    }

    // Check for no character read
    else if (n == 0)
        std::cout << "Nothing to read" << std::endl;

    // Check for received characters
    else if (n > 0)
    {
        return std::string(response);
    }
}

int SerialPortManage::GetSerialPortAddress(void)
{
  return fd;
}
