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

#ifdef _WIN32
#include <SerialConn.hpp>

#include <cassert>
#include <iostream> //TODO add more robust logging

namespace
{
    const char commPorts[16][10] = 
    {
        "\\\\.\\COM0",  "\\\\.\\COM1",  "\\\\.\\COM2",  "\\\\.\\COM3",
        "\\\\.\\COM4",  "\\\\.\\COM5",  "\\\\.\\COM6",  "\\\\.\\COM7",
        "\\\\.\\COM8",  "\\\\.\\COM9",  "\\\\.\\COM10", "\\\\.\\COM11",
        "\\\\.\\COM12", "\\\\.\\COM13", "\\\\.\\COM14", "\\\\.\\COM15" 
    };

    const std::size_t MAX_ARRAY_SIZE = 4096;
}

SerialConnection::WinSconnImpl::WinSconnImpl()
{
    for (auto& h : m_commPortHandles)
    {
        h = INVALID_HANDLE_VALUE;
    }
}

SerialConnection::WinSconnImpl::~WinSconnImpl()
{
    for (auto& h : m_commPortHandles)
    {
        if (h != INVALID_HANDLE_VALUE)
        {
            CloseHandle(h);
        }
    }
}

bool SerialConnection::WinSconnImpl::openPort(std::uint16_t port, std::uint32_t baud)
{
    //check valid number
    assert(port < 16);

    //create baudrate string
    switch (baud)
    {
    case 110:
        m_baudrate = "baud=110 data=8 parity=N stop=1 dtr=on rts=on";
        break;
    case 300:
        m_baudrate = "baud=300 data=8 parity=N stop=1 dtr=on rts=on";
        break;
    case 600:
        m_baudrate = "baud=600 data=8 parity=N stop=1 dtr=on rts=on";
        break;
    case 1200:
        m_baudrate ="baud=1200 data=8 parity=N stop=1 dtr=on rts=on";
        break;
    case 2400:
        m_baudrate = "baud=2400 data=8 parity=N stop=1 dtr=on rts=on";
        break;
    case 4800:
        m_baudrate = "baud=4800 data=8 parity=N stop=1 dtr=on rts=on";
        break;
    case 9600:
        m_baudrate ="baud=9600 data=8 parity=N stop=1 dtr=on rts=on";
        break;
    case 19200:
        m_baudrate = "baud=19200 data=8 parity=N stop=1 dtr=on rts=on";
        break;
    case 38400:
        m_baudrate = "baud=38400 data=8 parity=N stop=1 dtr=on rts=on";
        break;
    case 57600:
        m_baudrate = "baud=57600 data=8 parity=N stop=1 dtr=on rts=on";
        break;
    case 115200:
        m_baudrate = "baud=115200 data=8 parity=N stop=1 dtr=on rts=on";
        break;
    case 128000:
        m_baudrate = "baud=128000 data=8 parity=N stop=1 dtr=on rts=on";
        break;
    case 256000:
        m_baudrate = "baud=256000 data=8 parity=N stop=1 dtr=on rts=on";
        break;
    case 500000:
        m_baudrate = "baud=500000 data=8 parity=N stop=1 dtr=on rts=on";
        break;
    case 1000000:
        m_baudrate = "baud=1000000 data=8 parity=N stop=1 dtr=on rts=on";
        break;
    default:
        std::cerr << baud << ": Invalid baud rate" << std::endl;
        return false;
    }

    //get a handle to requested COM port
    m_commPortHandles[port] = CreateFileA
        (
            commPorts[port],
            GENERIC_READ | GENERIC_WRITE,
            0, //no share
            0, //no security
            OPEN_EXISTING,
            0, //no threads
            0
        ); //no template

    //check if port was created successfully
    if (m_commPortHandles[port] == INVALID_HANDLE_VALUE)
    {
        std::cerr << "Unable to open COM port " << port << std::endl;
        return false;
    }

    //clear the settings struct and apply to port
    memset(&m_portSettings, 0, sizeof(m_portSettings));
    m_portSettings.DCBlength = sizeof(m_portSettings);

    if (!BuildCommDCBA(m_baudrate.c_str(), &m_portSettings))
    {
        std::cerr << "Unable to apply COM port DCB settings." << std::endl;
        CloseHandle(m_commPortHandles[port]);
        return false;
    }

    if (!SetCommState(m_commPortHandles[port], &m_portSettings))
    {
        std::cerr << "Unable to apply COM port cfg settings." << std::endl;
        CloseHandle(m_commPortHandles[port]);
        return false;
    }

    //update timeout struct
    m_commTimeouts.ReadIntervalTimeout = MAXDWORD;
    m_commTimeouts.ReadTotalTimeoutConstant = 0;
    m_commTimeouts.ReadTotalTimeoutMultiplier = 0;
    m_commTimeouts.WriteTotalTimeoutConstant = 0;
    m_commTimeouts.WriteTotalTimeoutMultiplier = 0;

    //apply timeout settings
    if (!SetCommTimeouts(m_commPortHandles[port], &m_commTimeouts))
    {
        std::cerr << "Unable to apply COM port timeout settings." << std::endl;
        CloseHandle(m_commPortHandles[port]);
        return 1;
    }

    return true;
}

void SerialConnection::WinSconnImpl::closePort(std::uint16_t port)
{
    if (m_commPortHandles[port] != INVALID_HANDLE_VALUE)
    {
        CloseHandle(m_commPortHandles[port]);
    }
}

bool SerialConnection::WinSconnImpl::readByte(std::uint16_t port, byte& dst)
{
    if (m_commPortHandles[port] != INVALID_HANDLE_VALUE)
    {
        int readResult;
        ReadFile(m_commPortHandles[port], &dst, 1, (LPDWORD)((void*)&readResult), 0);
        return (readResult > 0);
    }
    return false;
}

std::size_t SerialConnection::WinSconnImpl::readByteArray(std::uint16_t port, std::vector<byte>& dst)
{
    assert(dst.size() <= MAX_ARRAY_SIZE); //limited to 4kb
    
    if (m_commPortHandles[port] != INVALID_HANDLE_VALUE)
    {
        std::size_t readSize = 0;

        ReadFile(m_commPortHandles[port], dst.data(), dst.size(), (LPDWORD)((void*)&readSize), 0);
        if(readSize < dst.size()) dst.resize(readSize);                
        return readSize;
    }
    return 0;
}

bool SerialConnection::WinSconnImpl::sendByte(std::uint16_t port, byte data)
{
    if (m_commPortHandles[port] != INVALID_HANDLE_VALUE)
    {
        int sendResult;
        WriteFile(m_commPortHandles[port], &data, 1, (LPDWORD)((void*)&sendResult), 0);
        return (sendResult > 0);
    }
    return false;
}

std::size_t SerialConnection::WinSconnImpl::sendByteArray(std::uint16_t port, const std::vector<byte>& data)
{
    assert(data.size() <= MAX_ARRAY_SIZE);
    if (m_commPortHandles[port] != INVALID_HANDLE_VALUE)
    {
        std::size_t sendResult;
        WriteFile(m_commPortHandles[port], data.data(), data.size(), (LPDWORD)((void*)&sendResult), 0);
        return sendResult;
    }
    return 0;
}

#endif //_WIN32