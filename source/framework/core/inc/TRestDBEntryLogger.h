/*************************************************************************
 * This file is part of the REST software framework.                     *
 *                                                                       *
 * Copyright (C) 2016 GIFNA/TREX (University of Zaragoza)                *
 * For more information see http://gifna.unizar.es/trex                  *
 *                                                                       *
 * REST is free software: you can redistribute it and/or modify          *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation, either version 3 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 * REST is distributed in the hope that it will be useful,               *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have a copy of the GNU General Public License along with   *
 * REST in $REST_PATH/LICENSE.                                           *
 * If not, see http://www.gnu.org/licenses/.                             *
 * For the list of contributors see $REST_PATH/CREDITS.                  *
 *************************************************************************/

#ifndef RestCore_TRestDBEntryLogger
#define RestCore_TRestDBEntryLogger

#include "TRestMetadata.h"
#include "TRestRun.h"

// Put this metadata ahead of all others metadata sections in TRestRun section
// to enable psql database filling. TRestRun can therefore initailize other metadata
// objects according to the filled information (gas component, pressure, drift, etc.)
//
// It will ask the user to fill a run information text file.
class TRestDBEntryLogger : public TRestMetadata {
   private:
    TRestRun* fRun; //!

    bool fSkipIfNotEmpty;
    string fTextOpenCommand = "vim";
    map<string, string> fMetainfo;

    void InitFromConfigFile();

    void Initialize();

    void AskForFilling(int run_id);

   protected:
   public:
    // getters
    string operator[](string itemname) { return Get(itemname); }
    string Get(string itemname) { return fMetainfo.count(itemname)==0? "": fMetainfo[itemname]; }
    int GetEntries() { return fMetainfo.size(); }

    void PrintMetadata();
    // Constructor
    TRestDBEntryLogger();
    // Destructor
    ~TRestDBEntryLogger() {}

    ClassDef(TRestDBEntryLogger, 1);  // Template for a REST "event process" class inherited from
                                         // TRestEventProcess
};
#endif
