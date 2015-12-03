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
#include <Util.hpp>

#include <iostream>

namespace
{
    std::size_t serialInputBufferSize = 200;
}

MainWindow::MainWindow()
    : nana::form        (nana::API::make_center(1024, 768), nana::appear::decorate<nana::appear::taskbar>()),
    m_runSerialThread   (false)
{
    buildMenuBar();
    buildComInterface();
}

MainWindow::~MainWindow()
{
    m_runSerialThread = false;
    m_threadPool.wait_for_finished();
}

//public



//private
void MainWindow::buildMenuBar()
{
    m_menuBar.create(*this);
    auto& fileMenu = m_menuBar.push_back(STR("File"));
    fileMenu.append(STR("exit"), [this](nana::menu::item_proxy&) { close(); });
    auto& toolsMenu = m_menuBar.push_back(STR("Tools"));
    toolsMenu.append(STR("Options"), [](nana::menu::item_proxy&)
    {
        //TODO make this an options window
        nana::form optionsWindow;
        optionsWindow.modality();
    });
    auto& helpMenu = m_menuBar.push_back(STR("Help"));
    helpMenu.append(STR("About"), [](nana::menu::item_proxy&)
    {
        //TODO make this an about box
        nana::form aboutWindow;
        aboutWindow.modality();
    });
}

void MainWindow::buildComInterface()
{
    m_comportLabel.create(*this, { 516, 26, 64, 20 });
    m_comportLabel.caption(STR("Port:"));

    m_comportDropdown.create(*this, { 580, 24, 140, 20 });
    auto ports = m_serialConnection.getAvailablePorts();
    if (!ports.empty())
    {
        for (const auto& p : ports)
        {
            m_comportDropdown.push_back(STRU(p));
        }
    }
    else
    {
        m_comportDropdown.push_back(STR("No COM ports available"));
    }
    m_comportDropdown.option(0);

    m_baudrateDropdown.create(*this, { 740, 24, 140, 20 });
    m_baudrateDropdown.push_back(STR("96000"));
    m_baudrateDropdown.push_back(STR("115200"));
    m_baudrateDropdown.option(0);
    
    m_connectButton.create(*this, nana::rectangle(900, 24, 114, 20));
    m_connectButton.caption(STR("Connect"));
    m_connectButton.events().click([this]()
    {
        if (m_connectButton.caption() == STR("Connect"))
        {
            m_connectButton.caption(STR("Disconnect"));
            m_comportDropdown.enabled(false);
            m_baudrateDropdown.enabled(false);

            m_runSerialThread = true;
            std::function<void()> serialFunc = std::bind(&MainWindow::serialThreadFunc, this);
            nana::threads::pool_push(m_threadPool, serialFunc)();
        }
        else
        {
            m_connectButton.caption(STR("Connect"));
            m_comportDropdown.enabled(true);
            m_baudrateDropdown.enabled(true);

            m_runSerialThread = false;
        }
    });

    //--------------------------------------------------//

    m_serialInputLabel.create(*this, { 516, 66, 64, 20 });
    m_serialInputLabel.caption(STR("Command:"));
    
    m_serialInputTextBox.create(*this, { 580, 64, 300, 20 });
    m_serialInputTextBox.multi_lines(false);
    m_serialInputTextBox.line_wrapped(false);

    m_serialInputButton.create(*this, nana::rectangle(900, 64, 114, 20));
    m_serialInputButton.caption(STR("Send Command"));
    m_serialInputButton.events().click([]() {});

    m_serialOutputTextBox.create(*this, { 516, 96, 498, 400 });
    m_serialOutputTextBox.multi_lines(true);
    m_serialOutputTextBox.line_wrapped(false);
    m_serialOutputTextBox.editable(false);

    nana::string;
}

void MainWindow::serialThreadFunc()
{
    //as far as I can tell nana promises to sync access properly
    //here goes....

    std::uint16_t currentPort = static_cast<std::uint16_t>(Util::String::extractNum(m_comportDropdown.text(m_comportDropdown.option())));
    std::uint32_t currentBaud = Util::String::extractNum(m_baudrateDropdown.text(m_baudrateDropdown.option()));

    if (!m_serialConnection.openPort(currentPort, currentBaud))
    {
        m_serialConnection.closePort(currentPort);
        m_serialOutputTextBox.append(STR("\nFailed to connect to selected COM port"), true);
        m_connectButton.caption(STR("Connect"));
        m_comportDropdown.enabled(true);
        m_baudrateDropdown.enabled(true);

        m_runSerialThread = false;
        return;
    }
    
    std::vector<byte> input(serialInputBufferSize);
    m_serialConnection.readByteArray(currentPort, input);
    std::string rxTxt(reinterpret_cast<char*>(input.data()));
    m_serialOutputTextBox.append(STRU(rxTxt), true);
    input.resize(serialInputBufferSize);

    std::vector<byte> send = { '$', '\r', '\n' }; //queries grbl for current settings
    auto sent = m_serialConnection.sendByteArray(currentPort, send);

    //poll for input from serial port - TODO serial needs to have a timeout here
    while (m_runSerialThread)
    {
        m_serialConnection.readByteArray(currentPort, input);
        std::string rxTxt(reinterpret_cast<char*>(input.data()));
        m_serialOutputTextBox.append(STRU(rxTxt), true);

        input.resize(serialInputBufferSize);

        SLEEP(500);
    }

    m_serialConnection.closePort(currentPort);
}