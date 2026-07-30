#include <TFile.h>
#include <TGraph.h>
#include <TH1D.h>
#include <TKey.h>
#include <TList.h>
#include <TNamed.h>
#include <TObjString.h>
#include <TRestProcessRunner.h>
#include <TRestRun.h>
#include <TRestTools.h>
#include <TStreamerInfo.h>
#include <TTree.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <ostream>
#include <set>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>

namespace {
namespace fs = std::filesystem;

static_assert(!std::is_copy_constructible_v<TRestRun>);
static_assert(!std::is_copy_assignable_v<TRestRun>);
static_assert(std::is_move_constructible_v<TRestRootFileHandle>);
static_assert(!std::is_copy_constructible_v<TRestRootFileHandle>);

struct StreamerIdentity {
    std::string name;
    int version;
    unsigned checksum;

    bool operator<(const StreamerIdentity& other) const {
        return std::tie(name, version, checksum) < std::tie(other.name, other.version, other.checksum);
    }
    bool operator==(const StreamerIdentity& other) const {
        return name == other.name && version == other.version && checksum == other.checksum;
    }
};

std::ostream& operator<<(std::ostream& output, const StreamerIdentity& identity) {
    return output << identity.name << ':' << identity.version << ':' << identity.checksum;
}

struct SchemaInventory {
    std::set<StreamerIdentity> infos;
    std::set<std::string> rules;
};

class TemporaryDirectory {
   private:
    fs::path fPath;

   public:
    TemporaryDirectory() {
        fPath = fs::temp_directory_path() /
                ("rest-root-io-" + std::to_string(std::rand()) + "-" + std::to_string(std::rand()));
        fs::create_directories(fPath);
    }
    ~TemporaryDirectory() {
        std::error_code ignored;
        fs::remove_all(fPath, ignored);
    }
    const fs::path& Path() const { return fPath; }
};

std::string ShellQuote(const std::string& value) {
    std::string quoted = "'";
    for (char character : value) {
        if (character == '\'')
            quoted += "'\\''";
        else
            quoted += character;
    }
    return quoted + "'";
}

int RunProcess(const std::string& executable, const std::vector<std::string>& arguments) {
    std::string command = ShellQuote(executable);
    for (const auto& argument : arguments) command += " " + ShellQuote(argument);
    return std::system(command.c_str());
}

SchemaInventory ReadInventory(const fs::path& filename) {
    SchemaInventory result;
    std::unique_ptr<TFile> file(TFile::Open(filename.c_str(), "READ"));
    EXPECT_TRUE(file != nullptr && file->IsOpen() && !file->IsZombie());
    if (!file || file->IsZombie()) return result;

    std::unique_ptr<TList> list(file->GetStreamerInfoList());
    EXPECT_NE(list, nullptr);
    if (!list) return result;
    TIter next(list.get());
    while (TObject* object = next()) {
        if (auto* info = dynamic_cast<TStreamerInfo*>(object)) {
            result.infos.insert({info->GetName(), info->GetClassVersion(), info->GetCheckSum()});
            continue;
        }
        auto* rules = dynamic_cast<TList*>(object);
        if (rules == nullptr || std::string(rules->GetName()) != "listOfRules") continue;
        TIter nextRule(rules);
        while (TObject* ruleObject = nextRule()) {
            if (auto* rule = dynamic_cast<TObjString*>(ruleObject))
                result.rules.insert(rule->GetString().Data());
        }
    }
    return result;
}

std::set<StreamerIdentity> PayloadInfos(const SchemaInventory& inventory) {
    std::set<StreamerIdentity> result;
    std::copy_if(inventory.infos.begin(), inventory.infos.end(), std::inserter(result, result.end()),
                 [](const auto& identity) { return identity.name == "TRestIOFixturePayload"; });
    return result;
}

std::vector<char> ReadBytes(const fs::path& filename) {
    std::ifstream input(filename, std::ios::binary);
    return {std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>()};
}

bool HasKeyClass(const fs::path& filename, const std::string& className) {
    std::unique_ptr<TFile> file(TFile::Open(filename.c_str(), "READ"));
    if (!file || file->IsZombie()) return false;
    TIter next(file->GetListOfKeys());
    while (TObject* object = next()) {
        auto* key = dynamic_cast<TKey*>(object);
        if (key != nullptr && className == key->GetClassName()) return true;
    }
    return false;
}

void CreateTwoVersionFixture(const fs::path& filename) {
    ASSERT_EQ(RunProcess(REST_IO_WRITER_V1, {filename.string(), "RECREATE"}), 0);
    ASSERT_EQ(RunProcess(REST_IO_WRITER_V2, {filename.string(), "UPDATE"}), 0);
}
}  // namespace

TEST(RootIO, UnloadedDictionaryPreservesAutomaticCollectionEvolutionAndRenamedFieldRule) {
    TemporaryDirectory temporary;
    const fs::path filename = temporary.Path() / "two-versions.root";
    CreateTwoVersionFixture(filename);

    // Establish the two independent ROOT behaviors before REST mutates the file:
    // fSamples evolves automatically by retaining its name, while fLegacyCode
    // reaches fRenamedCode exclusively through the embedded schema rule.
    ASSERT_EQ(RunProcess(REST_IO_VERIFIER_V1, {filename.string()}), 0);
    ASSERT_EQ(RunProcess(REST_IO_VERIFIER, {filename.string()}), 0);

    const SchemaInventory before = ReadInventory(filename);
    const auto payloadBefore = PayloadInfos(before);
    ASSERT_EQ(payloadBefore.size(), 2);
    ASSERT_TRUE(std::any_of(before.rules.begin(), before.rules.end(), [](const auto& rule) {
        return rule.find("TRestIOFixturePayload") != std::string::npos &&
               rule.find("fLegacyCode") != std::string::npos &&
               rule.find("fRenamedCode") != std::string::npos;
    }));
    EXPECT_FALSE(std::any_of(before.rules.begin(), before.rules.end(), [](const auto& rule) {
        return rule.find("TRestIOFixturePayload") != std::string::npos &&
               rule.find("fSamples") != std::string::npos;
    }));

    ASSERT_EQ(RunProcess(REST_IO_UPDATER, {filename.string(), "preserve"}), 0);

    const SchemaInventory after = ReadInventory(filename);
    EXPECT_TRUE(
        std::includes(after.infos.begin(), after.infos.end(), before.infos.begin(), before.infos.end()));
    EXPECT_TRUE(
        std::includes(after.rules.begin(), after.rules.end(), before.rules.begin(), before.rules.end()));
    EXPECT_EQ(PayloadInfos(after), payloadBefore);
    // Re-read with each matching dictionary to prove that both automatic
    // collection evolution and the independent renamed-field rule still work.
    EXPECT_EQ(RunProcess(REST_IO_VERIFIER_V1, {filename.string()}), 0);
    EXPECT_EQ(RunProcess(REST_IO_VERIFIER, {filename.string(), "require-update"}), 0);
}

TEST(RootIO, BorrowedPreflightFailureLeavesFileByteIdentical) {
    TemporaryDirectory temporary;
    const fs::path filename = temporary.Path() / "preflight-failure.root";
    CreateTwoVersionFixture(filename);
    const auto before = ReadBytes(filename);

    ASSERT_EQ(RunProcess(REST_IO_UPDATER, {filename.string(), "borrowed-failure"}), 0);

    EXPECT_EQ(ReadBytes(filename), before);
    EXPECT_EQ(PayloadInfos(ReadInventory(filename)).size(), 2);
}

TEST(RootIO, LoadedDictionaryCachePreservesAutomaticAndRuleBasedEvolutionAcrossUpdates) {
    TemporaryDirectory temporary;
    const fs::path filename = temporary.Path() / "loaded-updates.root";
    CreateTwoVersionFixture(filename);
    ASSERT_EQ(RunProcess(REST_IO_VERIFIER_V1, {filename.string()}), 0);
    ASSERT_EQ(RunProcess(REST_IO_VERIFIER, {filename.string()}), 0);
    const SchemaInventory before = ReadInventory(filename);
    const auto payloadBefore = PayloadInfos(before);
    ASSERT_EQ(payloadBefore.size(), 2);

    ASSERT_EQ(RunProcess(REST_IO_LOADED_UPDATER, {filename.string()}), 0);

    const SchemaInventory after = ReadInventory(filename);
    EXPECT_TRUE(
        std::includes(after.infos.begin(), after.infos.end(), before.infos.begin(), before.infos.end()));
    EXPECT_TRUE(
        std::includes(after.rules.begin(), after.rules.end(), before.rules.begin(), before.rules.end()));
    EXPECT_EQ(PayloadInfos(after), payloadBefore);
    std::unique_ptr<TFile> file(TFile::Open(filename.c_str(), "READ"));
    ASSERT_NE(file, nullptr);
    EXPECT_NE(file->Get<TNamed>("unloaded-update"), nullptr);
    EXPECT_NE(file->Get<TNamed>("loaded-update-1"), nullptr);
    EXPECT_NE(file->Get<TNamed>("loaded-update-2"), nullptr);
    EXPECT_EQ(RunProcess(REST_IO_VERIFIER_V1, {filename.string()}), 0);
    EXPECT_EQ(RunProcess(REST_IO_VERIFIER, {filename.string(), "require-update"}), 0);
}

TEST(RootIO, TransactionalMergeRetainsTargetOnlySchemaAndKeys) {
    TemporaryDirectory temporary;
    const fs::path target = temporary.Path() / "target.root";
    const fs::path input = temporary.Path() / "input.root";

    {
        auto file = TRestRootFileHandle::Open(target.string(), TRestRootFileMode::Recreate);
        ASSERT_TRUE(file) << file.Error();
        TH1D targetOnly("target-only", "target-only", 10, 0, 10);
        targetOnly.Fill(4);
        targetOnly.Write();
        TTree events("events", "events");
        int value = 0;
        events.Branch("value", &value);
        events.Fill();
        events.Fill();
        events.Write();
        auto* nested = file->mkdir("nested");
        ASSERT_NE(nested, nullptr);
        nested->cd();
        TNamed targetNested("target-nested", "target nested key");
        targetNested.Write();
        ASSERT_TRUE(file.Close()) << file.Error();
    }
    const SchemaInventory targetSchema = ReadInventory(target);
    {
        auto file = TRestRootFileHandle::Open(input.string(), TRestRootFileMode::Recreate);
        ASSERT_TRUE(file) << file.Error();
        TGraph incoming;
        incoming.SetName("incoming");
        incoming.SetPoint(0, 1, 2);
        incoming.Write();
        TTree events("events", "events");
        int value = 0;
        events.Branch("value", &value);
        events.Fill();
        events.Fill();
        events.Fill();
        events.Write();
        auto* nested = file->mkdir("nested");
        ASSERT_NE(nested, nullptr);
        nested->cd();
        TNamed inputNested("input-nested", "input nested key");
        inputNested.Write();
        ASSERT_TRUE(file.Close()) << file.Error();
    }

    std::string error;
    ASSERT_TRUE(TRestTools::MergeRootFilesTransactionally(target.string(), {input.string()}, target.string(),
                                                          false, &error))
        << error;

    std::unique_ptr<TFile> merged(TFile::Open(target.c_str(), "READ"));
    ASSERT_NE(merged, nullptr);
    EXPECT_NE(merged->Get<TH1D>("target-only"), nullptr);
    EXPECT_NE(merged->Get<TGraph>("incoming"), nullptr);
    EXPECT_NE(merged->Get<TNamed>("nested/target-nested"), nullptr);
    EXPECT_NE(merged->Get<TNamed>("nested/input-nested"), nullptr);
    auto* events = merged->Get<TTree>("events");
    ASSERT_NE(events, nullptr);
    EXPECT_EQ(events->GetEntries(), 5);
    const SchemaInventory finalSchema = ReadInventory(target);
    EXPECT_TRUE(std::includes(finalSchema.infos.begin(), finalSchema.infos.end(), targetSchema.infos.begin(),
                              targetSchema.infos.end()));
}

TEST(RootIO, TransactionRejectsIncompatibleSameNameClasses) {
    TemporaryDirectory temporary;
    const fs::path target = temporary.Path() / "target.root";
    const fs::path input = temporary.Path() / "input.root";
    {
        auto file = TRestRootFileHandle::Open(target.string(), TRestRootFileMode::Recreate);
        ASSERT_TRUE(file) << file.Error();
        TNamed conflict("conflict", "target class");
        conflict.Write();
        ASSERT_TRUE(file.Close()) << file.Error();
    }
    {
        auto file = TRestRootFileHandle::Open(input.string(), TRestRootFileMode::Recreate);
        ASSERT_TRUE(file) << file.Error();
        TH1D conflict("conflict", "input class", 10, 0, 10);
        conflict.Write();
        ASSERT_TRUE(file.Close()) << file.Error();
    }
    const auto targetBefore = ReadBytes(target);
    const auto inputBefore = ReadBytes(input);

    std::string error;
    EXPECT_FALSE(TRestTools::MergeRootFilesTransactionally(target.string(), {input.string()}, target.string(),
                                                           true, &error));
    EXPECT_NE(error.find("incompatible classes"), std::string::npos);
    EXPECT_EQ(ReadBytes(target), targetBefore);
    EXPECT_EQ(ReadBytes(input), inputBefore);
}

TEST(RootIO, TransactionFailurePreservesOriginalAndSources) {
    TemporaryDirectory temporary;
    const fs::path target = temporary.Path() / "target.root";
    const fs::path input = temporary.Path() / "input.root";
    const fs::path missing = temporary.Path() / "missing.root";
    {
        auto file = TRestRootFileHandle::Open(target.string(), TRestRootFileMode::Recreate);
        ASSERT_TRUE(file) << file.Error();
        TNamed original("original", "must survive");
        original.Write();
        ASSERT_TRUE(file.Close()) << file.Error();
    }
    {
        auto file = TRestRootFileHandle::Open(input.string(), TRestRootFileMode::Recreate);
        ASSERT_TRUE(file) << file.Error();
        TNamed incoming("incoming", "must survive");
        incoming.Write();
        ASSERT_TRUE(file.Close()) << file.Error();
    }
    const auto targetBefore = ReadBytes(target);
    const auto inputBefore = ReadBytes(input);

    std::string error;
    EXPECT_FALSE(TRestTools::MergeRootFilesTransactionally(
        target.string(), {input.string(), missing.string()}, target.string(), true, &error));
    EXPECT_FALSE(error.empty());
    EXPECT_EQ(ReadBytes(target), targetBefore);
    EXPECT_TRUE(fs::exists(input));
    EXPECT_EQ(ReadBytes(input), inputBefore);
    EXPECT_FALSE(fs::exists(missing));
}

TEST(RootIO, PostReplacementValidationFailureRollsBackAndKeepsSources) {
    TemporaryDirectory temporary;
    const fs::path target = temporary.Path() / "target.root";
    const fs::path input = temporary.Path() / "input.root";
    {
        auto file = TRestRootFileHandle::Open(target.string(), TRestRootFileMode::Recreate);
        ASSERT_TRUE(file) << file.Error();
        TNamed original("original", "must be restored byte-for-byte");
        original.Write();
        ASSERT_TRUE(file.Close()) << file.Error();
    }
    {
        auto file = TRestRootFileHandle::Open(input.string(), TRestRootFileMode::Recreate);
        ASSERT_TRUE(file) << file.Error();
        TNamed incoming("incoming", "must not be removed after rollback");
        incoming.Write();
        ASSERT_TRUE(file.Close()) << file.Error();
    }
    const auto targetBefore = ReadBytes(target);
    const auto inputBefore = ReadBytes(input);

    TRestTools::ForceNextTransactionalMergeValidationFailureForTesting();
    std::string error;
    EXPECT_FALSE(TRestTools::MergeRootFilesTransactionally(target.string(), {input.string()}, target.string(),
                                                           true, &error));
    EXPECT_NE(error.find("Forced post-replacement"), std::string::npos);
    EXPECT_EQ(ReadBytes(target), targetBefore);
    EXPECT_TRUE(fs::exists(input));
    EXPECT_EQ(ReadBytes(input), inputBefore);
}

TEST(RootIO, SplitMetadataUsesExplicitDestination) {
    TemporaryDirectory temporary;
    const fs::path mainName = temporary.Path() / "main.root";
    const fs::path splitName = temporary.Path() / "split.root";
    auto main = TRestRootFileHandle::Open(mainName.string(), TRestRootFileMode::Recreate);
    auto split = TRestRootFileHandle::Open(splitName.string(), TRestRootFileMode::Recreate);
    ASSERT_TRUE(main) << main.Error();
    ASSERT_TRUE(split) << split.Error();

    split->cd();
    TRestProcessRunner runner;
    runner.WriteProcessesMetadata(main.Get());
    ASSERT_TRUE(main.Close()) << main.Error();
    ASSERT_TRUE(split.Close()) << split.Error();

    EXPECT_TRUE(HasKeyClass(mainName, "TRestProcessRunner"));
    EXPECT_FALSE(HasKeyClass(splitName, "TRestProcessRunner"));
}

TEST(RootIO, RemoteDestinationsAreRejectedButReadPolicyIsPermissive) {
    const std::string remote = "root://127.0.0.1:1/not-present.root";
    EXPECT_TRUE(TRestTools::IsRemoteRootPath(remote));
    auto update = TRestRootFileHandle::Open(remote, TRestRootFileMode::Update);
    EXPECT_FALSE(update);
    EXPECT_NE(update.Error().find("refuses to mutate remote"), std::string::npos);
    auto recreate =
        TRestRootFileHandle::Open("https://127.0.0.1/not-present.root", TRestRootFileMode::Recreate);
    EXPECT_FALSE(recreate);
    EXPECT_NE(recreate.Error().find("refuses to mutate remote"), std::string::npos);

    auto read = TRestRootFileHandle::Open(remote, TRestRootFileMode::Read);
    EXPECT_FALSE(read);
    EXPECT_EQ(read.Error().find("refuses"), std::string::npos);
}

TEST(RootIO, FileUrlReadAndMoveLifecycle) {
    TemporaryDirectory temporary;
    const fs::path filename = fs::absolute(temporary.Path() / "lifecycle.root");
    auto owner = TRestRootFileHandle::Open(filename.string(), TRestRootFileMode::Recreate);
    ASSERT_TRUE(owner) << owner.Error();
    auto moved = std::move(owner);
    EXPECT_FALSE(owner);
    TNamed marker("marker", "value");
    marker.Write();
    ASSERT_TRUE(moved.Close()) << moved.Error();

    const std::string fileUrl = "file://" + filename.string();
    EXPECT_FALSE(TRestTools::IsRemoteRootPath(fileUrl));
    auto read = TRestRootFileHandle::Open(fileUrl, TRestRootFileMode::Read);
    ASSERT_TRUE(read) << read.Error();
    EXPECT_NE(read->Get<TNamed>("marker"), nullptr);
}

TEST(RootIO, TRestRunRejectsUnsupportedInputModes) {
    TemporaryDirectory temporary;
    const fs::path filename = temporary.Path() / "input.root";
    {
        auto file = TRestRootFileHandle::Open(filename.string(), TRestRootFileMode::Recreate);
        ASSERT_TRUE(file) << file.Error();
        ASSERT_TRUE(file.Close()) << file.Error();
    }
    EXPECT_EXIT(
        {
            TRestRun run;
            run.OpenInputFile(filename.c_str(), "RECREATE");
        },
        ::testing::ExitedWithCode(1), ".*");
}
