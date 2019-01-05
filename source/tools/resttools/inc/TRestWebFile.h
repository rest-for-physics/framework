#ifndef REST_WEB_Utils
#define REST_WEB_Utils


#include "TRestStringHelper.h"
#include <map>
#include "TUrl.h"


class TRestWebSettings{
protected:
	string fLocalDir;
	TUrl fServer_Download;
	TUrl fServer_Upload;
	int timeout;
	bool certificate;
	bool readOnly;

public:
	TRestWebSettings(string _fLocalDir, string _fServerUrl,
		int _timeout = 10, bool _certificate = true, bool _readOnly = true)
	{
		fLocalDir = _fLocalDir;
		fServer_Download = TUrl(_fServerUrl.c_str());
		fServer_Upload = TUrl(_fServerUrl.c_str());
		timeout = _timeout;
		certificate = _certificate;
		readOnly = _readOnly;
	}
	TRestWebSettings(string _fLocalDir, string _fServer_down, string _fServer_up,
		int _timeout = 10, bool _certificate = true, bool _readOnly = true)
	{
		fLocalDir = _fLocalDir;
		fServer_Download = TUrl(_fServer_down.c_str());
		fServer_Upload = TUrl(_fServer_up.c_str());
		timeout = _timeout;
		certificate = _certificate;
		readOnly = _readOnly;
	}

	string GetLocalDir() { return fLocalDir; }
};

class TRestWebFile : public TRestWebSettings {
protected:
	string fFileName;

public:
	TRestWebFile(): TRestWebSettings("/tmp/", "", "") { fFileName = ""; }
	TRestWebFile(string _fFileName, string _fLocalDir, string _fServerUrl,
		int _timeout=10, bool _certificate = true, bool _readOnly = true) 
		: TRestWebSettings(_fLocalDir, _fServerUrl, _timeout,_certificate, _readOnly)
	{
		SetFileName(_fFileName);
	}

	TRestWebFile(string _fFileName, string _fLocalDir, string _fServer_down, string _fServer_up,
		int _timeout = 10, bool _certificate = true, bool _readOnly = true)
		: TRestWebSettings(_fLocalDir, _fServer_down, _fServer_up, _timeout, _certificate, _readOnly)
	{
		SetFileName(_fFileName);
	}

	string Download();
	string Upload();

	void SetFileName(string filename)//if the file name is combined with directory name, then we set fLocalDir also
	{ 
		auto a = SeparatePathAndName(filename);
		fFileName = a.second; 
		if (isAbsolutePath(a.first))
			fLocalDir = a.first;
	}

	string GetFileName() { return fFileName; }
	static TRestWebFile GetRESTWebFiles(string KeyName);
};



inline TRestWebFile GetRESTWebFiles(string KeyName) { return TRestWebFile::GetRESTWebFiles(KeyName); }

#endif 