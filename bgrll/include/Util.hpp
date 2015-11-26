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

// nana wants to use wstring *sigh* on windows and string on everything else
// so here are some utilities to help us cope with that

#ifndef UTIL_FUNCS_HPP_
#define UTIL_FUNCS_HPP_

#include <locale>
#include <codecvt>
#include <string>

#include <nana/config.hpp>

#ifndef NANA_UNICODE
#define STRU(x) x
#else
#define STRU(x) Util::String::toWideString(x)
#endif //NANA_UNICODE

namespace Util
{
    namespace String
    {
        static inline std::wstring toWideString(const std::string& str)
        {
            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
            return converter.from_bytes(str);
        }

        static inline std::string toNarrowString(const std::wstring& str)
        {
            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
            return converter.to_bytes(str);
        }
    }
}

#endif //UTIL_FUNCS_HPP_
