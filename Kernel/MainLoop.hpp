/**
 * @file MainLoop.hpp
 * Defines class MainLoop.
 */

#ifndef __MainLoop__
#define __MainLoop__

#include "Forwards.hpp"

#include "Lib/Environment.hpp"
#include "Lib/Exception.hpp"

#include "Shell/Statistics.hpp"

namespace Kernel {

using namespace Lib;
using namespace Inferences;
using namespace Shell;

struct MainLoopResult
{
  typedef Statistics::TerminationReason TerminationReason;

  MainLoopResult(TerminationReason reason)
  : terminationReason(reason) {}
  MainLoopResult(TerminationReason reason, Clause* ref)
  : terminationReason(reason), refutation(ref) {}

  void updateStatistics();

  TerminationReason terminationReason;
  Clause* refutation;
};



class MainLoop {
public:
  virtual ~MainLoop() {}
  virtual void addInputClauses(ClauseIterator cit) = 0;

  MainLoopResult run();

  static MainLoopSP createFromOptions();

  /**
   * A struct that is thrown as an exception when a refutation is found
   * during the main loop.
   */
  struct RefutationFoundException : public ThrowableBase
  {
    RefutationFoundException(Clause* ref) : refutation(ref)
    {
      CALL("SaturationAlgorithm::RefutationFoundException::RefutationFoundException");
      ASS(isRefutation(ref));
    }

    Clause* refutation;
  };

protected:
  virtual MainLoopResult runImpl() = 0;

  static bool isRefutation(Clause* cl);
  static ImmediateSimplificationEngineSP createISE();
};

}

#endif // __MainLoop__
