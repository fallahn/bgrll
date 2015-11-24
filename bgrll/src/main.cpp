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

#include <nana/gui/wvl.hpp>

#include <SerialConn.hpp>

#include <iostream>

int main()
{
    SerialConnection sc;
    auto ports = sc.getAvailablePorts();
    for(const auto& p : ports) std::cout << p << std::endl;

    if (ports[0] == "COM3")
    {
        if (sc.openPort(3, 115200))
        {
            std::cout << "Opened COM3" << std::endl;

            std::vector<byte> rcd(2000);
            auto count = sc.readByteArray(3, rcd);
            std::cout << count << " bytes received" << std::endl;
            std::cout << reinterpret_cast<char*>(rcd.data()) << std::endl;

            std::vector<byte> send = { '$', '\r', '\n' };
            count = sc.sendByteArray(3, send);
            std::cout << count << " bytes sent" << std::endl;

            count = sc.readByteArray(3, rcd);
            std::cout << count << " bytes received" << std::endl;
            std::cout << reinterpret_cast<char*>(rcd.data()) << std::endl;
        }
    }


    nana::form form;
    form.caption("buns");

    form.show();
    nana::exec();

    return 0;
}