#include <TRestGDMLParser.h>
#include <TUUID.h>
#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <iterator>

namespace {
namespace fs = std::filesystem;

class GDMLParserTest : public ::testing::Test {
   protected:
    fs::path directory;

    void SetUp() override {
        directory = fs::temp_directory_path() / (std::string("rest-gdml-test-") + TUUID().AsString());
        fs::create_directories(directory);
    }

    void TearDown() override { fs::remove_all(directory); }

    fs::path WriteGDML(const std::string& define) {
        const auto input = directory / "geometry.gdml";
        std::ofstream output(input);
        output << "<gdml>" << define
               << "<solids><box name=\"box\" x=\"10\" y=\"20\" "
                  "z=\"30\" lunit=\"mm\"/></solids></gdml>";
        return input;
    }

    static std::string Read(const fs::path& path) {
        std::ifstream input(path);
        return std::string(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>());
    }
};

class InspectableGDMLParser : public TRestGDMLParser {
   public:
    const TiXmlElement* Globals() const { return fElementGlobal; }
};

TEST_F(GDMLParserTest, DefineHasIndependentOwnershipAndStackTeardown) {
    const auto input = WriteGDML("<define><constant name=\"length\" value=\"10\"/></define>");
    std::string compiled;
    {
        InspectableGDMLParser parser;
        parser.fOutputGdmlDirectory = directory.string() + "/";
        parser.Load(input.string());
        ASSERT_NE(parser.Globals(), nullptr);
        EXPECT_EQ(parser.Globals()->Parent(), nullptr);
        ASSERT_NE(parser.Globals()->FirstChildElement("constant"), nullptr);
        EXPECT_STREQ(parser.Globals()->FirstChildElement("constant")->Attribute("value"), "10");
        compiled = parser.GetOutputGDMLFile();
    }
    // Destruction must neither crash nor invalidate the compiled geometry.
    EXPECT_EQ(Read(compiled), Read(input) + "\n");
}

TEST_F(GDMLParserTest, MissingDefineHasNoGlobalTree) {
    const auto input = WriteGDML("");
    InspectableGDMLParser parser;
    parser.fOutputGdmlDirectory = directory.string() + "/";
    parser.Load(input.string());
    EXPECT_EQ(parser.Globals(), nullptr);
    EXPECT_EQ(Read(parser.GetOutputGDMLFile()), Read(input) + "\n");
}

TEST_F(GDMLParserTest, RepeatedLoadsPreserveCompiledGeometry) {
    const auto input = WriteGDML("<define/>");
    InspectableGDMLParser parser;
    parser.fOutputGdmlDirectory = directory.string() + "/";
    for (int iteration = 0; iteration < 10; iteration++) {
        parser.Load(input.string());
        ASSERT_NE(parser.Globals(), nullptr);
        EXPECT_EQ(parser.Globals()->Parent(), nullptr);
        EXPECT_EQ(Read(parser.GetOutputGDMLFile()), Read(input) + "\n");
    }
    const auto withoutDefine = WriteGDML("");
    parser.Load(withoutDefine.string());
    EXPECT_EQ(parser.Globals(), nullptr);
}
}  // namespace
