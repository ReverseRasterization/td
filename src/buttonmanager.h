#pragma once

#include "ui/button.h"

class ButtonManager
{
    public:
        struct ButtonWrapper
        {
            Button& button;
            std::function<void(Button& button)> onClick;
        }

    private:

}