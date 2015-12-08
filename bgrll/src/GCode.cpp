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

#include <iostream>
#include <fstream>

namespace 
{
    const std::uint8_t GRBL_MAX_BUFFER = 128u;
}

GCode::GCode()
    : m_front   (0u),
    m_back      (0u)
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
    m_front = 0u;
    m_back = 0u;
}

void GCode::update(SerialConnection& connection)
{

}

const std::string& GCode::getMessages() const
{
    return m_messages;
}

//private
void GCode::validate()
{
    //remove comments
    //warn on lines which are too long
}