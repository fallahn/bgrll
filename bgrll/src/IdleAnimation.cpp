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

#include <IdleAnimation.hpp>
#include <DrawingUtil.hpp>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/System/Clock.hpp>

namespace
{
    const float speed = 200.f;
    const float spawnFrequency = 0.1f;
}

IdleAnimation::IdleAnimation(sf::FloatRect bounds)
    : m_bounds(bounds)
{
    m_start.position = { Util::Random::value(0.f, bounds.width), Util::Random::value(0.f, bounds.height) };
    m_start.velocity = { Util::Random::value(-bounds.width, bounds.width), Util::Random::value(-bounds.height, bounds.height) };
    m_start.velocity = Util::Vector::normalise(m_start.velocity);

    m_end.position = { Util::Random::value(0.f, bounds.width), Util::Random::value(0.f, bounds.height) };
    m_end.velocity = { Util::Random::value(-bounds.width, bounds.width), Util::Random::value(-bounds.height, bounds.height) };
    m_end.velocity = Util::Vector::normalise(m_start.velocity);
}

//public
void IdleAnimation::update(float dt)
{
    m_lines.erase(std::remove_if(m_lines.begin(), m_lines.end(), 
        [](const std::unique_ptr<LineSection>& line)
    {
        return line->dead(); 
    }), m_lines.end());
    
    for (auto& l : m_lines)
    {
        l->update(dt);
    }

    updatePoint(m_start, dt);
    updatePoint(m_end, dt);

    static sf::Clock timer;
    if (timer.getElapsedTime().asSeconds() > spawnFrequency)
    {
        timer.restart();
        m_lines.emplace_back(std::make_unique<LineSection>(m_start.position, m_end.position));
    }
}

//private
void IdleAnimation::updatePoint(EndPoint& point, float dt)
{
    point.position += point.velocity * speed * dt;

    if (point.position.x < 0 || point.position.x > m_bounds.width)
    {
        point.velocity = Util::Vector::reflect(point.velocity, { 1.f, 0.f });
    }
    else if (point.position.y < 0 || point.position.y > m_bounds.height)
    {
        point.velocity = Util::Vector::reflect(point.velocity, { 0.f, 1.f });
    }
}

void IdleAnimation::draw(sf::RenderTarget& rt, sf::RenderStates states) const
{
    for (auto& l : m_lines)
    {
        rt.draw(*l);
    }
    
    std::vector<sf::Vertex> verts = { {m_start.position, sf::Color::Green}, {m_end.position, sf::Color::Green} };
    rt.draw(verts.data(), verts.size(), sf::PrimitiveType::Lines);
}

//------------------LineSection----------------//

namespace
{
    const float lifetime = 1.5f;
}

IdleAnimation::LineSection::LineSection(sf::Vector2f start, sf::Vector2f end)
    : m_lifetime    (lifetime),
    m_colour        (sf::Color::Green)
{
    m_vertices.emplace_back(sf::Vertex(start));
    m_vertices.emplace_back(sf::Vertex(end));
}

//public
void IdleAnimation::LineSection::update(float dt)
{
    m_lifetime -= dt;
    float alpha = 255.f * std::max(0.f, m_lifetime / lifetime);
    m_colour.a = static_cast<sf::Uint8>(alpha);

    m_vertices[0].color = m_vertices[1].color = m_colour;
}

bool IdleAnimation::LineSection::dead() const
{
    return !(m_colour.a > 0);
}

//private
void IdleAnimation::LineSection::draw(sf::RenderTarget& rt, sf::RenderStates states) const
{
    rt.draw(m_vertices.data(), m_vertices.size(), sf::PrimitiveType::Lines);
}