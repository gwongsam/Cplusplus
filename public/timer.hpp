#pragma once
#include <functional>
#include <list>
#include <vector>

#if true
class TimerManager;

class Timer {
 public:
  enum TimerType { ONCE, CIRCLE };

  Timer(TimerManager& manager);
  ~Timer();

  template <typename Fun>
  void Start(Fun fun, unsigned interval, TimerType timeType = CIRCLE);
  void Stop();

 private:
  void OnTimer(unsigned long long now);

 private:
  friend class TimerManager;
  TimerManager& manager_;
  TimerType timerType_;
  std::function<void(void)> timerFun_;
  unsigned interval_;
  unsigned long long expires_;

  size_t heapIndex_;
};

class TimerManager {
 public:
  static unsigned long long GetCurrentMillisecs();
  void DetectTimers();

 private:
  friend class Timer;
  void AddTimer(Timer* timer);
  void RemoveTimer(Timer* timer);

  void UpHeap(size_t index);
  void DownHeap(size_t index);
  void SwapHeap(size_t, size_t index2);

 private:
  struct HeapEntry {
    unsigned long long time;
    Timer* timer;
  };
  std::vector<HeapEntry> heap_;
};

template <typename Fun>
inline void Timer::Start(Fun fun, unsigned interval, TimerType timeType) {
  Stop();
  interval_ = interval;
  timerFun_ = fun;
  timerType_ = timeType;
  this->expires_ = this->interval_ + TimerManager::GetCurrentMillisecs();
  manager_.AddTimer(this);
}

#else

class TimerManager;

class Timer {
 public:
  enum TimerType { ONCE, CIRCLE };

  Timer(TimerManager& manager);
  ~Timer();

  template <typename Fun>
  void Start(Fun fun, unsigned interval, TimerType timeType = CIRCLE);
  void Stop();

 private:
  void OnTimer(unsigned long long now);

 private:
  friend class TimerManager;

  TimerManager& manager_;
  TimerType timerType_;
  std::function<void(void)> timerFun_;
  unsigned interval_;
  unsigned long long expires_;

  int vecIndex_;
  std::list<Timer*>::iterator itr_;
};

class TimerManager {
 public:
  TimerManager();

  static unsigned long long GetCurrentMillisecs();
  void DetectTimers();

 private:
  friend class Timer;
  void AddTimer(Timer* timer);
  void RemoveTimer(Timer* timer);

  int Cascade(int offset, int index);

 private:
  typedef std::list<Timer*> TimeList;
  std::vector<TimeList> tvec_;
  unsigned long long checkTime_;
};

template <typename Fun>
inline void Timer::Start(Fun fun, unsigned interval, TimerType timeType) {
  Stop();
  interval_ = interval;
  timerFun_ = fun;
  timerType_ = timeType;
  expires_ = interval_ + TimerManager::GetCurrentMillisecs();
  manager_.AddTimer(this);
}
#endif
