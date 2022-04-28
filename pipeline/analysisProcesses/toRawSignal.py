import numpy as np

import ROOT


def load_REST():
    import ROOT
    ROOT.gSystem.Load("libRestFramework.so")
    ROOT.gSystem.Load("libRestConnectors.so")
    ROOT.gSystem.Load("libRestDetector.so")
    ROOT.gSystem.Load("libRestRaw.so")
    ROOT.gSystem.Load("libRestTrack.so")
    ROOT.gSystem.Load("libRestGeant4.so")


load_REST()


# Create a fake detector event

def generate_random_detector_event() -> ROOT.TRestDetectorSignalEvent:
    event = ROOT.TRestDetectorSignalEvent()
    time_range = (0.0, 10000.0)  # ns
    energy_range = (1.0, 500.0)  # keV
    for signal_id in range(10):
        n_hits = np.random.randint(50, 200)
        time = np.random.rand(n_hits) * (time_range[1] - time_range[0]) + time_range[0]
        energy = np.random.rand(n_hits) * (energy_range[1] - energy_range[0]) + energy_range[0]

        for i in range(n_hits):
            event.AddChargeToSignal(signal_id, time[i], energy[i])

    event.SortSignals()
    return event


if __name__ == "__main__":
    event = generate_random_detector_event()
    # event.PrintEvent()

    detectorSignalToRawSignalProcess = ROOT.TRestDetectorSignalToRawSignalProcess()

    detectorSignalToRawSignalProcess.SetSampling(10.0)
    detectorSignalToRawSignalProcess.SetGain(5.0)

    detectorSignalToRawSignalProcess.PrintMetadata()

    event = detectorSignalToRawSignalProcess.ProcessEvent(event)

    # event.PrintEvent()

    rawSignalShapingProcess = ROOT.TRestRawSignalShapingProcess()

    rawSignalShapingProcess.SetShapingTime(5.0)

    rawSignalShapingProcess.PrintMetadata()

    event = rawSignalShapingProcess.ProcessEvent(event)

    # event.PrintEvent()
