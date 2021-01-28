- **TRestAnalysisHistogramViewerProcess** is a Track Process. However, it seems as it just access the analysis tree, therefore it could be perfectly covered by other generic process.
- **TRestTrackLinearizarionProcess** is a process used to obtain the energy profile along the track from a TRestTrack. It is connected to TRestLinearTrack. We need to think if this is being used, and if it is worth to create a new library. Perhaps it could be integrated inside TRestTrackEvent.
- **TRestRawFFT**: It was used previously with TRestSignal type, now it must be reviewed and adapted to be used with TRestRawSignal which is more appropiate.
- **TRestPointLikeAnaProcess**: Used by J.Gracia to identify X-ray like events.
- **TRestSignalDeconvolutionProcess**: A process to deconvolute a rawsignal pulse using its known response function. It is quarantine because together with TRestFFT it should be adapted to TRestRawSignal.
- **TRestLinearTrackXXX**: Few classes that are used to find projection of a track along its path. Could go on RestTrackLib. But it is not in final state, and dont see the time to work on it.


