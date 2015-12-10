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

#include <PreviewWindow.hpp>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/System/Clock.hpp>

#include <nana/gui/wvl.hpp>


PreviewWindow::PreviewWindow()
    : m_running(false)
{}

//public
void PreviewWindow::start(const nana::nested_form& parent)
{
    sf::RenderWindow rw;
    auto handle = parent.native_handle();
    rw.create((sf::WindowHandle)handle);
    rw.setFramerateLimit(60);

    m_idleAnimation = std::make_unique<IdleAnimation>(sf::FloatRect(sf::Vector2f(), rw.getView().getSize()));

    sf::Clock frameClock;
    const float timePerFrame = 1.f / 60.f;
    float timeSinceLastUpdate = 0.f;

    m_running = true;
    while (m_running)
    {
        float elapsedTime = frameClock.restart().asSeconds();
        timeSinceLastUpdate += elapsedTime;

        while (timeSinceLastUpdate > timePerFrame)
        {
            timeSinceLastUpdate -= timePerFrame;

            handleEvents(rw);
            update(timePerFrame);
        }
        draw(rw);
    }

    rw.close();
}

void PreviewWindow::end()
{
    m_running = false;
}

//private
void PreviewWindow::handleEvents(sf::RenderWindow& rw)
{
    sf::Event evt;
    while (rw.pollEvent(evt))
    {
        if (evt.type == sf::Event::Closed) rw.close();
    }
}

void PreviewWindow::update(float dt)
{
    m_idleAnimation->update(dt);
}

void PreviewWindow::draw(sf::RenderWindow& rw)
{
    rw.clear();

    rw.draw(*m_idleAnimation);

    rw.display();
}
