#include <TBranch.h>
#include <TFile.h>
#include <TKey.h>
#include <TLeaf.h>
#include <TTree.h>

#include <iostream>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

#include "TrackFixture.h"

namespace {
void Require(bool ok, const char* message) {
    if (!ok) throw std::runtime_error(message);
}

std::vector<char> Bytes(TFile& file, Long64_t offset, Int_t size) {
    Require(offset > 0 && size > 0, "invalid record range");
    std::vector<char> bytes(size);
    Require(!file.ReadBuffer(bytes.data(), offset, size), "cannot read record");
    return bytes;
}

void CheckBranch(TFile& source, TBranch& oldBranch, TFile& fixed, TBranch& newBranch) {
    Require(std::string(oldBranch.GetName()) == newBranch.GetName(), "branch name changed");
    Require(oldBranch.GetEntries() == newBranch.GetEntries(), "branch entries changed");
    Require(oldBranch.GetTotBytes() == newBranch.GetTotBytes(), "branch total bytes changed");
    Require(oldBranch.GetZipBytes() == newBranch.GetZipBytes(), "branch compressed bytes changed");
    Require(oldBranch.GetMaxBaskets() == newBranch.GetMaxBaskets(), "basket inventory changed");
    for (Int_t index = 0; index < oldBranch.GetMaxBaskets(); ++index) {
        const auto seek = oldBranch.GetBasketSeek(index);
        const auto size = oldBranch.GetBasketBytes()[index];
        Require(seek == newBranch.GetBasketSeek(index), "unaffected basket moved");
        Require(size == newBranch.GetBasketBytes()[index], "unaffected basket size changed");
        if (seek > 0 && size > 0)
            Require(Bytes(source, seek, size) == Bytes(fixed, seek, size), "unaffected basket changed");
    }
    const auto* children = oldBranch.GetListOfBranches();
    Require(children->GetEntries() == newBranch.GetListOfBranches()->GetEntries(), "subtree changed");
    for (Int_t index = 0; index < children->GetEntries(); ++index)
        CheckBranch(source, *static_cast<TBranch*>(children->At(index)), fixed,
                    *static_cast<TBranch*>(newBranch.GetListOfBranches()->At(index)));
}

void AppendTrack(const char* path) {
    TFile file(path, "UPDATE");
    auto* tree = file.Get<TTree>("EventTree");
    Require(tree != nullptr && tree->GetEntries() == 3, "unexpected base fixture");
    auto* event = new RecoveryTrackEvent;
    auto* branch = tree->Branch("RecoveryTrackEventBranch", &event, 128, 99);
    for (Int_t entry = 0; entry < 3; ++entry) {
        event->fTrack.clear();
        for (Int_t index = 0; index <= entry; ++index) {
            RecoveryTrack track;
            track.SetUniqueID(1000 + 10 * entry + index);
            track.fID = 10 * entry + index;
            track.fEnergy = entry + 0.25 * index;
            event->fTrack.push_back(track);
        }
        Require(branch->Fill() > 0 && branch->FlushBaskets() >= 0, "cannot write track fixture");
    }
    // Multiple old header cycles must not leave a legacy signal branch reachable.
    Require(tree->Write() > 0 && tree->Write() > 0, "cannot write header cycles");
    tree->ResetBranchAddress(branch);
    delete event;
}

void Verify(const char* sourcePath, const char* fixedPath) {
    TFile source(sourcePath, "READ");
    TFile fixed(fixedPath, "READ");
    auto* oldTree = source.Get<TTree>("EventTree");
    auto* newTree = fixed.Get<TTree>("EventTree");
    Require(oldTree != nullptr && newTree != nullptr, "missing EventTree");
    Require(newTree->GetEntries() == 3, "incorrect recovered entry count");
    TIter branches(oldTree->GetListOfBranches());
    while (auto* branch = static_cast<TBranch*>(branches())) {
        if (std::string(branch->GetName()) == "TRestDetectorSignalEventBranch") continue;
        auto* recovered = newTree->GetBranch(branch->GetName());
        Require(recovered != nullptr, "unaffected branch lost");
        CheckBranch(source, *branch, fixed, *recovered);
    }
    TIter keys(source.GetListOfKeys());
    while (auto* key = static_cast<TKey*>(keys())) {
        if (std::string(key->GetName()) == "EventTree") continue;
        auto* recovered = fixed.GetKey(key->GetName(), key->GetCycle());
        Require(recovered && recovered->GetSeekKey() == key->GetSeekKey() &&
                    recovered->GetNbytes() == key->GetNbytes(),
                "unaffected key moved");
        Require(Bytes(source, key->GetSeekKey(), key->GetNbytes()) ==
                    Bytes(fixed, recovered->GetSeekKey(), recovered->GetNbytes()),
                "unaffected key changed");
    }
    Int_t treeCycles = 0;
    TIter newKeys(fixed.GetListOfKeys());
    while (auto* key = static_cast<TKey*>(newKeys()))
        if (std::string(key->GetName()) == "EventTree") ++treeCycles;
    Require(treeCycles == 1, "obsolete EventTree cycles remain");

    std::set<TBranch*> owners;
    const auto collect = [&](auto&& self, TObjArray* array) -> void {
        TIter next(array);
        while (auto* branch = static_cast<TBranch*>(next())) {
            owners.insert(branch);
            self(self, branch->GetListOfBranches());
        }
    };
    collect(collect, newTree->GetListOfBranches());
    TIter leaves(newTree->GetListOfLeaves());
    while (auto* leaf = static_cast<TLeaf*>(leaves()))
        Require(owners.count(leaf->GetBranch()) != 0, "dangling leaf after signal replacement");

    Long64_t total = 0, zipped = 0;
    TIter newBranches(newTree->GetListOfBranches());
    while (auto* branch = static_cast<TBranch*>(newBranches())) {
        total += branch->GetTotBytes("*");
        zipped += branch->GetZipBytes("*");
    }
    Require(total == newTree->GetTotBytes() && zipped == newTree->GetZipBytes(),
            "incorrect tree byte counters");

    auto* event = new RecoveryTrackEvent;
    auto* branch = newTree->GetBranch("RecoveryTrackEventBranch");
    Require(newTree->SetBranchAddress(branch->GetName(), &event) >= TTree::kMatch, "cannot bind old tracks");
    for (Int_t entry = 0; entry < 3; ++entry) {
        Require(branch->GetEntry(entry) > 0, "cannot read old tracks");
        Require(event->fTrack.size() == static_cast<std::size_t>(entry + 1), "track count changed");
        for (Int_t index = 0; index <= entry; ++index) {
            const auto& track = event->fTrack[index];
            Require(track.GetUniqueID() == static_cast<UInt_t>(1000 + 10 * entry + index) &&
                        track.fID == 10 * entry + index && track.fEnergy == entry + 0.25 * index,
                    "historical track values changed");
        }
    }
    newTree->ResetBranchAddress(branch);
    delete event;
}
}  // namespace

int main(int argc, char** argv) {
    try {
        if (argc == 3 && std::string(argv[1]) == "append")
            AppendTrack(argv[2]);
        else if (argc == 4 && std::string(argv[1]) == "verify")
            Verify(argv[2], argv[3]);
        else
            return 2;
        return 0;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
