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

#ifndef DRAWING_UTIL_HPP_
#define DRAWING_UTIL_HPP_

#include <SFML/System/Vector2.hpp>

#include <cassert>
#include <cmath>
#include <ctime>
#include <random>

namespace Util
{
    namespace Vector
    {
        static inline float dot(const sf::Vector2f& lv, const sf::Vector2f& rv)
        {
            return lv.x * rv.x + lv.y * rv.y;
        }

        static inline sf::Vector2f normalise(sf::Vector2f source)
        {
            float length = std::sqrt(dot(source, source));
            if (length != 0) source /= length;
            return source;
        }

        static inline sf::Vector2f reflect(const sf::Vector2f& velocity, const sf::Vector2f& normal)
        {
            return -2.f * dot(velocity, normal) * normal + velocity;
        }
    }

    namespace Random
    {
        static std::default_random_engine rndEngine(static_cast<unsigned long>(std::time(0)));
        static inline float value(float begin, float end)
        {
            assert(begin < end);
            std::uniform_real_distribution<float> dist(begin, end);
            return dist(rndEngine);
        }

        static inline int value(int begin, int end)
        {
            assert(begin < end);
            std::uniform_int_distribution<int> dist(begin, end);
            return dist(rndEngine);
        }
    }
}

#endif