#pragma once

namespace Engine
{
    class KeyboardInput final
    {
    public:
        void Update();

        bool IsDown(int virtualKey) const;
        bool WasPressed(int virtualKey) const;

    private:
        bool m_currentKeys[256] = {};
        bool m_previousKeys[256] = {};
    };
}