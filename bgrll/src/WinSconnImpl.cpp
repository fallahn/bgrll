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

#include <SerialConn.hpp>

#ifdef _WIN32

namespace
{
    const char comPorts[16][10] = 
    {
        "\\\\.\\COM0",  "\\\\.\\COM1",  "\\\\.\\COM2",  "\\\\.\\COM3",
        "\\\\.\\COM4",  "\\\\.\\COM5",  "\\\\.\\COM6",  "\\\\.\\COM7",
        "\\\\.\\COM8",  "\\\\.\\COM9",  "\\\\.\\COM10", "\\\\.\\COM11",
        "\\\\.\\COM12", "\\\\.\\COM13", "\\\\.\\COM14", "\\\\.\\COM15" 
    };

}

SerialConnection::WinSconnImpl::~WinSconnImpl()
{
    for (auto& h : m_comportHandles)
    {
        if (h != INVALID_HANDLE_VALUE)
        {
            CloseHandle(h);
        }
    }
}

bool SerialConnection::WinSconnImpl::openPort(std::uint16_t port, std::uint32_t baud)
{
    return false;
}

void SerialConnection::WinSconnImpl::closePort(std::uint16_t port)
{

}

bool SerialConnection::WinSconnImpl::readByte(std::uint16_t port, byte& dst)
{
    return false;
}

bool SerialConnection::WinSconnImpl::readByteArray(std::uint16_t port, byte* dst, std::size_t count)
{
    return false;
}

bool SerialConnection::WinSconnImpl::sendByte(std::uint16_t port, byte data)
{
    return false;
}

bool SerialConnection::WinSconnImpl::sendByteArray(std::uint16_t port, const std::vector<byte>& data)
{
    return false;
}

#endif //_WIN32