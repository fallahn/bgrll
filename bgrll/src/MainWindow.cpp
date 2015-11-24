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

#include <MainWindow.hpp>

#include <nana/gui/widgets/button.hpp>

namespace
{

}

MainWindow::MainWindow()
    : nana::form(nana::API::make_center(1024, 768), nana::appear::decorate<nana::appear::taskbar>())
{
    //left group contains visualisation
    m_leftGroup.create(*this, { 0, 24, 512, 744 });

    //right group contains serial console and jog controls
    m_rightGroup.create(*this, { 512, 24, 512, 744 });

    buildMenuBar();
    buildComInterface();
}

//public



//private
void MainWindow::buildMenuBar()
{
    m_menuBar.create(*this);
    auto& fileMenu = m_menuBar.push_back(L"File");
    fileMenu.append(L"exit", [this](nana::menu::item_proxy&) { close(); });
    auto& toolsMenu = m_menuBar.push_back(L"Tools");
    toolsMenu.append(L"Options", [](nana::menu::item_proxy&)
    {
        //TODO make this an options window
        nana::form optionsWindow;
        optionsWindow.modality();
    });
    auto& helpMenu = m_menuBar.push_back(L"Help");
    helpMenu.append(L"About", [](nana::menu::item_proxy&)
    {
        //TODO make this an about box
        nana::form aboutWindow;
        aboutWindow.modality();
    });
}

void MainWindow::buildComInterface()
{
    m_serialInput.create(m_rightGroup, { 0, 0, 50, 20 });
    //m_serialInput.caption(L"Input:");
    m_serialInput.move({ 516, 24 });
    m_rightGroup["serialInput"] << m_serialInput;
 
    nana::button button(m_rightGroup);
    button.size({ 50, 50 });
    button.caption(L"buns");
    m_rightGroup["button"] << button;
    m_rightGroup.collocate();
    m_rightGroup.caption(L"flaps");
}