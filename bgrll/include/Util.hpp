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
#if (_MSC_VER >= 1800)
#include <codecvt>
#else
//#include <boost/locale/encoding_utf.hpp>
#endif //_MSC_VER
#include <string>
#include <cassert>

#include <nana/config.hpp>

#ifndef NANA_UNICODE
#define STRU(x) x
#define STRD(x) x
#else
#define STRU(x) Util::String::toWideString(x)
#define STRD(x) Util::String::toNarrowString(x)
#endif //NANA_UNICODE

namespace Util
{
    namespace String
    {
 #if(_MSC_VER >= 1800)
        static inline std::wstring toWideString(const std::string& str)
        {
            assert(!str.empty());
            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
            return converter.from_bytes(str);
        }

        static inline std::string toNarrowString(const std::wstring& str)
        {
            assert(!str.empty());
            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
            return converter.to_bytes(str);
        }
#else //codecvt probably not supported
        //using boost::locale::conv::utf_to_utf;
        static inline std::wstring toWideString(const std::string& str)
        {
            return std::wstring();
            //return utf_to_utf<wchar_t>(str.c_str(), str.c_str() + str.size());
        }

        static inline std::string toNarrowString(const std::wstring& str)
        {
            return std::string();
            //return utf_to_utf<char>(str.c_str(), str.c_str() + str.size());
        }
#endif //_MSC_VER

        static inline std::int32_t extractNum(const std::string& str)
        {
            static std::string numbers = "0123456789";
            std::string retVal;

            std::size_t result = str.find_first_of(numbers.c_str());

            while (result != std::string::npos)
            {
                retVal.push_back(str[result]);
                std::size_t lastResult = result;
                result = str.find_first_of(numbers.c_str(), result + 1);
                if (result - lastResult > 1) break;
            }

            return std::atoi(retVal.c_str());
        }

        static inline std::int32_t extractNum(const std::wstring& str)
        {
            static std::wstring numbers = L"0123456789";
            std::wstring retVal;

            std::size_t result = str.find_first_of(numbers.c_str());

            while (result != std::string::npos)
            {
                retVal.push_back(str[result]);
                std::size_t lastResult = result;
                result = str.find_first_of(numbers.c_str(), result + 1);
                if (result - lastResult > 1) break;
            }

            return std::atoi(toNarrowString(retVal).c_str());
        }
    }
}

#endif //UTIL_FUNCS_HPP_
