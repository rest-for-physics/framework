#include <TFile.h>
#include <TNamed.h>
#include <TRestTools.h>
#include <TTree.h>

#include <chrono>
#include <filesystem>
#include <iostream>

int main(int argc, char** argv) {
    if (argc != 2) return 2;
    namespace fs = std::filesystem;
    const fs::path dir = argv[1];
    fs::create_directories(dir);
    const auto target = (dir / "target.root").string();
    const auto worker = (dir / "worker.root").string();
    {
        auto file = TRestRootFileHandle::Open(target, TRestRootFileMode::Recreate);
        if (!file) return 3;
        TTree tree("events", "events");
        double value = 0;
        tree.Branch("value", &value);
        for (int i = 0; i < 100000; ++i) {
            value = i * 0.12345;
            tree.Fill();
        }
        tree.Write();
        if (!file.Close()) return 4;
    }
    {
        auto file = TRestRootFileHandle::Open(worker, TRestRootFileMode::Recreate);
        if (!file) return 5;
        TNamed marker("worker", "worker");
        marker.Write();
        if (!file.Close()) return 6;
    }
    const auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < 100; ++i) {
        auto file = TRestRootFileHandle::Open(target, TRestRootFileMode::Update);
        if (!file || !file.Close()) return 7;
    }
    const auto updated = std::chrono::steady_clock::now();
    for (int i = 0; i < 30; ++i) {
        std::string error;
        if (!TRestTools::MergeRootFilesTransactionally(target, {worker}, target, false, &error)) {
            std::cerr << error << '\n';
            return 8;
        }
    }
    const auto merged = std::chrono::steady_clock::now();
    std::cout << "update100_ms=" << std::chrono::duration<double, std::milli>(updated - start).count()
              << " merge30_ms=" << std::chrono::duration<double, std::milli>(merged - updated).count()
              << " file_bytes=" << fs::file_size(target) << '\n';
}
