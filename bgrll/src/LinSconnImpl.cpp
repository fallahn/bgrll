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

}

bool SerialConnection::LinSconnImpl::openPort(std::uint16_t port, std::uint32_t baud)
{
    return false;
}

void SerialConnection::LinSconnImpl::closePort(std::uint16_t port)
{

}

bool SerialConnection::LinSconnImpl::readByte(std::uint16_t port, byte& dst)
{
    return false;
}

bool SerialConnection::LinSconnImpl::readByteArray(std::uint16_t port, byte* dst, std::size_t count)
{
    return false;
}

bool SerialConnection::LinSconnImpl::sendByte(std::uint16_t port, byte data)
{
    return false;
}

bool SerialConnection::LinSconnImpl::sendByteArray(std::uint16_t port, const std::vector<byte>& data)
{
    return false;
}
#endif //__LINUX__