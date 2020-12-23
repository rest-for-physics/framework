///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDetectorDaqChannelSwitchingProcess.cxx
///
///
///             First implementation of raw signal analysis process into REST_v2
///             Created from TRestDetectorSignalAnalysisProcess
///             Date : Jun/2018
///             Author : Ni Kaixiang
///
///_______________________________________________________________________________

#include <TLegend.h>
#include <TPaveText.h>

#include "TRestDetectorDaqChannelSwitchingProcess.h"
using namespace std;

ClassImp(TRestDetectorDaqChannelSwitchingProcess)
    //______________________________________________________________________________
    TRestDetectorDaqChannelSwitchingProcess::TRestDetectorDaqChannelSwitchingProcess() {
    Initialize();
}

//______________________________________________________________________________
TRestDetectorDaqChannelSwitchingProcess::~TRestDetectorDaqChannelSwitchingProcess() {}

//______________________________________________________________________________
void TRestDetectorDaqChannelSwitchingProcess::Initialize() {
    SetSectionName(this->ClassName());

    fFirstDaqChannelDef.clear();
    fIgnoreUndefinedModules = false;
    fReadout = NULL;
    fEvent = NULL;
}

//______________________________________________________________________________
void TRestDetectorDaqChannelSwitchingProcess::InitProcess() {
    fReadout = GetMetadata<TRestDetectorReadout>();
    if (fReadout != NULL) {
        auto iter = fFirstDaqChannelDef.begin();
        while (iter != fFirstDaqChannelDef.end()) {
            auto mod = fReadout->GetReadoutModuleWithID(iter->first);
            if (mod == NULL) continue;
            // finding out the old "firstdaqchannel" value
            int mindaq = 1e9;
            for (int i = 0; i < mod->GetNumberOfChannels(); i++) {
                if (mod->GetChannel(i)->GetDaqID() < mindaq) {
                    mindaq = mod->GetChannel(i)->GetDaqID();
                }
            }

            // re-setting the value
            for (int i = 0; i < mod->GetNumberOfChannels(); i++) {
                mod->GetChannel(i)->SetDaqID(mod->GetChannel(i)->GetDaqID() - mindaq + iter->second);
            }

            iter++;
        }

        if (fIgnoreUndefinedModules) {
            for (int i = 0; i < fReadout->GetNumberOfReadoutPlanes(); i++) {
                TRestDetectorReadoutPlane& plane = (*fReadout)[i];

                for (int j = 0; j < plane.GetNumberOfModules(); j++) {
                    TRestDetectorReadoutModule& mod = plane[j];
                    if (fFirstDaqChannelDef.count(mod.GetModuleID()) == 0) {
                        for (int i = 0; i < mod.GetNumberOfChannels(); i++) {
                            mod.GetChannel(i)->SetDaqID(-1e9);
                        }
                    }
                }
            }
        }
    }
}

//______________________________________________________________________________
TRestEvent* TRestDetectorDaqChannelSwitchingProcess::ProcessEvent(TRestEvent* eventInput) {
    fEvent = eventInput;
    return eventInput;
}

//______________________________________________________________________________
void TRestDetectorDaqChannelSwitchingProcess::EndProcess() {}

//______________________________________________________________________________
// redefining module's first daq channel:
// <module id="1" firstdaqchannel="136*3" />
// ignore undefined modules modules by setting their channel's daq id to -1e9
// <parameter name="ignoreUndefinedModules" value="true" />
void TRestDetectorDaqChannelSwitchingProcess::InitFromConfigFile() {
    TiXmlElement* ele = fElement->FirstChildElement("module");
    while (ele != NULL) {
        int id = StringToInteger(GetParameter("id", ele));
        int channel = StringToInteger(GetParameter("firstdaqchannel", ele));
        if (id == -1 || channel == -1) continue;
        fFirstDaqChannelDef[id] = channel;
        ele = ele->NextSiblingElement("module");
    }
    fIgnoreUndefinedModules = StringToBool(GetParameter("ignoreUndefinedModules", "false"));
}
