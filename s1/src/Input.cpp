#include "Input.h"

Input::Input()
{
}

Input::Input(const Input& other)
{    
}

Input::~Input()
{
}

void Input::Initialize()
{
    for(int i = 0; i < 256; i++)
    {
        m_keys[i] = false;
    }
}

void Input::KeyDown(unsigned int input)
{
    m_keys[input] = true;
}

void Input::KeyUp(unsigned int input)
{
    m_keys[input] = false;
}

bool Input::IsKeyDown(unsigned int input)
{
    return m_keys[input];
}
