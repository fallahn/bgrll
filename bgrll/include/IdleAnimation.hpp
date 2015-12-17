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

#ifndef IDLE_ANIMATION_HPP_
#define IDLE_ANIMATION_HPP_

#include <SFML/System/NonCopyable.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Vertex.hpp>

#include <memory>
#include <vector>

class IdleAnimation final : public sf::NonCopyable, public sf::Drawable
{
public:
    explicit IdleAnimation(sf::FloatRect bounds);
    ~IdleAnimation() = default;

    void update(float);

private:
    class LineSection final : public sf::NonCopyable, public sf::Drawable
    {
    public:
        LineSection(sf::Vector2f start, sf::Vector2f end);
        ~LineSection() = default;

        void update(float);
        bool dead() const;

    private:
        float m_lifetime;
        sf::Color m_colour;
        std::vector<sf::Vertex> m_vertices;

        void draw(sf::RenderTarget&, sf::RenderStates) const override;
    };
    std::vector<std::unique_ptr<LineSection>> m_lines;

    struct EndPoint
    {
        sf::Vector2f position;
        sf::Vector2f velocity;
    };
    EndPoint m_start, m_end;

    sf::FloatRect m_bounds;

    void updatePoint(EndPoint&, float);
    void draw(sf::RenderTarget&, sf::RenderStates) const override;
};

#endif //IDLE_ANIMATION_HPP_
