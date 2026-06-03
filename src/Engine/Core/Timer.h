#pragma once

#include <chrono>

namespace Engine {
  class Timer final {
  public:
    Timer();

    void Tick();

    float GetDeltaSeconds() const { return m_deltaSeconds; }
    float GetTotalSeconds() const { return m_totalSeconds; }

  private:
    using Clock = std::chrono::high_resolution_clock;

    Clock::time_point m_startTime;
    Clock::time_point m_previousTime;

    float m_deltaSeconds = 0.0f;
    float m_totalSeconds = 0.0f;
  };
}