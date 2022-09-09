#include "Launcher.h"
#include "RevivalApi.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "Out.h"
#include <iostream>
#include <TlHelp32.h>

using namespace rapidjson;

void Launch(std::string username, std::string password, std::string id) {
	printf("Requesting server..\n");

	std::string resp = DownloadString(std::format("https://blandosvn.uk/Launcher.aspx?type=3&place={}", id));
	if (resp != "") {
		std::string str = "Failed to launch OSVN! [{}]\nPlease try again, check the version you're trying to join.\nIf this keeps occuring AND you have done these steps.\nPlease contact staff!\n";
		MessageBox(NULL, std::format(str, resp).c_str(), "OSVN Launcher Crash", MB_ICONERROR);
		exit(1);
	}

	printf("Launching roblox...\n");

	printf("Got token: %s\n", tk.c_str());

	std::string port = DownloadString(std::format("https://blandosvn.uk/Launcher.aspx?type=24&place={}", id));
	std::string joinScript = DownloadString("https://blandosvn.uk/Game/join.txt");

	std::string PlayTestScript = std::format(joinScript, port, tk);

	std::ofstream tmp_script(std::format("{}\\content\\fonts\\join.txt", GetExePath()));
	tmp_script << PlayTestScript;
	tmp_script.close();

	ShellExecuteA(NULL, "open", std::format("{}\\Roblox.exe", GetExePath()).c_str(), std::format("-script {}\\content\\fonts\\join.txt", GetExePath()).c_str(), NULL, SW_SHOWDEFAULT);
	printf("Opened roblox\n");

	Sleep(1500);
	HWND rbx = FindWindowA(NULL, "Roblox - [Place1]"); 
	SetWindowTextA(rbx, "Osvn - [Place1]");

	remove(std::format("{}\\joiner.rbxl", GetExePath()).c_str());
}


Launcher::Launcher(std::string username, std::string password, std::string game)
{
	curl_global_init(CURL_GLOBAL_ALL);
	curler = curl_easy_init();

	id = game;

	if (curler)
		printf("Ready!\n");

	bool authorized = Revival::DoLogin(curler, username, password);
	printf("Authorized: %s\n", authorized ? "true" : "false");

	if (authorized)
		Launch(username, password, id);
	else
		return;

	curl_global_cleanup();
}