
#include <thread>

#include "wpriorityqueue.hpp"		// wrapper priority queue

using namespace hrd11;


WPriorityQueue<int>* gpq;


static void ThreadFunc()
{
  gpq->Push(3);
  gpq->Push(5);
  gpq->Push(2);
  gpq->Push(4);
  gpq->Push(1);

}

int main()
{
  int tmp = 0;
  WPriorityQueue<int> wpq;
  std::chrono::seconds timeout(2);

  gpq = &wpq;


  std::thread th(ThreadFunc);
  std::thread th1(ThreadFunc);
  std::thread th2(ThreadFunc);
  std::thread th3(ThreadFunc);

  for (int i = 0; i < 20; ++i)
  {
    bool ret = wpq.Pop(tmp, timeout);
    printf("-- pop(%d), ret %d\n", tmp, ret);
  }

  th.join();
  th1.join();
  th2.join();
  th3.join();

  return 0;
}
