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

#include <GCode.hpp>
#include <SerialConn.hpp>
#include <Util.hpp>

#include <iostream>
#include <fstream>
#include <cstring>
#include <algorithm>

#include <nana/gui/widgets/textbox.hpp>

namespace
{
    const std::uint8_t GRBL_MAX_BUFFER = 128u;
    const std::uint8_t GRBL_MAX_LINE_LENGTH = 70u;

    std::vector<byte> toDataArray(const std::string& str)
    {
        std::vector<byte> data(str.size());
        std::memcpy(data.data(), str.data(), str.size());
        return std::move(data);
    }
}

GCode::GCode(nana::textbox& tb)
    : m_textbox         (tb),
    m_front             (0u),
    m_back              (0u),
    m_sentBufferSize    (0u)
{

}

//public
void GCode::open(const std::string& path)
{
    close();

    std::ifstream file(path);
    if (file.good())
    {
        std::string line;
        while (std::getline(file, line))
        {
            m_lines.push_back(line);
        }

        validate();

        m_messages += "Opened " + path + "\n";
    }
    else
    {
        m_messages += "Failed to open " + path + "\n";
    }
}

void GCode::close()
{
    m_messages.clear();
    m_lines.clear();
    reset();
}

bool GCode::start(SerialConnection& connection, std::uint16_t port)
{
    if (!(m_front == 0 && m_back == 0) || m_lines.empty()) return false;

    //send as much as we can to fill the buffer
    do
    {
        m_textbox.append(STRU(m_lines[m_back]), true);
        m_sentBufferSize += connection.sendByteArray(port, toDataArray(m_lines[m_back++]));
    } while (m_sentBufferSize < GRBL_MAX_BUFFER && m_back < m_lines.size());
    return true;
}

bool GCode::update(SerialConnection& connection, std::uint16_t port)
{
    m_sentBufferSize -= m_lines[m_front++].size();
    if (GRBL_MAX_BUFFER - m_sentBufferSize > m_lines[m_back].size())
    {
        if (m_back < m_lines.size())
        {
            m_textbox.append(STRU(m_lines[m_back]), true);
            m_sentBufferSize += connection.sendByteArray(port, toDataArray(m_lines[m_back++]));
        }
        else
        {
            //return here to say we sent everything
            return true;
        }
    }
    return false;
}

void GCode::reset()
{
    m_front = 0u;
    m_back = 0u;
    m_sentBufferSize = 0u;
}

const std::string& GCode::getMessages() const
{
    return m_messages;
}

//private
void GCode::validate()
{
    //remove comments / invalid lines
    m_lines.erase(std::remove_if(m_lines.begin(), m_lines.end(), [](const std::string& str)
    {
        return (str[0] != 'N' && str[0] != 'G' && str[0] != 'M');
    }), m_lines.end());

    //validate lengths
    for (auto i = 0u; i < m_lines.size(); ++i)
    {
        m_lines[i] += "\n";
        if (m_lines[i].size() >= GRBL_MAX_LINE_LENGTH)
        {
            m_messages += "WARNING: line " + std::to_string(i) + " exceeds GRBL max line length\n";
        }
    }
}
