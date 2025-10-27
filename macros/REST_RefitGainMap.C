//////////////////////////////////////////////////////////
/// Macro to refit the gain map with a simple 'GUI' to allow
/// the user to add or delete peaks and update the fits with
/// the FIT PANEL. This macro is meant to be used with the
/// FIT PANEL. The GUI consists of a dialog canvas and two
/// graphical canvases, one with all the segments and another
/// one with the segment selected alone. The dialog canvas
/// contains a button for each module. This will draw the
/// segments of the module in the first canvas (cAll). Then
/// the user can select the segment to be shown in the second
/// canvas by clicking on the first one. When the spectrum of
/// the segment is shown in the second canvas (cAlone), the
/// user can add or delete peaks with the buttons 'Add peak'
/// and 'Delete peak'. The user can also update the fits
/// using the FIT PANEL by selecting the fit type 'Prev. Fit'
/// and fit function named 'g'+peakNumber (in energy descending
/// order starting at 0). It is important to check the option
/// 'Add to list' in the FIT PANEL (the draw option 'SAME' is
/// optional but it can be helpful), if not all the fits will
/// be deleted. After fitting with the fit panel, click on the
/// button 'UpdateFits' to update the fits in the canvas and the
/// calibration curve of that segment.
/// Finally, export the gain map with the button 'Export' on the
/// dialog canvas. The user can also export the gain map to a
/// root file with the command 'gm.Export();' in the terminal.
///
/// \author: Álvaro Ezquerro aezquerro@unizar.es
///
/// Known bugs:
/// - Sometimes when clicking the drawAlone button, the segment
///   spectrum displayed in cAlone is not the correct one. I have
///   not been able to reproduce it systematically.
/// - Crash when deleting all the peaks of a segment.
/// - Crash (seg fault) sometimes when deleting one of the peaks.
///  Although you can still continue using the macro (but it crashes
///  totally when exiting the root terminal with .q)
///
/// Possible improvements:
/// - Encapsule the canvases (and fit panel maybe?) in a proper
///   ROOT GUI.
///
//////////////////////////////////////////////////////////

// Forward declaration of TFitEditor2
class TFitEditor2;
typedef std::multimap<TObject*, TF1*>::iterator fPrevFitIter;

// Class definition for GainMapRefitter
class GainMapRefitter {
   public:
    // Member variables (formerly global variables)
    TRestDataSetGainMap gm;
    TRestDataSetGainMap::Module* m;
    std::map<std::string, double> meansAux;
    std::string fitNameAux;
    const int screenWidth;
    const int screenHeight;
    const int cAllHeight;
    const int cAllWidth;
    const int cAloneHeight;
    const int cAloneWidth;
    TCanvas* cAll;
    TCanvas* cAlone;
    TFitEditor* fitEditor;
    TFitEditor2* fitEditor2;

   public:
    GainMapRefitter()
        : m(nullptr),
          screenWidth(gClient->GetDisplayWidth()),
          screenHeight(gClient->GetDisplayHeight()),
          cAllHeight(2.0 / 3 * screenHeight),
          cAllWidth(1.25 * cAllHeight),
          cAloneHeight(0.45 * screenHeight),
          cAloneWidth(0.8 * cAllHeight),
          cAll(new TCanvas("cAll", "cAll", cAllWidth, cAllHeight)),
          cAlone(new TCanvas("cAlone", "cAlone", cAloneWidth, cAloneHeight)),
          fitEditor(nullptr),
          fitEditor2(nullptr) {}

    ~GainMapRefitter() {
        delete cAll;
        delete cAlone;
        // Note: fitEditor and fitEditor2 are managed by ROOT and not deleted here
    }

    // Method declarations
    void drawAll();
    void drawAlone(const int x, const int y);
    void drawWithinAll(int x, int y);
    void clearCanvas(TCanvas* c, size_t n_subPad = 0);
    void highlightDrawnAlonePad(const int x, const int y);
    void DeletePeak(const int x, const int y, const int peakNumber);
    void AddPeak(const int x, const int y, const int peakNumber);
    void UpdateFits(const int x, const int y);
    void changeModule(int plane, int module);
    TDialogCanvas* createDialog();
    void ImportGainMap(const std::string& gainMapFile) { gm.Import(gainMapFile); }

    // Getter for gm (if needed externally)
    TRestDataSetGainMap& GetGainMap() { return gm; }
};

// Class that inherits from TFitEditor to be able to access the protected methods of TFitEditor
class TFitEditor2 : public TFitEditor {
   public:
    TFitEditor2(TVirtualPad* pad, TObject* obj) : TFitEditor(pad, obj){};
    virtual ~TFitEditor2() {}

    // making public the protected method GetFitObjectListOfFunctions()
    TList* GetFitObjectListOfFunctions() { return TFitEditor::GetFitObjectListOfFunctions(); }
    TF1* GetFitFunction() { return TFitEditor::GetFitFunction(); }
    void CleanFitFunctionList(std::string containingString = "Prev") {
        /*// print fPrevFit
        std::cout << "Before fPrevFit:" << std::endl;
        for (auto it = fPrevFit.begin(); it != fPrevFit.end(); ++it) {
            std::cout << it->first->GetName() << " => " << it->second->GetName() << '\n';
        } //*/
        std::pair<fPrevFitIter, fPrevFitIter> look = fPrevFit.equal_range(fFitObject);
        for (fPrevFitIter it = look.first; it != look.second; ++it) {
            std::string name = it->second->GetName();
            if (name.find(containingString) != std::string::npos) {
                fPrevFit.erase(it);
                break;
            }
        }
        TFitEditor::FillFunctionList();  // update the list of functions in the fit panel
    }

    void ToggleNecessaryOptions() {
        if (fAdd2FuncList) {
            if (fAdd2FuncList->GetState() == kButtonUp) fAdd2FuncList->SetState(kButtonDown);
        }

        if (fDrawSame) {
            if (fDrawSame->GetState() == kButtonUp) fDrawSame->SetState(kButtonDown);
        }
    }

    TGComboBox* GetFunctionListComboBox() { return fFuncList; }

    std::string GetSelectedFunctionName() {
        if (!fFuncList) return "";
        return fFuncList->GetSelectedEntry()->GetTitle();
    }

    void SelectFunctionByName(const char* name) {
        if (!fFuncList) return;
        if (fFuncList->FindEntry(name))
            if (fFuncList->GetSelected() != fFuncList->FindEntry(name)->EntryId())
                fFuncList->Select(fFuncList->FindEntry(name)->EntryId());
    }
};

// Method definitions
void GainMapRefitter::clearCanvas(TCanvas* c, size_t n_subPad) {
    c->cd(n_subPad);
    if (n_subPad == 0) {
        c->GetListOfPrimitives()->Clear();
        c->Modified();
        c->Range(0, 0, 1, 1);
    } else {
        TPad* subpad = (TPad*)c->GetPad(n_subPad);
        subpad->SetFillColor(0);
        subpad->GetListOfPrimitives()->Clear();
        subpad->Modified();
        subpad->Range(0, 0, 1, 1);
    }
}

void GainMapRefitter::drawWithinAll(int x, int y) {
    size_t nSubPad = x + 1 + m->GetNumberOfSegmentsX() * (m->GetNumberOfSegmentsY() - y - 1);
    if (cAll->GetCanvasImp()) {
        clearCanvas(cAll, nSubPad);
    } else {
        cAll = new TCanvas("cAll", "cAll", cAllWidth, cAllHeight);
        cAll->Divide(m->GetNumberOfSegmentsX(), m->GetNumberOfSegmentsY());
    }
    cAll->cd(nSubPad);
    m->DrawSpectrum((size_t)x, (size_t)y, true, -1, cAll);

    std::string action = (std::string) "gRefitter->drawAlone(" + std::to_string(x) + (std::string) "," +
                         std::to_string(m->fSegSpectra[x].size() - 1 - y) + (std::string) ");";
    TButton* but = new TButton("Draw alone", action.c_str(), .5, .8, .8, .88);
    but->Draw();
    cAll->Update();
}

void GainMapRefitter::drawAlone(const int x, const int y) {
    for (TObject* obj : *m->fSegSpectra.at(x).at(y)->GetListOfFunctions()) {
        if (obj && obj->InheritsFrom(TF1::Class())) {
            TF1* g = (TF1*)obj;
            meansAux[(std::string)g->GetName()] = g->GetParameter(1);
        }
    }

    if (cAlone->GetCanvasImp()) {
        clearCanvas(cAlone);
    } else {
        cAlone = new TCanvas("cAlone", "cAlone", cAloneWidth, cAloneHeight);
    }

    cAlone->cd();
    std::string canvasTitle = "hSpc_" + std::to_string(m->GetPlaneId()) + "_" +
                              std::to_string(m->GetModuleId()) + "_" + std::to_string(x) + "_" +
                              std::to_string(y);
    cAlone->SetTitle(canvasTitle.c_str());
    m->DrawSpectrum(x, y, true, -1, cAlone);

    std::string action = "gRefitter->UpdateFits(" + std::to_string(x) + "," + std::to_string(y) + ")";
    TButton* butAlone = new TButton("UpdateFits", action.c_str(), .7, .75, .9, .825);
    butAlone->Draw();

    for (size_t n = 0; n < m->fEnergyPeaks.size(); n++) {
        bool peakHasFit = false;
        std::string objName = "g" + std::to_string(n);
        TF1* f = m->fSegSpectra[x][y]->GetFunction(objName.c_str());
        if (f) {
            std::string action = "gRefitter->DeletePeak(" + std::to_string(x) + "," + std::to_string(y) +
                                 "," + std::to_string(n) + ")";
            std::string name = "Delete energy " + DoubleToString(m->fEnergyPeaks.at(n), "%g");
            TButton* but = new TButton(name.c_str(), action.c_str(), .7, .65 - n * .05, .95, .7 - n * .05);
            but->Draw();
            peakHasFit = true;
        }
        if (!peakHasFit) {
            std::string action = "gRefitter->AddPeak(" + std::to_string(x) + "," + std::to_string(y) + "," +
                                 std::to_string(n) + ")";
            std::string name = "Add energy " + std::to_string(m->fEnergyPeaks.at(n));
            TButton* but = new TButton(name.c_str(), action.c_str(), .7, .65 - n * .05, .95, .7 - n * .05);
            but->Draw();
        }
    }
    cAlone->Update();

    highlightDrawnAlonePad(x, y);
    if (!fitEditor) {
        fitEditor = TFitEditor::GetInstance(cAlone, m->fSegSpectra.at(x).at(y));
        fitEditor2 = (TFitEditor2*)fitEditor;
    }
    fitEditor2->ToggleNecessaryOptions();
    fitEditor2->CleanFitFunctionList("Prev");
    fitEditor->Show(cAlone, m->fSegSpectra.at(x).at(y));
    fitEditor2->SelectFunctionByName(fitNameAux.c_str());
}

void GainMapRefitter::highlightDrawnAlonePad(const int x, const int y) {
    uint COLOR = 38;
    size_t nSubPad = x + 1 + m->GetNumberOfSegmentsX() * (m->GetNumberOfSegmentsY() - y - 1);
    size_t nPads = 0;
    for (const auto& object : *cAll->GetListOfPrimitives())
        if (object->InheritsFrom(TVirtualPad::Class())) ++nPads;

    for (size_t i = 0; i < nPads; i++) {
        if (i == nSubPad) continue;
        TVirtualPad* pad = (TVirtualPad*)cAll->cd(i + 1);
        if (pad->GetFillColor() != 0) {
            pad->SetFillColor(0);
            pad->Modified();
            pad->Update();
        }
    }

    if (nSubPad > nPads) {
        std::cout << "Error: the number of pads is " << nPads << " and the selected pad is " << nSubPad
                  << std::endl;
        return;
    }
    TVirtualPad* pad = (TVirtualPad*)cAll->cd(nSubPad);
    if (pad->GetFillColor() == COLOR) return;
    pad->SetFillColor(COLOR);
    pad->Modified();
    pad->Update();
}

void GainMapRefitter::DeletePeak(const int x, const int y, const int peakNumber) {
    TH1F* h = m->fSegSpectra.at(x).at(y);
    TGraph* gr = m->fSegLinearFit.at(x).at(y);
    std::string objName = "g" + std::to_string(peakNumber);
    TF1* f = h->GetFunction(objName.c_str());
    if (f) {
        h->GetListOfFunctions()->Remove(f);
        gr->RemovePoint(peakNumber);
        fitEditor2->CleanFitFunctionList(objName.c_str());
    }

    drawWithinAll(x, y);
    drawAlone(x, y);
}

void GainMapRefitter::AddPeak(const int x, const int y, const int peakNumber) {
    TH1F* h = m->fSegSpectra.at(x).at(y);
    std::string objName = "g" + std::to_string(peakNumber);
    TF1* g = new TF1(objName.c_str(), "gaus", meansAux[objName] * 0.8, meansAux[objName] * 1.2);
    h->Fit(g, "R+Q0");

    drawWithinAll(x, y);
    drawAlone(x, y);
}

void GainMapRefitter::UpdateFits(const int x, const int y) {
    TH1F* h = m->fSegSpectra.at(x).at(y);
    TList* list = h->GetListOfFunctions();
    for (size_t n = 0; n < m->fEnergyPeaks.size(); n++) {
        std::string objName = "g" + std::to_string(n);
        TF1* firstFit = h->GetFunction(objName.c_str());
        TF1* lastFit = nullptr;
        for (int i = list->GetSize() - 1; i >= 0; i--) {
            TObject* obj = list->At(i);
            if (obj && obj->InheritsFrom(TF1::Class()) && obj->GetName() == objName) {
                if (!lastFit)
                    lastFit = (TF1*)list->Remove(obj);
                else
                    list->Remove(obj);
            }
        }

        if (firstFit && lastFit && firstFit != lastFit) lastFit->Copy(*firstFit);
        if (firstFit) h->GetListOfFunctions()->Add(firstFit);

        if (h->GetFunction(objName.c_str())) m->UpdateCalibrationFits(x, y);
    }

    drawWithinAll(x, y);
    fitNameAux = fitEditor2->GetSelectedFunctionName();
    drawAlone(x, y);

    std::cout << std::endl;
    std::cout << "Segment " << x << ", " << y << std::endl;
    for (TObject* obj : *h->GetListOfFunctions()) {
        if (obj && obj->InheritsFrom(TF1::Class())) {
            TF1* f = (TF1*)obj;
            std::cout << "Function " << f->GetName() << std::endl;
            std::cout << "\tmean : " << f->GetParameter(1) << std::endl;
            std::cout << "\tsigma : " << f->GetParameter(2) << std::endl;
        }
    }

    TGraph* gr = m->fSegLinearFit.at(x).at(y);
    if (!gr) return;
    std::cout << "Graph " << gr->GetName() << std::endl;
    for (int i = 0; i < gr->GetN(); i++) {
        double x, y;
        gr->GetPoint(i, x, y);
        std::cout << "\tPoint " << i << ": " << x << ", " << y << std::endl;
    }
    for (TObject* obj : *gr->GetListOfFunctions()) {
        if (obj && obj->InheritsFrom(TF1::Class())) {
            TF1* f = (TF1*)obj;
            std::cout << "Function " << f->GetName() << std::endl;
            std::cout << "\tslope : " << f->GetParameter(1) << std::endl;
            std::cout << "\tintercept : " << f->GetParameter(0) << std::endl;
        }
    }
    std::cout << std::endl;
}

void GainMapRefitter::drawAll() {
    cAll->cd();
    if (cAll->GetCanvasImp()) {
        clearCanvas(cAll);
    } else {
        cAll = new TCanvas("cAll", "cAll", 900, 700);
    }

    m->DrawSpectrum(true, -1, cAll);
    for (size_t i = 0; i < m->fSegSpectra.size(); i++) {
        for (size_t j = 0; j < m->fSegSpectra[i].size(); j++) {
            cAll->cd(i + 1 + m->fSegSpectra[i].size() * j);
            std::string action = (std::string) "gRefitter->drawAlone(" + std::to_string(i) +
                                 (std::string) "," + std::to_string(m->fSegSpectra[i].size() - 1 - j) +
                                 (std::string) ");";
            TButton* but = new TButton("Draw alone", action.c_str(), .5, .8, .8, .88);
            but->Draw();
        }
    }
    cAll->Update();
}

void GainMapRefitter::changeModule(int plane, int module) {
    if (m) {
        for (size_t i = 0; i < m->fSegSpectra.size(); i++)
            for (size_t j = 0; j < m->fSegSpectra[i].size(); j++) m->UpdateCalibrationFits(i, j);
    }
    m = gm.GetModule(plane, module);
    if (!m) return;

    if (cAlone->GetCanvasImp()) {
        clearCanvas(cAlone);
        cAlone->SetTitle("cAlone");
        cAlone->Update();
    }

    drawAll();
    cAll->SetTitle(
        ((std::string) "Plane " + std::to_string(plane) + (std::string) ", Module " + std::to_string(module))
            .c_str());
    cAll->Update();
}

TDialogCanvas* GainMapRefitter::createDialog() {
    double width = 300, height = 100;
    int nPlanes = gm.GetNumberOfPlanes();
    int nModules = gm.GetModuleIDs(*gm.GetPlaneIDs().begin()).size();
    width = width * nPlanes;
    height = height * nModules;
    TDialogCanvas* dialog = new TDialogCanvas("Module selection", "", width, height);
    dialog->GetCanvasImp()->SetWindowPosition(25, 50);

    int i = 0;
    for (auto pm : gm.GetPlaneIDs()) {
        int j = 0;
        for (auto mm : gm.GetModuleIDs(pm)) {
            std::string action =
                "gRefitter->changeModule(" + std::to_string(pm) + "," + std::to_string(mm) + ");";
            std::string name = "Plane " + std::to_string(pm) + ", Module " + std::to_string(mm);
            TButton* but =
                new TButton(name.c_str(), action.c_str(), .1 + i * .8 / nPlanes, .4 + j * .4 / nModules,
                            .1 + (i + 1) * .8 / nPlanes, .2 + (j + 1) * .4 / nModules);
            but->Draw();
            j++;
        }
        i++;
    }

    TButton* butExport = new TButton("Export", "gRefitter->gm.Export();", .4, .1, .6, .3);
    butExport->Draw();
    return dialog;
}

// Global pointer to GainMapRefitter instance. It needs to be global because ROOT's button actions
// are executed in the global scope by CINT/Cling.
GainMapRefitter* gRefitter = nullptr;
// Main function to initialize the class
void REST_RefitGainMap(std::string gainMapFile) {
    gRefitter = new GainMapRefitter();
    gRefitter->ImportGainMap(gainMapFile);

    std::cout << std::endl;
    std::cout << " ************************************************" << std::endl;
    std::cout << " *************** REFITTING MACRO ****************" << std::endl;
    std::cout << " * This macro is meant to be used with the FIT  *" << std::endl;
    std::cout << " * PANEL. First steps:                          *" << std::endl;
    std::cout << " *                                              *" << std::endl;
    std::cout << " * 1. Select the module in the dialog canvas.   *" << std::endl;
    std::cout << " *                                              *" << std::endl;
    std::cout << " * 2. Click on the button 'Draw alone' of the   *" << std::endl;
    std::cout << " * segment you want to refit. The segment       *" << std::endl;
    std::cout << " * spectrum will be shown in the small canvas   *" << std::endl;
    std::cout << " * and the Fit Panel will open. It should auto- *" << std::endl;
    std::cout << " * matically select the histogram of the corres-*" << std::endl;
    std::cout << " * ponding segment. Check that the fit type is  *" << std::endl;
    std::cout << " * 'Prev. Fit'. The fit function are named      *" << std::endl;
    std::cout << " * 'g'+peak number (in energy descending order  *" << std::endl;
    std::cout << " * starting at 0). For example:                 *" << std::endl;
    std::cout << " * - g0 is 22.5keV                              *" << std::endl;
    std::cout << " * - g1 is 8.0keV                               *" << std::endl;
    std::cout << " *                                              *" << std::endl;
    std::cout << " * It's important to check option 'Add to list' *" << std::endl;
    std::cout << " * in the FIT PANEL (the draw option 'SAME' is  *" << std::endl;
    std::cout << " * optional but it is helpful).                 *" << std::endl;
    std::cout << " *                                              *" << std::endl;
    std::cout << " * 3. After selecting the desired peak function *" << std::endl;
    std::cout << " * ('g0', 'g1'...), select the range with the   *" << std::endl;
    std::cout << " * bottom 'X' named scroller and click on the   *" << std::endl;
    std::cout << " * button 'Fit'.                                *" << std::endl;
    std::cout << " *                                              *" << std::endl;
    std::cout << " * 4. After fitting with the fit panel, click   *" << std::endl;
    std::cout << " * on the button 'UpdateFits' to update the     *" << std::endl;
    std::cout << " * fits in the canvas and the calibration curve *" << std::endl;
    std::cout << " * of that segment.                             *" << std::endl;
    std::cout << " *                                              *" << std::endl;
    std::cout << " * 5. Repeat steps 2-4 for all the segments of  *" << std::endl;
    std::cout << " * the module and step 1 to change the module.  *" << std::endl;
    std::cout << " *                                              *" << std::endl;
    std::cout << " * 6. Export the gain map with the button       *" << std::endl;
    std::cout << " * 'Export' on the dialog canvas.               *" << std::endl;
    std::cout << " ************************************************" << std::endl;
    std::cout << std::endl;

    auto dialog = gRefitter->createDialog();

    Int_t x = 0, y = 0;
    UInt_t w = 0, h = 0;
    dialog->GetCanvasImp()->GetWindowGeometry(x, y, w, h);

    gRefitter->cAll->SetWindowPosition(
        x,
        y + 35 +
            100 *
                gRefitter->GetGainMap().GetModuleIDs(*gRefitter->GetGainMap().GetPlaneIDs().begin()).size());
    Int_t x2 = 0, y2 = 0;
    UInt_t w2 = 0, h2 = 0;
    gRefitter->cAll->GetCanvasImp()->GetWindowGeometry(x2, y2, w2, h2);

    gRefitter->cAlone->SetWindowPosition(x2 + w2, y2);
}
