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
    const char comPorts[16][10] = 
    {
        "\\\\.\\COM0",  "\\\\.\\COM1",  "\\\\.\\COM2",  "\\\\.\\COM3",
        "\\\\.\\COM4",  "\\\\.\\COM5",  "\\\\.\\COM6",  "\\\\.\\COM7",
        "\\\\.\\COM8",  "\\\\.\\COM9",  "\\\\.\\COM10", "\\\\.\\COM11",
        "\\\\.\\COM12", "\\\\.\\COM13", "\\\\.\\COM14", "\\\\.\\COM15" 
    };

    const std::size_t MAX_ARRAY_SIZE = 4096;

    //wrapper around reg key to provide convenient release via RAII
    struct KeyRelease
    {
    public:
        KeyRelease(HKEY k) : m_key(k) {}
        ~KeyRelease() { RegCloseKey(m_key); }
    private:
        HKEY m_key;
    };

    bool rxReady(HANDLE port)
    {
        if (SetCommMask(port, EV_RXCHAR))
        {
            unsigned long mask;
            WaitCommEvent(port, &mask, nullptr); //TODO check result of this and report error if 0
            return true;
        }
        return false;
    }
}

SerialConnection::WinSconnImpl::WinSconnImpl()
{
    for (auto& h : m_comPortHandles)
    {
        h = INVALID_HANDLE_VALUE;
    }
}

SerialConnection::WinSconnImpl::~WinSconnImpl()
{
    for (auto& h : m_comPortHandles)
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
    m_comPortHandles[port] = CreateFileA
        (
            comPorts[port],
            GENERIC_READ | GENERIC_WRITE,
            0, //no share
            0, //no security
            OPEN_EXISTING,
            0, //no threads
            0
        ); //no template

    //check if port was created successfully
    if (m_comPortHandles[port] == INVALID_HANDLE_VALUE)
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
        CloseHandle(m_comPortHandles[port]);
        return false;
    }

    if (!SetCommState(m_comPortHandles[port], &m_portSettings))
    {
        std::cerr << "Unable to apply COM port cfg settings." << std::endl;
        CloseHandle(m_comPortHandles[port]);
        return false;
    }

    //update timeout struct
    m_comTimeouts.ReadIntervalTimeout = MAXDWORD;
    m_comTimeouts.ReadTotalTimeoutConstant = 0;
    m_comTimeouts.ReadTotalTimeoutMultiplier = 0;
    m_comTimeouts.WriteTotalTimeoutConstant = 0;
    m_comTimeouts.WriteTotalTimeoutMultiplier = 0;

    //apply timeout settings
    if (!SetCommTimeouts(m_comPortHandles[port], &m_comTimeouts))
    {
        std::cerr << "Unable to apply COM port timeout settings." << std::endl;
        CloseHandle(m_comPortHandles[port]);
        return 1;
    }

    return true;
}

void SerialConnection::WinSconnImpl::closePort(std::uint16_t port)
{
    if (m_comPortHandles[port] != INVALID_HANDLE_VALUE)
    {
        CloseHandle(m_comPortHandles[port]);
    }
}

bool SerialConnection::WinSconnImpl::readByte(std::uint16_t port, byte& dst)
{
    HANDLE comport;
    if ((comport = m_comPortHandles[port]) != INVALID_HANDLE_VALUE)
    {
        if (rxReady(comport))
        {
            int readResult;
            ReadFile(comport, &dst, 1, (LPDWORD)((void*)&readResult), 0);
            return (readResult > 0);
        }
    }
    return false;
}

std::size_t SerialConnection::WinSconnImpl::readByteArray(std::uint16_t port, std::vector<byte>& dst)
{
    assert(dst.size() <= MAX_ARRAY_SIZE); //limited to 4kb
    
    HANDLE comport;
    if ((comport = m_comPortHandles[port]) != INVALID_HANDLE_VALUE)
    {
        if (rxReady(comport))
        {            
            std::size_t total = 0;
            std::memset(dst.data(), 0, dst.size()); //make sure values are all 0 so strings appear null terminated

            for (auto& b : dst)
            {
                std::size_t readSize = 0;
                ReadFile(comport, &b, 1, (LPDWORD)((void*)&readSize), 0);
                total += readSize;
                if (b == '\0') break; //reached end of string
                SLEEP(getDelay());
            }

            return total;
        }
    }
    return 0;
}

bool SerialConnection::WinSconnImpl::sendByte(std::uint16_t port, byte data)
{
    if (m_comPortHandles[port] != INVALID_HANDLE_VALUE)
    {
        int sendResult;
        WriteFile(m_comPortHandles[port], &data, 1, (LPDWORD)((void*)&sendResult), 0);
        return (sendResult > 0);
    }
    return false;
}

std::size_t SerialConnection::WinSconnImpl::sendByteArray(std::uint16_t port, const std::vector<byte>& data)
{
    assert(data.size() <= MAX_ARRAY_SIZE);
    HANDLE comport;
    if ((comport = m_comPortHandles[port]) != INVALID_HANDLE_VALUE)
    {
        //we send a byte at a time rather than the whole array as modern computers tend to send data too
        //fast for grbl to keep up
        std::size_t total = 0;
        for (auto& b : data)
        {
            std::size_t sendResult;
            WriteFile(comport, &b, 1, (LPDWORD)((void*)&sendResult), 0);
            total += sendResult;
            SLEEP(getDelay());
        }
        return total;
    }
    return 0;
}

//windows version works by querying the registry for port names
std::vector<std::string> SerialConnection::SconnImpl::getAvailablePorts()
{
    HKEY serialKey;
    std::vector<std::string> portNames;
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "HARDWARE\\DEVICEMAP\\SERIALCOMM", 0, KEY_QUERY_VALUE, &serialKey) == ERROR_SUCCESS)
    {
        KeyRelease kr(serialKey);

        unsigned long maxNameLength;
        unsigned long maxValueLength;
        auto queryInfo = RegQueryInfoKey(serialKey, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, &maxNameLength, &maxValueLength, nullptr, nullptr);

        if (queryInfo == ERROR_SUCCESS)
        {
            unsigned long nameSize = maxNameLength + 1;
            unsigned long valueSize = (maxValueLength / sizeof(TCHAR)) + 1;

            std::vector<char> nameData(nameSize * sizeof(TCHAR));
            std::memset(nameData.data(), 0, nameData.size());
            std::vector<unsigned char> valueData(maxValueLength);
            std::memset(valueData.data(), 0, valueData.size());

            int idx = 0;
            unsigned long type = 0;
            auto currentNameSize = nameSize;
            unsigned long currentDataSize = nameData.size();
            auto enumVal = RegEnumValue(serialKey, idx, static_cast<TCHAR*>(nameData.data()), &currentNameSize, nullptr, &type, static_cast<BYTE*>(valueData.data()), &currentDataSize);
            
            while (enumVal == ERROR_SUCCESS)
            {
                if (type == REG_SZ)
                {
                    //we have string value
                    portNames.emplace_back(reinterpret_cast<TCHAR*>(valueData.data()));
                    //TODO stash name string?
                }

                currentNameSize = nameSize;
                currentDataSize = nameData.size();
                std::memset(nameData.data(), 0, nameData.size());
                std::memset(valueData.data(), 0, valueData.size());
                idx++;
                enumVal = RegEnumValue(serialKey, idx, static_cast<TCHAR*>(nameData.data()), &currentNameSize, nullptr, &type, static_cast<BYTE*>(valueData.data()), &currentDataSize);
            }
        }
        else
        {
            //error
            std::cerr << "Failed to query COM registry info" << std::endl;
        }
    }

    return std::move(portNames);
}

#endif //_WIN32