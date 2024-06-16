#pragma once
#include "Singleton.h"
#include <chrono>

class TimeManager final : public Singleton<TimeManager>
{
public:
    [[nodiscard]] float GetElapsed(){
        return m_ElapsedTime.count();
    }
    [[nodiscard]] std::chrono::high_resolution_clock::time_point GetCurrent() const{
        return m_Current;
    }
    [[nodiscard]] std::chrono::duration<float, std::milli> GetFixedTimeStep() const{
        return m_FixedTimeStep;
    }
    [[nodiscard]] float GetTotalElapsed() {
        return std::chrono::duration<float>(m_StartTime - m_Current).count();
    }
    void Update(){
        m_Current = std::chrono::high_resolution_clock::now();
        m_ElapsedTime = std::chrono::duration<float>(m_Current - m_Previous);
        m_Previous = m_Current;
    }
private:
    friend class Singleton<TimeManager>;
    TimeManager() : m_StartTime{ std::chrono::high_resolution_clock::now() } {};
    std::chrono::duration<float> m_ElapsedTime{};
    std::chrono::high_resolution_clock::time_point m_Previous{std::chrono::high_resolution_clock::now() };
    const std::chrono::duration<float, std::milli> m_FixedTimeStep{ 100.f / 16.f };
    std::chrono::high_resolution_clock::time_point m_Current{};
    std::chrono::high_resolution_clock::time_point m_StartTime{};
};