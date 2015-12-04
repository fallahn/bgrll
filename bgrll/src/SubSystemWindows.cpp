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

//allows using a WinMain entry point to remove the console window on windows
//platforms. To enable define SUBSYSTEM_WINDOWS and set the subsystem settings
//under linker system options to to Windows in Visual Studio, or set the linker
//flag -Wl,--subsystem,windows in gcc/MinGW

#ifdef _WIN32
#ifdef SUBSYSTEM_WINDOWS

#include <Windows.h>

extern int main(int argc, char* argv[]);

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, INT)
{
    return main(__argc, __argv);
}

#endif //SUBSYSTEM_WINDOWS
#endif //_WIN32