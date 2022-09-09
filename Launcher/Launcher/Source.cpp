
/* Wrote by Whirlpooj / Whirlpool, (July 13 - Now) */
/* The full, untampered OSVN Launcher */
/* Customize it and mask something cool I guess */

#include "Launcher.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <TlHelp32.h>
#include <tchar.h>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "wldap32.lib")
#pragma comment(lib, "normaliz.lib")
#pragma comment (lib, "crypt32.lib")

#define RELEASE

__forceinline bool running(const TCHAR* const executableName) {
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	const auto snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (!Process32First(snapshot, &entry)) {
		CloseHandle(snapshot);
		return false;
	}

	do {
		if (!_tcsicmp(entry.szExeFile, executableName)) {
			CloseHandle(snapshot);
			return true;
		}
	} while (Process32Next(snapshot, &entry));

	CloseHandle(snapshot);
	return false;
}

int main(int argc, char** argv) {

	std::string s(argv[1]);
	std::string tmp;

	s.erase(0, 5);

	std::stringstream stream(s);
	std::vector<std::string> args;

	while (std::getline(stream, tmp, '&'))
		args.push_back(tmp);

	std::string game = args[0];
	auto username = args[1];
	auto password = args[2];
	auto singleplayer = args[3];

	printf("Starting...\n");

	printf("Checking..\n");

#ifdef RELEASE
	HKEY Key;
	if (!RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\ROBLOX Corporation\\Roblox", 0, KEY_READ, &Key)) {
		DWORD dwt;
		if (!RegQueryValueExA(Key, "OverrideLockViewGamelayout", 0, &dwt, NULL, NULL)) {
			printf("Failed!\n");
			RegCloseKey(Key);
			Sleep(200);
			exit(1);
		}
	}
#endif

	printf("Checking for RADMIN..\n");
	if (!running("RvRvpnGui.exe")) {
		printf("RAdmin not running, please open it and retry\n");
		Sleep(2000);
		exit(0);
	}

	/* Ping the gameserver */

	if (system("ping 26.201.200.179 -n 1 > nul") != 0) {
		printf("Failed to ping gameserver, are you in the radmin?\n");
		printf("If you aren't please join with the credentials: \n     name: Osvn2\n     password: PlayOsvn\n");
		printf("If you don't have RADMIN installed get it here: https://www.radmin-vpn.com/\n");
		printf("If this keeps happening EVEN when you have RADMIN installed and running (joined the server too), please ask for support in the discord\nPress enter to close this window...");
		std::cin.get();
		exit(0);
	}

	if (singleplayer != "true")
		Launcher l = Launcher(username, password, game);
}