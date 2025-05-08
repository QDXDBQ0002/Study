#ifndef INPUT_H
#define INPUT_H

/*
    为了保持教程的简单，我暂时使用了windows输入，直到我做DirectInput的教程（DirectInput要优越得多）。
    input类处理来自键盘的用户输入。这个类的输入来自SystemClass::MessageHandler函数。输入对象将在键盘数组中存储每个键的状态。
    当被查询时，它会告诉调用函数是否按下了某个键。
*/
class Input
{
private:
    bool m_keys[256];
public:
    Input(/* args */);
    Input(const Input& other);
    ~Input();
    void Initialize();
    bool IsKeyDown(unsigned int key);
    void KeyDown(unsigned int key);
    void KeyUp(unsigned int key);
};



#endif

