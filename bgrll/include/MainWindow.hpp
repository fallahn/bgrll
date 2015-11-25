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

#ifndef MAIN_WINDOW_HPP_
#define MAIN_WINDOW_HPP_

#include <nana/gui/wvl.hpp>
#include <nana/gui/widgets/menubar.hpp>
#include <nana/gui/widgets/textbox.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/label.hpp>

class MainWindow final : public nana::form
{
public:
    MainWindow();
    ~MainWindow() = default;

private:

    nana::menubar m_menuBar;

    nana::label m_serialInputLabel;
    nana::textbox m_serialInputTextBox;
    nana::textbox m_serialOutputTextBox;
    nana::button m_serialInputButton;

    void buildMenuBar();
    void buildComInterface(); //commandline input and output window from RS232
};

#endif //MAIN_WINDOW_HPP_