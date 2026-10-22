#include <TRestAnalysisTree.h>
#include <TRestRun.h>

#include <cmath>
#include <cstdlib>
#include <string>

namespace {
bool Near(double left, double right) { return std::abs(left - right) < 1e-12; }
}  // namespace

int main(int argc, char** argv) {
    if (argc != 5) return 2;
    const double expectedTheta = std::stod(argv[2]);
    const double expectedPhi = std::stod(argv[3]);
    const double expectedEnergy = std::stod(argv[4]);

    TRestRun run(argv[1]);
    TRestAnalysisTree* tree = run.GetAnalysisTree();
    if (tree == nullptr || tree->GetEntries() <= 100) return 3;

    if (tree->Draw("theta", "", "goff", 1, 100) != 1 || !Near(tree->GetV1()[0], expectedTheta)) return 4;
    if (tree->Draw("phi", "", "goff", 1, 100) != 1 || !Near(tree->GetV1()[0], expectedPhi)) return 5;
    if (tree->Draw("totalEdep", "", "goff", 1, 100) != 1 || !Near(tree->GetV1()[0], expectedEnergy)) return 6;

    run.GetEntry(100);
    if (!Near(tree->GetObservableValue<double>("theta"), expectedTheta)) return 7;
    if (!Near(tree->GetObservableValue<double>("phi"), expectedPhi)) return 8;
    if (!Near(tree->GetObservableValue<double>("totalEdep"), expectedEnergy)) return 9;
    return EXIT_SUCCESS;
}
