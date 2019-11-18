#pragma once
#include "IRuntimeModule.hpp"
#include <chrono>

namespace My {
class Clock : implements IRuntimeModule {
public:
    virtual int  Initialize();
    virtual void Tick();
    virtual void Finalize();

    float totalTime() const;
    float deltaTime() const;

    void Reset();
    void Start();
    void Stop();

private:
    std::chrono::duration<double> mDeltaTime;
    std::chrono::duration<double> mPausedTime;

    std::chrono::high_resolution_clock::time_point mBaseTime;
    std::chrono::high_resolution_clock::time_point mStopTime;
    std::chrono::high_resolution_clock::time_point mPrevTime;
    std::chrono::high_resolution_clock::time_point mCurrTime;

    bool mStopped;
};

}  // namespace My