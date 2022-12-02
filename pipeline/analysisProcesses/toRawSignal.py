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
    time_range = (0.0, 100.0 * 1000)  # ns
    energy_range = (1.0, 500.0)  # keV
    for signal_id in range(4):
        n_hits = np.random.randint(20, 50)
        time = np.random.rand(n_hits) * (time_range[1] - time_range[0]) + time_range[0]
        energy = np.random.rand(n_hits) * (energy_range[1] - energy_range[0]) + energy_range[0]

        for i in range(n_hits):
            event.AddChargeToSignal(signal_id, time[i], energy[i])

    event.SortSignals()
    return event


if __name__ == "__main__":
    np.random.seed(1)

    event = generate_random_detector_event()
    # event.PrintEvent()

    sampling_time = 100.0  # ns
    shaping_time = 1500.0  # ns

    detectorSignalToRawSignalProcess = ROOT.TRestDetectorSignalToRawSignalProcess()

    detectorSignalToRawSignalProcess.SetSampling(sampling_time)
    detectorSignalToRawSignalProcess.SetGain(5.0)

    detectorSignalToRawSignalProcess.PrintMetadata()

    event = detectorSignalToRawSignalProcess.ProcessEvent(event)

    # event.PrintEvent()
    # event.DrawEvent()

    rawSignalShapingProcess = ROOT.TRestRawSignalShapingProcess()

    rawSignalShapingProcess.SetShapingTime(shaping_time / sampling_time)

    rawSignalShapingProcess.PrintMetadata()

    event = rawSignalShapingProcess.ProcessEvent(event)

    # event.PrintEvent()
    # event.DrawEvent()

    rawSignalAddNoiseProcess = ROOT.TRestRawSignalAddNoiseProcess()

    rawSignalAddNoiseProcess.SetNoiseLevel(5.0)

    rawSignalAddNoiseProcess.PrintMetadata()

    event = rawSignalAddNoiseProcess.ProcessEvent(event)

    # event.PrintEvent()
    # event.DrawEvent()

    input()
