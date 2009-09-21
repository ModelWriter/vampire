/**
 * @file Timer.cpp
 * Implements class Timer.
 */

#include <ctime>

#include "../Debug/Assertion.hpp"
#include "../Debug/Tracer.hpp"

#include "Int.hpp"
#include "Portability.hpp"

#include "Timer.hpp"


#define UNIX_USE_SIGALRM 1

using namespace std;

#if COMPILER_MSVC

#include <windows.h>

int Lib::Timer::miliseconds()
{
  static bool init=false;
  static LARGE_INTEGER freq;
  if(!init) {
    ALWAYS(QueryPerformanceFrequency(&freq));
    init=true;
  }

  LARGE_INTEGER counter;
  QueryPerformanceCounter(&counter);

  return counter.QuadPart*1000/freq.QuadPart;
}

void Lib::Timer::initTimer()
{
}


#elif UNIX_USE_SIGALRM

#include <signal.h>
#include <sys/time.h>

int timer_sigalrm_counter=-1;

void
timer_sigalrm_handler (int sig)
{
  timer_sigalrm_counter++;
}

/** number of miliseconds (of CPU time) passed since some moment */
int Lib::Timer::miliseconds()
{
  CALL("Timer::miliseconds");
  ASS_GE(timer_sigalrm_counter, 0);

  return timer_sigalrm_counter;
}

void Lib::Timer::initTimer()
{
  CALL("Timer::initTimer");
  ASS_EQ(timer_sigalrm_counter, -1);

  timer_sigalrm_counter=0;

  signal (SIGALRM, timer_sigalrm_handler);
  struct itimerval oldt, newt;
  newt.it_interval.tv_usec = 1000;
  newt.it_interval.tv_sec = 0;
  newt.it_value.tv_usec = 1000;
  newt.it_value.tv_sec = 0;
  setitimer (ITIMER_REAL, &newt, &oldt);
}


#else

#include <sys/time.h>
#include <sys/resource.h>

/** number of miliseconds (of CPU time) passed since some moment */
int Lib::Timer::miliseconds()
{
  struct timeval tim;
  struct rusage ru;
  getrusage(RUSAGE_SELF, &ru);
  tim=ru.ru_utime;
  int t=tim.tv_sec*1000 + tim.tv_usec / 1000;
  tim=ru.ru_stime;
  t+=tim.tv_sec*1000 + tim.tv_usec / 1000;
  return t;
//  return (int)( ((long long)clock())*1000/CLOCKS_PER_SEC );
}

void Lib::Timer::initTimer()
{
}

#endif

namespace Lib
{

string Timer::msToSecondsString(int ms)
{
  return Int::toString(static_cast<float>(ms)/1000)+" s";
}

};

//#include <iostream>
//
//int main (int argc, char* argv [])
//{
//  int counter = 0;
//  Lib::Timer timer;
//  int last = -1;
//  timer.start();
//
//  for (;;) {
//    counter++;
//    int current = timer.elapsedDeciseconds();
//    if (current <= last) {
//      continue;
//    }
//    last = current;
////      cout << current << "\n";
//    if (current == 100) {
//      cout << "Total calls to clock() during "
//	   << current
//	   << " deciseconds is "
//	   << counter
//	   << '\n';
//      return 0;
//    }
//  }
//}
//
