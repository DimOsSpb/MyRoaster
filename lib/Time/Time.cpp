#include <Time.h>



Timer::Timer(uint32_t periodMS){
    _periodMs = periodMS;
};

void Timer::start(uint32_t periodMS){
    _periodMs = periodMS;
};

bool Timer::isReady(){
    if (millis() - _timer >= _periodMs) {
      do {
        _timer += _periodMs;
        if (_timer < _periodMs) break;          //if uint32 overload
      } while (_timer < millis() - _periodMs);
      return true;
    }
    return false;
};