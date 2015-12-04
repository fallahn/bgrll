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
    std::size_t serialInputBufferSize = 1024;
    std::int16_t noPort = -1;
}

MainWindow::MainWindow()
    : nana::form        (nana::API::make_center(1024, 768), nana::appear::decorate<nana::appear::taskbar>()),
    m_runSerialThread   (false),
    m_currentPort       (noPort),
    m_previewWindowForm (*this, { 10, 26, 504, 470 }, {false, false, false, false, false, false, false})
{
    buildMenuBar();
    buildComInterface();

    m_previewWindowForm.bgcolor({ 0, 0, 0 });
    m_previewWindowForm.show();

    //launch SFML in own thread
    auto previewThread = nana::threads::pool_push(m_threadPool, [this]()
    {
        m_previewWindow.start(m_previewWindowForm);
    });
    previewThread();
}

MainWindow::~MainWindow()
{
    m_runSerialThread = false;
    m_previewWindow.end();
    m_threadPool.wait_for_finished();
}

//public



//private
void MainWindow::buildMenuBar()
{
    m_menuBar.create(*this);
    auto& fileMenu = m_menuBar.push_back(STR("File"));
    fileMenu.append(STR("Open .nc File"), [this](nana::menu::item_proxy&) {});
    fileMenu.append(STR("Close File"), [this](nana::menu::item_proxy&) {});
    fileMenu.append(STR("Exit"), [this](nana::menu::item_proxy&) { close(); });
    
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
    m_comportLabel.create(*this, { 526, 26, 64, 20 });
    m_comportLabel.caption(STR("Port:"));

    m_comportDropdown.create(*this, { 590, 24, 140, 20 });
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

    m_baudrateDropdown.create(*this, { 750, 24, 140, 20 });
    m_baudrateDropdown.push_back(STR("96000"));
    m_baudrateDropdown.push_back(STR("115200"));
    m_baudrateDropdown.option(0);
    
    m_connectButton.create(*this, nana::rectangle(900, 24, 114, 20));
    m_connectButton.caption(STR("Connect"));
    m_connectButton.events().click([this]()
    {
        if (m_currentPort == -1)
        {
            m_connectButton.caption(STR("Disconnect"));
            m_comportDropdown.enabled(false);
            m_baudrateDropdown.enabled(false);

            m_serialOutputTextBox.append(STR("Connecting...\n"), true);

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

    m_serialInputLabel.create(*this, { 526, 66, 64, 20 });
    m_serialInputLabel.caption(STR("Command:"));
    
    m_serialInputTextBox.create(*this, { 590, 64, 300, 20 });
    m_serialInputTextBox.multi_lines(false);
    m_serialInputTextBox.line_wrapped(false);

    m_serialInputButton.create(*this, nana::rectangle(900, 64, 114, 20));
    m_serialInputButton.caption(STR("Send Command"));
    m_serialInputButton.events().click([this]()
    {
        if (m_currentPort != noPort)
        {
            nana::string text;
            m_serialInputTextBox.getline(0, text);

            if (text.empty()) return;
            text += L"\r\n";

            std::string ntext = STRD(text);
            std::vector<byte> data(ntext.size());
            std::memcpy(data.data(), ntext.data(), ntext.size());
            auto sent = m_serialConnection.sendByteArray(m_currentPort, data);
            if (sent > 0)
            {
                m_serialOutputTextBox.append(text, true);
            }
            else
            {
                m_serialOutputTextBox.append(STR("Failed sending command\n"), true);
            }
            m_serialInputTextBox.reset();
        }    
    });

    m_serialOutputTextBox.create(*this, { 526, 96, 488, 400 });
    m_serialOutputTextBox.multi_lines(true);
    m_serialOutputTextBox.line_wrapped(false);
    m_serialOutputTextBox.editable(false);
    m_serialOutputTextBox.append(L"Disconnected...\n", true);

    nana::string;
}

void MainWindow::serialThreadFunc()
{
    //as far as I can tell nana promises to sync access properly
    //here goes....

    //TODO this won't work on linux as port names also include information such as whether or not they are USB ports
    m_currentPort = static_cast<std::uint16_t>(Util::String::extractNum(m_comportDropdown.text(m_comportDropdown.option())));
    std::uint32_t currentBaud = Util::String::extractNum(m_baudrateDropdown.text(m_baudrateDropdown.option()));

    if (!m_serialConnection.openPort(m_currentPort, currentBaud))
    {
        m_serialConnection.closePort(m_currentPort);
        m_serialOutputTextBox.append(STR("\nFailed to connect to selected COM port"), true);
        m_connectButton.caption(STR("Connect"));
        m_comportDropdown.enabled(true);
        m_baudrateDropdown.enabled(true);

        m_runSerialThread = false;
        m_currentPort = noPort;
        return;
    }
    
    std::vector<byte> input(serialInputBufferSize);
    m_serialConnection.readByteArray(m_currentPort, input);
    printToConsole(input);
    input.resize(serialInputBufferSize);

    std::vector<byte> send = { '$', '\r', '\n', '\0' }; //queries grbl for current settings
    m_serialConnection.sendByteArray(m_currentPort, send);
    printToConsole(send);

    //poll for input from serial port
    while (m_runSerialThread)
    {
        auto rxd = m_serialConnection.readByteArray(m_currentPort, input);
        if (rxd > 0)
        {
            printToConsole(input);
            input.resize(serialInputBufferSize);
        }
        SLEEP(500);
    }

    m_serialConnection.closePort(m_currentPort);
    m_currentPort = noPort;

    m_serialOutputTextBox.append(STR("\nDisconnected...\n"), true);
}

void MainWindow::printToConsole(const std::vector<byte>& str)
{
    std::string txt(reinterpret_cast<const char*>(str.data()));
    m_serialOutputTextBox.append(STRU(txt), true);
}