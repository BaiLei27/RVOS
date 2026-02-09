#include "Gui/RISCVApplication.hh"

int main(int argc, char *argv[])
{
    auto app= RISCVApplication::Create();
    return app->run(argc, argv);
}