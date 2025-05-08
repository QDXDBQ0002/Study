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

void Input::KeyDown(unsigned int key)
{
    m_keys[key] = true;
}

void Input::KeyUp(unsigned int key)
{
    m_keys[key] = false;
}

bool Input::IsKeyDown(unsigned int key)
{
    return m_keys[key];
}

