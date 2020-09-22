#include "doctest.h"

#include "fifo.h"
#include "fifo_atomic.h"
#include <chrono>
#include <thread>
#include <vector>

// std::vector<int32_t> testData;

struct BigStruct
{
  uint32_t threadNum = 0;
  uint32_t counter   = 0;
  uint32_t payload1  = 0;
  uint32_t payload2  = 0;
  uint32_t payload3  = 0;
  uint32_t payload4  = 0;
};
constexpr uint32_t tstSize = 10;

using FifoType       = Fifo<BigStruct, 5>;
using FifoTypeAtomic = FifoAtomic<BigStruct, 5>;

FifoType       ff;
FifoTypeAtomic ffa;
// uint32_t    cnt = 0;

//----------------------------------------------------
template<typename T>
void fifo_wr_thread0(T &ff)
{
  uint32_t cnt = 0;

  while (cnt < tstSize) {
    BigStruct testData;
    testData.threadNum = 0;
    testData.counter   = cnt;
    if (ff.push(testData)) {
      MESSAGE("Wr0: " << cnt);
//      std::this_thread::yield();
      ++cnt;
    }
  }
}

//----------------------------------------------------
template<typename T>
void fifo_wr_thread1(T &ff)
{
  uint32_t cnt = 0;

  while (cnt < tstSize) {
    BigStruct testData;
    testData.threadNum = 1;
    testData.counter   = cnt;
    if (ff.push(testData)) {
      MESSAGE("Wr1: " << cnt );
//      std::this_thread::yield();
      ++cnt;
    }
  }
}

//----------------------------------------------------
template<typename T>
void fifo_rd_thread(T &ff)
{
  uint32_t cnts[2] = { 0, 0 };
  uint32_t cnt     = 0;

  while (cnt < tstSize) {
    if (!ff.empty()) {

//      std::this_thread::yield();
      auto out = ff.getOut();
      MESSAGE("Rd Thrd: " << out.threadNum << "  Cnt: " << out.counter);
      CHECK_EQ(out.counter, cnts[out.threadNum]);
      ++cnts[out.threadNum];
      ff.pop();
      ++cnt;
    }
    //        std::this_thread::sleep_for(std::chrono::microseconds(std::rand()
    //        / 10));
  }
}

////----------------------------------------------------
//TEST_CASE("fifo , fifo_parallel")
//{
//  //  std::srand(1234);
//  //
//  //  for (auto i = 0U; i < tstSize; ++i)
//  //    //        testData.push_back(std::rand());
//  //    testData.push_back(i);
//
//  std::thread tx0(fifo_wr_thread0<FifoType>, std::ref(ff));
//  std::thread tx1(fifo_wr_thread1<FifoType>, std::ref(ff));
//  std::thread rx(fifo_rd_thread<FifoType>, std::ref(ff));
//
//  tx0.join();
//  tx1.join();
//  rx.join();
//}

////----------------------------------------------------
//TEST_CASE("fifo, fifo_parallel_atomic")
//{
//  //  std::srand(1234);
//  //
//  //  for (auto i = 0U; i < tstSize; ++i)
//  //    //        testData.push_back(std::rand());
//  //    testData.push_back(i);
//
//  std::thread tx0(fifo_wr_thread0<FifoTypeAtomic>, std::ref(ffa));
//  std::thread tx1(fifo_wr_thread1<FifoTypeAtomic>, std::ref(ffa));
//  std::thread rx(fifo_rd_thread<FifoTypeAtomic>, std::ref(ffa));
//
//  tx0.join();
//  tx1.join();
//  rx.join();
//}
