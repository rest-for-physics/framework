
#include <TRestRun.h>
#include <gtest/gtest.h>

TEST(Framework, example) {
    auto run = TRestRun();
    run.PrintAllMetadata();
    EXPECT_TRUE(&run != nullptr);
    return;
}
