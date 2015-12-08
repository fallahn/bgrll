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

//contains the currently loaded g-code as a series of strings

#ifndef GCODE_HPP_
#define GCODE_HPP_

#include <string>
#include <vector>

class SerialConnection;
class GCode final
{
public:
    GCode();
    ~GCode() = default;
    GCode(const GCode&) = delete;
    const GCode& operator = (const GCode&) = delete;

    void open(const std::string& path);
    void close();

    void update(SerialConnection&);

    const std::string& getMessages() const;

private:

    std::uint16_t m_front;
    std::uint16_t m_back;
    std::string m_messages;

    std::vector<std::string> m_lines;

    void validate(); //check line lengths, strip comments atc
};

#endif //GCODE_HPP_
