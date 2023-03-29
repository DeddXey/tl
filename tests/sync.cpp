#include "doctest.h"

#include "sync.h"
#include <chrono>
#include <thread>

//SyncCounter<int16_t> sc;

//
//void sc_thread1()
//{
//  std::this_thread::sleep_for( 7us );
//}
//
//void sc_thread2()
//{
//  std::this_thread::sleep_for( 7us );
//}
//
//
////----------------------------------------------------
//TEST_CASE("sync, SyncCounter") {
//  //  std::srand(1234);
//  //
//  //  for (auto i = 0U; i < tstSize; ++i)
//  //    //        testData.push_back(std::rand());
//  //    testData.push_back(i);
//
//  std::thread t1(sc_thread1);
//  std::thread t2(sc_thread2);
//
//  t1.join();
//  t2.join();
//
//}