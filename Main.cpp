#include <iostream>

#include "Core/Application.h"

int main(void)
{
    Application App;

    int success = App.Initialize();
    if (success == -1)
    {
        std::cout << "Error Initializing Application";
        return -1;
    }

    App.Run();

    App.Release();
    return 0;
}