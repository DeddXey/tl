

#include "fifo.h"
#include "fifo_atomic.h"
#include "gtest/gtest.h"
#include <thread>
#include <chrono>


constexpr uint32_t  tstSize = 1024;

using FifoType = Fifo <int32_t, 255>;
using FifoTypeAtomic = FifoAtomic <int32_t, 255>;

FifoType ff;
FifoTypeAtomic ffa;

std::vector<int32_t> testData;


uint32_t    cnt = 0;

//----------------------------------------------------
template <typename T>
void fifo_wr_thread(T& ff)
{
    uint32_t cnt = 0;

    while (cnt < tstSize) {
        if (ff.push(testData[cnt])) {
            std::this_thread::yield();
            ++cnt;
        }
    //    std::this_thread::sleep_for(std::chrono::microseconds(std::rand() / 10));
    }
}

//----------------------------------------------------
template <typename T>
void fifo_rd_thread(T& ff)
{
    uint32_t cnt = 0;

    while (cnt < tstSize) {
        if (! ff.empty()) {
            std::this_thread::yield();
            uint32_t out = ff.getOut();
            ff.pop();
            EXPECT_EQ(out, testData[cnt]);
            ++cnt;
        }
//        std::this_thread::sleep_for(std::chrono::microseconds(std::rand() / 10));
    }
}


//----------------------------------------------------
TEST(fifo, fifo_parallel)
{
    std::srand(1234);

    for(auto i = 0U; i < tstSize; ++i)
        //        testData.push_back(std::rand());
        testData.push_back(i);


    std::thread tx(fifo_wr_thread<FifoType>, std::ref(ff));
    std::thread rx(fifo_rd_thread<FifoType>, std::ref(ff));

    tx.join();
    rx.join();
}


//----------------------------------------------------
TEST(fifo, fifo_parallel_atomic)
{
    std::srand(1234);

    for(auto i = 0U; i < tstSize; ++i)
        //        testData.push_back(std::rand());
        testData.push_back(i);


    std::thread tx(fifo_wr_thread<FifoTypeAtomic>, std::ref(ffa));
    std::thread rx(fifo_rd_thread<FifoTypeAtomic>, std::ref(ffa));

    tx.join();
    rx.join();
}
