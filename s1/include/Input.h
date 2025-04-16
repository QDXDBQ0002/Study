#ifndef _INPUTCLASS_H_
#define _INPUTCLASS_H_

class Input
{
    private:
        bool m_keys[256];
    public:
        Input();
        Input(const Input&);
        ~Input();
        void Initialize();
        void KeyDown(unsigned int);
        void KeyUp(unsigned int);
        bool IsKeyDown(unsigned int);
};

#endif