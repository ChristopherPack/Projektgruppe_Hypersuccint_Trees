#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "gmock/gmock-matchers.h"

#define PHT_TEST

#include "pht/precomputed_function.h"
#include "pht/cached_function.h"
#include "pht/timed_cached_function.h"


#include <iostream>


class MockFunctions {
public:
    MOCK_METHOD(uint32_t, add, (uint32_t a, uint32_t b));
    MOCK_METHOD(void, precompute_add, ((std::map<std::tuple<uint32_t,uint32_t>,uint32_t>&)));
};

TEST(CachingTest, PrecomputedFunctionControlFlowTest) {
    MockFunctions mockFunctions;
    ON_CALL(mockFunctions, precompute_add(::testing::_)).WillByDefault([&mockFunctions](std::map<std::tuple<uint32_t,uint32_t>,uint32_t>& cache){
        for(uint32_t i = 0; i <= 100; i++) {
            for(uint32_t j = 0; j <= 100; j++) {
                cache.insert({{i, j}, mockFunctions.add(i, j)});
            }
        }
    });
    ON_CALL(mockFunctions, add(::testing::_, ::testing::_)).WillByDefault([](uint32_t a, uint32_t b){return a+b;});
    
    pht::PrecomputedFunction<uint32_t,uint32_t,uint32_t> precomputed_add_func = pht::PrecomputedFunction<uint32_t,uint32_t,uint32_t>([&mockFunctions](std::map<std::tuple<uint32_t,uint32_t>,uint32_t>& cache){mockFunctions.precompute_add(cache);});

    {
        ::testing::InSequence sequence;
        EXPECT_CALL(mockFunctions, precompute_add(::testing::_)).Times(1);
        EXPECT_CALL(mockFunctions, add(::testing::Le(100), ::testing::Le(100))).Times(101*101);
    }
    precomputed_add_func(0, 0);

    EXPECT_CALL(mockFunctions, precompute_add(::testing::_)).Times(0);
    for(uint32_t i = 0; i <= 100; i++) {
        for(uint32_t j = 0; j <= 100; j++) {
            precomputed_add_func(i, j);
        }
    }

    precomputed_add_func.markDirty();
    {
        ::testing::InSequence sequence;
        EXPECT_CALL(mockFunctions, precompute_add(::testing::_)).Times(1);
        EXPECT_CALL(mockFunctions, add(::testing::Le(100), ::testing::Le(100))).Times(101*101);
    }
    precomputed_add_func(0, 0);

    EXPECT_CALL(mockFunctions, precompute_add(::testing::_)).Times(0);
    for(uint32_t i = 0; i <= 100; i++) {
        for(uint32_t j = 0; j <= 100; j++) {
            precomputed_add_func(i, j);
        }
    }

    EXPECT_THROW({precomputed_add_func(101, 101);}, std::out_of_range);
}

TEST(CachingTest, PrecomputedFunctionDataTest) {
    pht::PrecomputedFunction<uint32_t,uint32_t,uint32_t> precomputed_add_func = pht::PrecomputedFunction<uint32_t,uint32_t,uint32_t>([](std::map<std::tuple<uint32_t,uint32_t>,uint32_t>& cache){
        for(uint32_t i = 0; i <= 100; i++) {
            for(uint32_t j = 0; j <= 100; j++) {
                cache.insert({{i, j}, i+j});
            }
        }
    });

    EXPECT_EQ(precomputed_add_func(0, 0), 0);
    for(uint32_t i = 0; i <= 100; i++) {
        for(uint32_t j = 0; j <= 100; j++) {
            EXPECT_EQ(precomputed_add_func(i, j), i+j);
        }
    }

    precomputed_add_func.markDirty();
    EXPECT_EQ(precomputed_add_func(0, 0), 0);
    for(uint32_t i = 0; i <= 100; i++) {
        for(uint32_t j = 0; j <= 100; j++) {
            EXPECT_EQ(precomputed_add_func(i, j), i+j);
        }
    }
}

TEST(CachingTest, CachedFunctionControlFlowTest) {
    MockFunctions mockFunctions;
    ON_CALL(mockFunctions, add(::testing::_, ::testing::_)).WillByDefault([](uint32_t a, uint32_t b){return a+b;});
    
    pht::CachedFunction<uint32_t,uint32_t,uint32_t> cached_add_func = pht::CachedFunction<uint32_t,uint32_t,uint32_t>([&mockFunctions](uint32_t a, uint32_t b){return mockFunctions.add(a, b);});

    for(uint32_t i = 0; i <= 100; i++) {
        for(uint32_t j = 0; j <= 100; j++) {
            EXPECT_CALL(mockFunctions, add(i, j)).Times(1);
            cached_add_func(i, j);
        }
    }
    for(uint32_t i = 0; i <= 100; i++) {
        for(uint32_t j = 0; j <= 100; j++) {
            EXPECT_CALL(mockFunctions, add(i, j)).Times(0);
            cached_add_func(i, j);
        }
    }

    cached_add_func.clear();
    for(uint32_t i = 0; i <= 100; i++) {
        for(uint32_t j = 0; j <= 100; j++) {
            EXPECT_CALL(mockFunctions, add(i, j)).Times(1);
            cached_add_func(i, j);
        }
    }
    for(uint32_t i = 0; i <= 100; i++) {
        for(uint32_t j = 0; j <= 100; j++) {
            EXPECT_CALL(mockFunctions, add(i, j)).Times(0);
            cached_add_func(i, j);
        }
    }
    
    EXPECT_CALL(mockFunctions, add(::testing::Le(100), ::testing::Le(100))).Times(101*101);
    cached_add_func.invalidate();
    for(uint32_t i = 0; i <= 100; i++) {
        for(uint32_t j = 0; j <= 100; j++) {
            EXPECT_CALL(mockFunctions, add(i, j)).Times(0);
            cached_add_func(i, j);
        }
    }
}

TEST(CachingTest, CachedFunctionDataTest) {
    pht::CachedFunction<uint32_t,uint32_t,uint32_t> cached_add_func = pht::CachedFunction<uint32_t,uint32_t,uint32_t>([](uint32_t a, uint32_t b){return a+b;});

    for(uint32_t i = 0; i <= 100; i++) {
        for(uint32_t j = 0; j <= 100; j++) {
            EXPECT_EQ(cached_add_func(i, j), i+j);
        }
    }

    cached_add_func.clear();
    for(uint32_t i = 0; i <= 100; i++) {
        for(uint32_t j = 0; j <= 100; j++) {
            EXPECT_EQ(cached_add_func(i, j), i+j);
        }
    }
    
    cached_add_func.invalidate();
    for(uint32_t i = 0; i <= 100; i++) {
        for(uint32_t j = 0; j <= 100; j++) {
            EXPECT_EQ(cached_add_func(i, j), i+j);
        }
    }
}

TEST(CachingTest, TimedCachedFunctionControlFlowTest) {
    MockFunctions mockFunctions;
    ON_CALL(mockFunctions, add(::testing::_, ::testing::_)).WillByDefault([](uint32_t a, uint32_t b){return a+b;});
    
    pht::TimedCachedFunction<uint32_t,uint32_t,uint32_t> timed_cached_add_func = pht::TimedCachedFunction<uint32_t,uint32_t,uint32_t>([&mockFunctions](uint32_t a, uint32_t b){return mockFunctions.add(a, b);});

    for(uint32_t i = 0; i <= 100; i++) {
        for(uint32_t j = 0; j <= 100; j++) {
            EXPECT_CALL(mockFunctions, add(i, j)).Times(1);
            timed_cached_add_func(i, j);
        }
    }
    for(uint32_t i = 0; i <= 100; i++) {
        for(uint32_t j = 0; j <= 100; j++) {
            EXPECT_CALL(mockFunctions, add(i, j)).Times(0);
            timed_cached_add_func(i, j);
        }
    }

    timed_cached_add_func.clear();
    for(uint32_t i = 0; i <= 100; i++) {
        for(uint32_t j = 0; j <= 100; j++) {
            EXPECT_CALL(mockFunctions, add(i, j)).Times(1);
            timed_cached_add_func(i, j);
        }
    }
    for(uint32_t i = 0; i <= 100; i++) {
        for(uint32_t j = 0; j <= 100; j++) {
            EXPECT_CALL(mockFunctions, add(i, j)).Times(0);
            timed_cached_add_func(i, j);
        }
    }
    
    EXPECT_CALL(mockFunctions, add(::testing::Le(100), ::testing::Le(100))).Times(101*101);
    timed_cached_add_func.invalidate();
    for(uint32_t i = 0; i <= 100; i++) {
        for(uint32_t j = 0; j <= 100; j++) {
            EXPECT_CALL(mockFunctions, add(i, j)).Times(0);
            timed_cached_add_func(i, j);
        }
    }

    std::this_thread::sleep_for(std::chrono::seconds(5));
    EXPECT_CALL(mockFunctions, add(0, 0)).Times(0);
    EXPECT_NO_THROW({timed_cached_add_func(0, 0);});
}

TEST(CachingTest, TimedCachedFunctionDataTest) {
    pht::TimedCachedFunction<uint32_t,uint32_t,uint32_t> timed_cached_add_func = pht::TimedCachedFunction<uint32_t,uint32_t,uint32_t>([](uint32_t a, uint32_t b){return a+b;});

    for(uint32_t i = 0; i <= 100; i++) {
        for(uint32_t j = 0; j <= 100; j++) {
            EXPECT_EQ(timed_cached_add_func(i, j), i+j);
        }
    }

    timed_cached_add_func.clear();
    for(uint32_t i = 0; i <= 100; i++) {
        for(uint32_t j = 0; j <= 100; j++) {
            EXPECT_EQ(timed_cached_add_func(i, j), i+j);
        }
    }
    
    timed_cached_add_func.invalidate();
    for(uint32_t i = 0; i <= 100; i++) {
        for(uint32_t j = 0; j <= 100; j++) {
            EXPECT_EQ(timed_cached_add_func(i, j), i+j);
        }
    }

    pht::TimedCachedFunction<uint32_t,uint32_t,uint32_t> timed_cached_add_func_short = pht::TimedCachedFunction<uint32_t,uint32_t,uint32_t>([](uint32_t a, uint32_t b){return a+b;}, 500);
    for(uint32_t i = 0; i <= 100; i++) {
        for(uint32_t j = 0; j <= 100; j++) {
            EXPECT_EQ(timed_cached_add_func(i, j), i+j);
        }
    }

    std::this_thread::sleep_for(std::chrono::seconds(5));
    for(uint32_t i = 0; i <= 100; i++) {
        for(uint32_t j = 0; j <= 100; j++) {
            EXPECT_EQ(timed_cached_add_func(i, j), i+j);
        }
    }
}