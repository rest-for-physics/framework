///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDaqChannelSwitchingProcess.cxx
///
///
///             First implementation of raw signal analysis process into REST_v2
///             Created from TRestSignalAnalysisProcess
///             Date : Jun/2018
///             Author : Ni Kaixiang
///
///_______________________________________________________________________________

#include <TLegend.h>
#include <TPaveText.h>

#include "TRestDaqChannelSwitchingProcess.h"
using namespace std;

ClassImp(TRestDaqChannelSwitchingProcess)
    //______________________________________________________________________________
    TRestDaqChannelSwitchingProcess::TRestDaqChannelSwitchingProcess() {
    Initialize();
}

//______________________________________________________________________________
TRestDaqChannelSwitchingProcess::~TRestDaqChannelSwitchingProcess() {}

//______________________________________________________________________________
void TRestDaqChannelSwitchingProcess::Initialize() {
    SetSectionName(this->ClassName());

    fFirstDaqChannelDef.clear();
    fIgnoreUndefinedModules = false;
    fReadout = NULL;
    fEvent = NULL;
}

//______________________________________________________________________________
void TRestDaqChannelSwitchingProcess::InitProcess() {
    fReadout = GetMetadata<TRestReadout>();
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
                TRestReadoutPlane& plane = (*fReadout)[i];

                for (int j = 0; j < plane.GetNumberOfModules(); j++) {
                    TRestReadoutModule& mod = plane[j];
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
TRestEvent* TRestDaqChannelSwitchingProcess::ProcessEvent(TRestEvent* eventInput) {
    fEvent = eventInput;
    return eventInput;
}

//______________________________________________________________________________
void TRestDaqChannelSwitchingProcess::EndProcess() {}

//______________________________________________________________________________
// redefining module's first daq channel:
// <module id="1" firstdaqchannel="136*3" />
// ignore undefined modules modules by setting their channel's daq id to -1e9
// <parameter name="ignoreUndefinedModules" value="true" />
void TRestDaqChannelSwitchingProcess::InitFromConfigFile() {
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
