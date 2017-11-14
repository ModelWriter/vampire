
/*
 * File PortfolioMode.hpp.
 *
 * This file is part of the source code of the software program
 * Vampire 4.2.2. It is protected by applicable
 * copyright laws.
 *
 * This source code is distributed under the licence found here
 * https://vprover.github.io/license.html
 * and in the source directory
 *
 * In summary, you are allowed to use Vampire for non-commercial
 * uses but not allowed to distribute, modify, copy, create derivatives,
 * or use in competitions. 
 * For other uses of Vampire please contact developers for a different
 * licence, which we will make an effort to provide. 
 */
/**
 * @file PortfolioMode.hpp
 * Defines class PortfolioMode.
 */

#ifndef __PortfolioMode__
#define __PortfolioMode__

#include "Forwards.hpp"

#include "Lib/Portability.hpp"
#include "Lib/ScopedPtr.hpp"
#include "Lib/Set.hpp"
#include "Lib/Stack.hpp"

#include "Lib/VString.hpp"
#include "Lib/Sys/Semaphore.hpp"

#include "Shell/Property.hpp"
#include "Schedules.hpp"

namespace CASC
{

using namespace std;
using namespace Lib;
using namespace Shell;

class PortfolioMode {
  enum {
    SEM_LOCK = 0,
    SEM_PRINTED = 1
  };

  PortfolioMode();

public:
  static bool perform(float slowness);

private:

  // some of these names are kind of arbitrary and should be perhaps changed

  bool searchForProof();
  bool performStrategy(Shell::Property* property);
  void getSchedules(Property& prop, Schedule& quick, Schedule& fallback);
  bool runSchedule(Schedule& schedule, int terminationTime);
  unsigned getSliceTime(vstring sliceCode,vstring& chopped);
  bool waitForChildAndCheckIfProofFound();
  void runSlice(vstring slice, unsigned timeLimitInDeciseconds) NO_RETURN;
  void runSlice(Options& strategyOpt) NO_RETURN;

#if VDEBUG
  DHSet<pid_t> childIds;
#endif

  float _slowness;

  /**
   * Problem that is being solved.
   *
   * Note that in the current process this child object is the only one that
   * will be using the problem object.
   */
  ScopedPtr<Problem> _prb;

  Semaphore _syncSemaphore; // semaphore for synchronizing proof printing
};

}

#endif // __PortfolioMode__
