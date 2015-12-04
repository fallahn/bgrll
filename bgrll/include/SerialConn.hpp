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

//provides cross platform serial connection capabilities

#ifndef SERIAL_CONN_HPP_
#define SERIAL_CONN_HPP_

#include <memory>
#include <vector>
#include <array>
#include <string>

#ifdef _WIN32
#include <Windows.h>
#define WIN32_LEAN_AND_MEAN
#define SLEEP(x) Sleep(x)
#elif defined __LINUX__
#include <cstdio>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#define SLEEP(x) usleep(1000 * x)
#endif

using byte = unsigned char;

class SerialConnection final
{
public:
    SerialConnection()
    {
#ifdef __LINUX__
        m_impl = std::make_unique<LinSconnImpl>();
#elif defined(_WIN32)
        m_impl = std::make_unique<WinSconnImpl>();
#endif //__LINUX__
    }

    ~SerialConnection() = default;

    //attempts to open given port at given baud. returns false
    //if fails
    bool openPort(std::uint16_t port, std::uint32_t baud)
    {
        return m_impl->openPort(port, baud);
    }

    //returns true if given port is currently open, else
    //returns false
    bool portOpened(std::uint16_t port) const
    {
        return m_impl->portOpened(port);
    }

    //closes given port if it is open
    void closePort(std::uint16_t port)
    {
        m_impl->closePort(port);
    }

    //attempts to read a byte from the given port. returns true
    //if successful else false if fails (port not open for example)
    //retrieved data is stored in dst. this function is *blocking*
    bool readByte(std::uint16_t port, byte& dst)
    {
         return m_impl->readByte(port, dst);
    }

    //attempts to read an array of bytes into dst, up to current size
    //returns actual number of bytes read. function is *blocking*
    std::size_t readByteArray(std::uint16_t port, std::vector<byte>& dst)
    {
        return m_impl->readByteArray(port, dst);
    }

    //attempt to send byte of data on given port if it is open
    //returns true on success, else returns false
    bool sendByte(std::uint16_t port, byte data)
    {
        return m_impl->sendByte(port, data);
    }

    //attempts to send the given array of data via the port if it is
    //open. returns number of bytes successfully sent. this function is *blocking*
    std::size_t sendByteArray(std::uint16_t port, const std::vector<byte>& data)
    {
        return m_impl->sendByteArray(port, data);
    }

    //returns a vector of strings containing the names of availble COM ports on the current system
    std::vector<std::string> getAvailablePorts() const
    {
        return std::move(SconnImpl::getAvailablePorts());
    }

    //sets a delay in milliseconds when sending or receiving each byte as PCs are generally too fast
    //for a grbl controller (default is 10 milliseconds)
    void setDelay(std::uint16_t delay)
    {
        m_impl->setDelay(delay);
    }

private:
    class SconnImpl
    {
    public:
        SconnImpl() = default;
        virtual ~SconnImpl() = default;

        virtual bool openPort(std::uint16_t port, std::uint32_t baud) = 0;
        virtual bool portOpened(std::uint16_t port) const = 0;
        virtual void closePort(std::uint16_t port) = 0;

        virtual bool readByte(std::uint16_t port, byte& dst) = 0;
        virtual std::size_t readByteArray(std::uint16_t port, std::vector<byte>& dst) = 0;

        virtual bool sendByte(std::uint16_t port, byte data) = 0;
        virtual std::size_t sendByteArray(std::uint16_t port, const std::vector<byte>& data) = 0;

        static std::vector<std::string> getAvailablePorts();

        void setDelay(std::uint16_t delay)
        {
            m_delay = delay;
        }

        std::uint16_t getDelay() const
        {
            return m_delay;
        }
    private:
        std::uint16_t m_delay = 10u;
    };


#ifdef _WIN32

    class WinSconnImpl final : public SconnImpl
    {
    public:
        WinSconnImpl();
        ~WinSconnImpl();

        bool openPort(std::uint16_t port, std::uint32_t baud) override;
        bool portOpened(std::uint16_t port) const override;
        void closePort(std::uint16_t port) override;

        bool readByte(std::uint16_t port, byte& dst) override;
        std::size_t readByteArray(std::uint16_t port, std::vector<byte>& dst)override;

        bool sendByte(std::uint16_t port, byte data) override;
        std::size_t sendByteArray(std::uint16_t port, const std::vector<byte>& data) override;

    private:

        std::array<HANDLE, 16> m_comPortHandles;
        DCB m_portSettings;
        COMMTIMEOUTS m_comTimeouts;
        std::string m_baudrate;
    };
#elif defined __LINUX__
    class LinSconnImpl final : public SconnImpl
    {
    public:
        LinSconnImpl();
        ~LinSconnImpl() = default;

        bool openPort(std::uint16_t port, std::uint32_t baud) override;
        bool portOpened(std::uint16_t port) const override;
        void closePort(std::uint16_t port) override;

        bool readByte(std::uint16_t port, byte& dst) override;
        std::size_t readByteArray(std::uint16_t port, std::vector<byte>& dst)override;

        bool sendByte(std::uint16_t port, byte data) override;
        std::size_t sendByteArray(std::uint16_t port, const std::vector<byte>& data) override;

    private:

        std::array<int, 30> m_comPortHandles;
        int m_error;
        termios m_newPortSettings;
        std::array<termios, 30> m_oldPortSettings;
    };
#endif //_WIN32
    std::unique_ptr<SconnImpl> m_impl;

};


#endif //SERIAL_CONN_HPP_