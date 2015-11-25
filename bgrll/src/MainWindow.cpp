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

namespace
{

}

MainWindow::MainWindow()
    : nana::form(nana::API::make_center(1024, 768), nana::appear::decorate<nana::appear::taskbar>())
{
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
    m_serialInputLabel.create(*this, { 516, 26, 64, 20 });
    m_serialInputLabel.caption(L"Command:");
    
    m_serialInputTextBox.create(*this, { 580, 24, 300, 20 });
    m_serialInputTextBox.multi_lines(false);
    m_serialInputTextBox.line_wrapped(false);

    m_serialInputButton.create(*this, nana::rectangle(900, 24, 100, 20));
    m_serialInputButton.caption(L"Send Command");

    m_serialOutputTextBox.create(*this, { 516, 56, 498, 400 });
    m_serialOutputTextBox.multi_lines(true);
    m_serialOutputTextBox.line_wrapped(false);
    m_serialOutputTextBox.editable(false);

    nana::string;
}