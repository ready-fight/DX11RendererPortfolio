#include "Engine/Input/KeyboardInput.h"

#include <Windows.h>

namespace Engine
{
    void KeyboardInput::Update()
    {
        for (int i = 0; i < 256; ++i)
        {
            m_previousKeys[i] = m_currentKeys[i];
            m_currentKeys[i] = (GetAsyncKeyState(i) & 0x8000) != 0;
        }
    }

    bool KeyboardInput::IsDown(int virtualKey) const
    {
        return m_currentKeys[virtualKey];
    }

    bool KeyboardInput::WasPressed(int virtualKey) const
    {
        return m_currentKeys[virtualKey] && !m_previousKeys[virtualKey];
    }
}