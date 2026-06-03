#include "Engine/Core/Timer.h"

namespace Engine {

  Timer::Timer() {
    m_startTime = Clock::now();
    m_previousTime = m_startTime;
  }

  void Timer::Tick() {
    const Clock::time_point currentTime = Clock::now();

    const std::chrono::duration<float> delta = currentTime - m_previousTime;
    const std::chrono::duration<float> total = currentTime - m_startTime;

    m_deltaSeconds = delta.count();
    m_totalSeconds = total.count();

    m_previousTime = currentTime;
  }
}