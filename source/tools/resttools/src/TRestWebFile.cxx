#include "TRestWebFile.h"

map<string, TRestWebFile> webFilesList = {
	{ "TRestGas", TRestWebFile(
		"gases.rml",
		(string)getenv("REST_PATH") + "/inputData/gasFiles/",
		"https://gasUser@sultan.unizar.es/gasFiles/",
		3,
		false,
		false

	)},
	{ "TRestReadout",TRestWebFile(
		"readouts.rml",
		(string)getenv("REST_PATH") + "/inputData/readouts/",
		"https://sultan.unizar.es/readouts/"
	)}
};

TRestWebFile TRestWebFile::GetRESTWebFiles(string keyName)
{
	return webFilesList[keyName];
}

string TRestWebFile::Download() {

	if (fFileName == "" || string(fServer_Download.GetUrl()) == "")
	{
		return "";
	}
	string fullFileName = fLocalDir + "/" + fFileName;

	stringstream cmd;

	//we first download to temporary file
	string DownLoadFile = "/tmp/" + fFileName;
	if ((string)fServer_Download.GetProtocol() == "https") {
		cmd << "wget ";
		if (!certificate)cmd << "--no-check-certificate ";
		cmd << string(fServer_Download.GetUrl()) << "/" << fFileName << " ";

		cmd << " -O " << DownLoadFile << " ";
		cmd << "-T " << timeout << " -t 1 ";
		cmd << "-q";
	}
	else if ((string)fServer_Download.GetProtocol() == "ssh")
	{
		cmd << "sshpass -p " << fServer_Download.GetPasswd() << " ";
		cmd << "scp ";
		if (fServer_Download.GetPort() != 0) cmd << "-P " << fServer_Download.GetPort() << " ";
		cmd << fServer_Download.GetUser() << "@" << fServer_Download.GetHost() << ":";
		cmd << fServer_Download.GetFile() << "/" << fFileName << " ";
		cmd << DownLoadFile;
	}

	cout << "downloading file, command: " << cmd.str() << endl;
	int a = system(cmd.str().c_str());


	if (a == 0)
	{
		cout << "-- Success : download OK!" << endl;
		//we move the file to local database if it is writable
		if (isPathWritable(fLocalDir))
		{
			system(("mv " + DownLoadFile + " " + fullFileName).c_str());
			return fullFileName;
		}
		//otherwise we return the temporary file
		else {
			return DownLoadFile;
		}
	}
	else
	{
		cout << "REST Error : TRestWebFile::Download() : download failed!" << endl;
		if (a == 1024) cout << "Network connection problem?" << endl;
		if (a == 2048) cout << "Gas definition does NOT exist in database?" << endl;

		//we return the file from last successfull download, if it exists
		if (fileExists(fullFileName)) {
			cout << "-- Info : Using local file:" << fullFileName << endl;
			return fullFileName;
		}
		return "";
	}

}

string TRestWebFile::Upload() {
	if (fFileName == "" || string(fServer_Upload.GetUrl()) == "")
	{
		return "";
	}
	if (readOnly) {
		cout << "REST Warning : TRestWebFile::Upload() : The server is read only! cannot upload" << endl;
		return "";
	}
	string fullFileName = fLocalDir + "/" + fFileName;
	if (!fileExists(fullFileName)) {
		cout << "REST Warning : TRestWebFile::Upload() : local file does not exist! File name: " << fullFileName << endl;
		return "";
	}

	stringstream cmd;
	//cmd << "scp " << fullFileName << " " << fServer_Upload.GetUser() << "@" << fServer_Upload.GetHost() << ":./" << fServer_Upload.GetFile();
	
	if ((string)fServer_Upload.GetProtocol() == "ssh") {
		cmd << "sshpass -p " << fServer_Upload.GetPasswd() << " ";
		cmd << "scp ";
		if (fServer_Upload.GetPort() != 0) cmd << "-P " << fServer_Upload.GetPort() << " ";
		cmd << fullFileName << " ";
		cmd << fServer_Upload.GetUser() << "@" << fServer_Upload.GetHost() << ":./";
		cmd << fServer_Upload.GetFile() << "/" << fFileName;
	}
	
	//GetFile --> "gasFiles/"
	//GetFileName --> "gases.rml"

	cout << "uploading file, command: " << cmd.str() << endl;

	int a = system(cmd.str().c_str());

	if (a != 0)
	{
		cout << "-- Error : problem uploading file to remote server" << endl;
		cout << "Please report this problem at http://gifna.unizar.es/rest-forum/" << endl;
		return "";
	}
	return fullFileName;
}