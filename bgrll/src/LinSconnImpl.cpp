/*********************************************************************
Matt Marchant 2015
http://trederia.blogspot.com
bgrll - Zlib license.
This software is provided 'as-is', without any express or
implied warranty. In no event will the authors be held
liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute
it freely, subject to the following restrictions:
1. The origin of this software must not be misrepresented;
you must not claim that you wrote the original software.
If you use this software in a product, an acknowledgment
in the product documentation would be appreciated but
is not required.
2. Altered source versions must be plainly marked as such,
and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any
source distribution.
*********************************************************************/

#ifdef __LINUX__

#include <SerialConn.hpp>

#include <iostream>
#include <cassert>

namespace
{
    const char comPorts[30][16] = 
    { 
        "/dev/ttyS0","/dev/ttyS1","/dev/ttyS2","/dev/ttyS3","/dev/ttyS4","/dev/ttyS5",
        "/dev/ttyS6","/dev/ttyS7","/dev/ttyS8","/dev/ttyS9","/dev/ttyS10","/dev/ttyS11",
        "/dev/ttyS12","/dev/ttyS13","/dev/ttyS14","/dev/ttyS15","/dev/ttyUSB0",
        "/dev/ttyUSB1","/dev/ttyUSB2","/dev/ttyUSB3","/dev/ttyUSB4","/dev/ttyUSB5",
        "/dev/ttyAMA0","/dev/ttyAMA1","/dev/ttyACM0","/dev/ttyACM1",
        "/dev/rfcomm0","/dev/rfcomm1","/dev/ircomm0","/dev/ircomm1"
    };
    const std::size_t MAX_ARRAY_SIZE = 4096;
}

SerialConnection::LinSconnImpl::LinSconnImpl()
    : m_error(0)
{
    //init arrays to default value
    for (auto& c : m_comPortHandles) c = -1;

}

bool SerialConnection::LinSconnImpl::openPort(std::uint16_t port, std::uint32_t baud)
{
    int baudrate = 0;

    assert(port < 30);

    //set baudrate
    switch (baud)
    {
    case 50:
        baudrate = B50;
        break;
    case 75:
        baudrate = B75;
        break;
    case 110:
        baudrate = B110;
        break;
    case 134:
        baudrate = B134;
        break;
    case 150:
        baudrate = B150;
        break;
    case 200:
        baudrate = B200;
        break;
    case 300:
        baudrate = B300;
        break;
    case 600:
        baudrate = B600;
        break;
    case 1200:
        baudrate = B1200;
        break;
    case 1800:
        baudrate = B1800;
        break;
    case 2400:
        baudrate = B2400;
        break;
    case 4800:
        baudrate = B4800;
        break;
    case 9600:
        baudrate = B9600;
        break;
    case 19200:
        baudrate = B19200;
        break;
    case 38400:
        baudrate = B38400;
        break;
    case 57600:
        baudrate = B57600;
        break;
    case 115200:
        baudrate = B115200;
        break;
    case 230400:
        baudrate = B230400;
        break;
    case 460800:
        baudrate = B460800;
        break;
    case 500000:
        baudrate = B500000;
        break;
    case 576000:
        baudrate = B576000;
        break;
    case 921600:
        baudrate = B921600;
        break;
    case 1000000:
        baudrate = B1000000;
        break;
    default: std::cerr << baud << ": Invalid baudrate." << std::endl;
        return false;
    }

    //attempt to open port
    m_comPortHandles[port] = open(comPorts[port], O_RDWR | O_NOCTTY | O_NDELAY);
    if (m_comPortHandles[port] == -1)
    {
        perror("SerConn::OpenPort: Unable to open port");
        return false;
    }
    //apply port settings
    error = tcgetattr(m_comPortHandles[port], m_oldPortSettings + port);
    if (error == -1)
    {
        close(m_comPortHandles[port]);
        perror("SerConn::OpenPort: Unable to read port settings");
        return false;
    }
    //clear and set port settings struct
    memset(&m_newPortSettings, 0, sizeof(m_newPortSettings));
    m_newPortSettings.c_cflag = baudrate | CS8 | CLOCAL | CREAD;
    m_newPortSettings.c_iflag = IGNPAR;
    m_newPortSettings.c_oflag = 0;
    m_newPortSettings.c_lflag = 0;
    m_newPortSettings.c_cc[VMIN] = 0; //block until n bytes rcd
    m_newPortSettings.c_cc[VTIME] = 0; //block until timer expires

    error = tcsetattr(m_comPortHandles[port], TCSANOW, &m_newPortSettings);
    if (error == -1)
    {
        close(m_comPortHandles[port]);
        perror("SerConn::OpenPort: Unable to apply port settings");
        return false;
    }

    //check port status
    int status = 0;
    if (ioctl(m_comPortHandles[port], TIOCMGET, &status) == -1)
    {
        perror("SerConn::OpenPort: Unable to get port status");
        return false;
    }
    //enable dtr and rts
    status |= TIOCM_DTR;
    status |= TIOCM_RTS;
    //update port status
    if (ioctl(m_comPortHandles[port], TIOCMSET, &status) == -1)
    {
        perror("SerConn::OpenPort: Unable to update port status");
        return false;
    }
    return true;
}

bool SerialConnection::LinSconnImpl::portOpened(std::uint16_t port) const
{
    return false;
}

void SerialConnection::LinSconnImpl::closePort(std::uint16_t port)
{
    int status = 0;
    if (ioctl(m_comPortHandles[port], TIOCMGET, &status) == -1)
    {
        perror("SerConn::ClosePort: Unable to get port status");
    }
    //turn off DTR and RTS
    status &= ~TIOCM_DTR;
    status &= ~TIOCM_RTS;

    if (ioctl(m_comPortHandles[port], TIOCMSET, &status) == -1)
    {
        perror("SerConn::ClosePort: Unable to set port status");
    }

    close(m_comPortHandles[port]);
    tcsetattr(m_comPortHandles[port], TCSANOW, m_oldPortSettings + port);
}

bool SerialConnection::LinSconnImpl::readByte(std::uint16_t port, byte& dst)
{
    if (m_comPortHandles[port] == -1)
    {
        perror("SerConn::ReadByte: Invalid port specified");
        return false;
    }

    read(m_comPortHandles[port], &dst, 1);
    return true;
}

std::size_t SerialConnection::LinSconnImpl::readByteArray(std::uint16_t port, std::vector<byte>& dst)
{
    if (m_comPortHandles[port] == -1)
    {
        perror("SerConn::ReadByteArray: Invalid port specified");
        return false;
    }

#ifndef __STRICT_ANSI__ //-ansi flag in GCC
    assert(dst.size() < SSIZE_MAX);
#else
    assert(dst.size() < MAX_ARRAY_SIZE);
#endif //__STRICT_ANSI__

    //TODO need to read a byte at a time using the delay property
    auto readCount = read(m_comPortHandles[port], dst.data(), dst.size());
    if (dst.size() > readCount) dst.resize(readCount);
    return readCount;
}

bool SerialConnection::LinSconnImpl::sendByte(std::uint16_t port, byte data)
{
    if (m_comPortHandles[port] == -1)
    {
        perror("SerConn::SendByte: Invalid port specified");
        return -1;
    }
    int n = write(m_comPortHandles[port], &data, 1);
    return (n >= 0);
}

std::size_t SerialConnection::LinSconnImpl::sendByteArray(std::uint16_t port, const std::vector<byte>& data)
{
#ifndef __STRICT_ANSI__ //-ansi flag in GCC
    assert(data.size() < SSIZE_MAX);
#else
    assert(data.size() < MAX_ARRAY_SIZE);
#endif //__STRICT_ANSI__

    if (m_comPortHandles[port] == -1)
    {
        perror("SerConn::SendByteArray: Invalid port specified");
        return 0;
    }
    //TODO need to send a byte at a time using the delay property
    return write(m_comPortHandles[port], data.data(), data.size());
}

//linux version of static function
std::vector<std::string> SerialConnection::SconnImpl::getAvailablePorts()
{
    //TODO check /dev/serial/ exists, and if it does list contents
    
    return{};
}

#endif //__LINUX__