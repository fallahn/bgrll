#include <nana/gui/wvl.hpp>

int main()
{
    nana::form form;
    form.caption("buns");

    form.show();
    nana::exec();

    return 0;
}