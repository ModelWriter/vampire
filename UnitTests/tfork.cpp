/**
 * @file tfork.cpp
 * Test of the forking abilities.
 */

#include "Lib/Portability.hpp"

#include "Test/UnitTesting.hpp"

#define UNIT_ID fork
UT_CREATE;


//forking isn't supported in the visual studio
#if !COMPILER_MSVC

#include <cerrno>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#include "Lib/Int.hpp"

#include "Lib/Sys/Multiprocessing.hpp"
#include "Lib/Sys/Semaphore.hpp"
#include "Lib/Sys/SyncPipe.hpp"


using namespace Lib;
using namespace Lib::Sys;

TEST_FUN(fork_simple)
{
  pid_t fres=Multiprocessing::instance()->fork();
  ASS_NEQ(fres,-1);
  if(!fres) {
    //we're in the child
    int a=0;
    for(int i=0;i<1000;i++) {
      for(int j=0;j<1000;j++) {
	a++;
      }
    }
    exit(0);
  }

  int status;
  errno=0;
  pid_t res=waitpid(fres, &status, 0);

  if(res==-1) {
    SYSTEM_FAIL("Error in waiting for forked process.",errno);
  }
  ASS_EQ(res,fres); //pid of the forked process and the one we waited for must be the same

  ASS(!WIFSTOPPED(status));
  ASS(!WIFSIGNALED(status));
  ASS(WIFEXITED(status));
  ASS_EQ(WEXITSTATUS(status),0);
}

TEST_FUN(fork_and_semaphores)
{
  Semaphore sem(3);

  pid_t fres=Multiprocessing::instance()->fork();
  ASS_NEQ(fres,-1);
  if(!fres) {
    //we're in the child

    Semaphore sem2(sem);

    sem.inc(1);
    int a=0;
    for(int i=0;i<1000;i++) {
      for(int j=0;j<1000;j++) {
	a++;
      }
    }

    sem2.inc(1);
    sem2.inc(0); //incPoint

    sem2.dec(2); //waitPoint

    exit(0);
  }

  sem.dec(0);
  ASS_EQ(sem.get(1),2);
  ASS_EQ(sem.get(0),0);

  ASS(!sem.isLastInstance()); //now the other process should be between incPoint and waitPoint

  sem.inc(2);

  int status;
  errno=0;
  pid_t res=waitpid(fres, &status, 0);

  if(res==-1) {
    SYSTEM_FAIL("Error in waiting for forked process.",errno);
  }
  ASS_EQ(res,fres); //pid of the forked process and the one we waited for must be the same

  ASS(!WIFSTOPPED(status));
  ASS(!WIFSIGNALED(status));
  ASS(WIFEXITED(status));
  ASS_EQ(WEXITSTATUS(status),0);

  ASS(sem.isLastInstance());
}

TEST_FUN(fork_and_pipe)
{
  SyncPipe p1;
  SyncPipe p2;

  pid_t fres=Multiprocessing::instance()->fork();
  ASS_NEQ(fres,-1);
  if(!fres) {
    //we're in the child1

    p2.neverRead();

    for(;;) {
      int num=-1;
      p1.acquireRead();
      p1.in()>>num;
      p1.releaseRead();

      if(num%2==1) {
	//this one isn't for this process, so put it back
	p1.acquireWrite();
	p1.out()<<num<<endl;
	p1.releaseWrite();
	continue;
      }
      p2.acquireWrite();
      p2.out()<<(num*3)<<endl;
      p2.releaseWrite();
      if(num==0) {
	exit(0);
      }
    }
  }

  pid_t fres2=Multiprocessing::instance()->fork();
  ASS_NEQ(fres2,-1);
  if(!fres2) {
    //we're in the child2

    p2.neverRead();

    for(;;) {
      int num=-1;
      p1.acquireRead();
      p1.in()>>num;
      p1.releaseRead();

      if(num%2==0) {
	//this one isn't for this process, so put it back
	p1.acquireWrite();
	p1.out()<<num<<endl;
	p1.releaseWrite();
	continue;
      }
      p2.acquireWrite();
      p2.out()<<(num*2)<<endl;
      p2.releaseWrite();
      if(num==1) {
	exit(0);
      }
    }
  }

  p1.neverRead();
  p2.neverWrite();

  //let's talk to children...
  for(int i=1000;i>=0;i--) {
    p1.acquireWrite();
    p1.out()<<i<<endl;
    p1.releaseWrite();

    int num=-1;
    p2.acquireRead();
    p2.in()>>num;
    p2.releaseRead();
    if(i%2==0) {
      ASS_EQ(num, i*3);
    }
    else {
      ASS_EQ(num, i*2);
    }
  }

  int status;
  errno=0;
  pid_t res=waitpid(fres, &status, 0);
  if(res==-1) {
    SYSTEM_FAIL("Error in waiting for first forked process.",errno);
  }
  ASS_EQ(res,fres); //pid of the forked process and the one we waited for must be the same

  ASS(!WIFSTOPPED(status));
  ASS(!WIFSIGNALED(status));
  ASS(WIFEXITED(status));
  ASS_EQ(WEXITSTATUS(status),0);

  errno=0;
  pid_t res2=waitpid(fres2, &status, 0);
  if(res2==-1) {
    SYSTEM_FAIL("Error in waiting for second forked process.",errno);
  }
  ASS_EQ(res2,fres2); //pid of the forked process and the one we waited for must be the same

  ASS(!WIFSTOPPED(status));
  ASS(!WIFSIGNALED(status));
  ASS(WIFEXITED(status));
  ASS_EQ(WEXITSTATUS(status),0);
}


#endif