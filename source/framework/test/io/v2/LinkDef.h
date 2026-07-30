#ifdef __CLING__
#pragma link C++ class TRestIOFixturePayload+;
#pragma link C++ class std::vector<double>+;
#pragma read sourceClass = "TRestIOFixturePayload" version = "[1]" \
    source = "int fLegacyCode" targetClass = "TRestIOFixturePayload" \
    target = "fRenamedCode" code = "{ fRenamedCode = onfile.fLegacyCode; }"
#endif
