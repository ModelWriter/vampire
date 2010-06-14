#include <iostream>
#include "Lib/SkipList.hpp"
#include "Lib/BinaryHeap.hpp"
#include "Lib/DHMultiset.hpp"
#include "Lib/Int.hpp"
#include "Lib/DArray.hpp"
#include "Lib/Random.hpp"
#include "Lib/Sort.hpp"

#include "Test/UnitTesting.hpp"

#define UNIT_ID skiplist
UT_CREATE;

using namespace std;
using namespace Lib;

const int cnt=105000;

typedef int StoredType;

StoredType arr[cnt];

TEST_FUN(skiplist1)
{
  SkipList<StoredType, Int> sl1;
  SkipList<StoredType, Int> sl2;
  DArray<StoredType> darr(cnt);
  DHMultiset<StoredType> ms;

  for(int i=0;i<cnt;i++)
  {
    int num=(rand()%cnt)/100;
    ms.insert(num);
    sl1.insert(num);

    sl2.insert(num);
    darr[i]=num;
    arr[i]=num;
  }

  for(int i=0;i<cnt/2;i++)
  {
    ms.remove(arr[i]);
  }

  for(int i=0;i<cnt/2;i++)
  {
    sl1.remove(arr[i]);
  }

  sort<Int>(darr.begin(), darr.end());
  for(int i=0;i<cnt;i++)
  {
    ASS_EQ(sl2.pop(),darr[i]);
  }
}