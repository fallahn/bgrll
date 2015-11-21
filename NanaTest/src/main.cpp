#include <nana/gui/wvl.hpp>
#include <nana/gui/widgets/button.hpp>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

#include <Windows.h>

namespace
{
    sf::Uint8 c = 0u;
}

int main()
{
    nana::form form;
    form.caption("buns");

    sf::RenderWindow rw;
    rw.create((HWND)form.native_handle());

    form.events().destroy([&rw]() { rw.close(); });
    form.events().focus([&rw]() {rw.clear(sf::Color(50, 60, c++)); rw.display(); });

    //nana::button button(form, nana::rectangle(20, 20, 150, 30));
    //button.caption("flaps");
    //button.events().click(nana::API::exit);
    form.show();
    nana::exec();

    return 0;
}