
#include <TRestRun.h>
#include <gtest/gtest.h>

TEST(FrameworkCore, TRestRun) {
    auto run = TRestRun();
    run.PrintAllMetadata();
    EXPECT_TRUE(&run != nullptr);
}
