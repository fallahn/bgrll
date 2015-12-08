#include <nana/gui/wvl.hpp>

#include <SFML/Graphics/RenderWindow.hpp>

int main()
{   
    nana::form form;
    form.caption("buns");

    sf::RenderWindow rw;
    rw.create(reinterpret_cast<sf::WindowHandle>(form.native_handle()));

    form.events().destroy([&rw]() { rw.close(); });
    form.events().focus([&rw]() {rw.clear(sf::Color(50, 60, 50)); rw.display(); });

    form.show();
    nana::exec();

    return 0;
}
