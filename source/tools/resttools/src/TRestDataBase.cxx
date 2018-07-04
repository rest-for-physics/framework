#include "TRestDataBase.h"
#include "TRestTools.h"
#include "TSystem.h"
#include <sys/types.h>  
#include <sys/stat.h>  
#include <stdio.h>  
#include <errno.h>  
#include "TRestStringHelper.h"


TRestDataBase* TRestDataBase::instantiate() {

	vector <TString> list = TRestTools::GetListOfRESTLibraries();
	for (unsigned int n = 0; n < list.size(); n++)
	{
		gSystem->Load(list[n]);
	}

	TClass*c = TClass::GetClass("TRestDataBaseImpl");
	if (c != NULL)//this means we have the package installed
	{
		return (TRestDataBase*)c->New();
	}
	return NULL;
}


DataBaseFileInfo::DataBaseFileInfo(string filename) {

	auto _fullname = ToAbsoluteName(filename);

	struct stat buf;
	int result = stat(_fullname.c_str(), &buf);

	if (result != 0)
	{
		cout << "Failed to load file \"" << _fullname << "\"!" << endl;
		fileSize = 0;
		evtRate = 0;
		quality = true;
		start = 0;
		stop = 0;
		for (int i = 0; i < 41; i++) {
			sha1sum[i] = 0;
		}
	}
	else
	{
		fileSize = buf.st_size;
		evtRate = 0;
		quality = true;
		start = buf.st_ctime;
		stop = buf.st_mtime;
		string sha1result = ExecuteShellCommand("sha1sum " + _fullname);
		string sha1 = Spilt(sha1result, " ")[0];
		if (sha1.size() == 40) {
			for (int i = 0; i < 40; i++) {
				sha1sum[i] = sha1[i];
			}
			sha1sum[40] = 0;
		}
	}
}

void DataBaseFileInfo::Print() {
	cout << "----DataBaseFileInfo struct----" << endl;
	cout << "size: " << fileSize << endl;
	cout << "event rate: " << evtRate << endl;
	cout << "sha1sum: " << sha1sum << endl;
	cout << "quality: " << quality << endl;
	cout << "start time: " << ToDateTimeString(start) << endl;
	cout << "stop time: " << ToDateTimeString(stop) << endl;
}