#include "curl/curl.h"
#include <format>
#include <fstream>
#include <TlHelp32.h>

#define RELEASE

#ifdef RELEASE
#define PlayersService "https://blandosvn.uk/"
#define PlacesService "https://blandosvn.uk/"
#endif
#ifdef DEBUG
#define PlayersService "http://10.0.1.6/"
#define PlacesService "http://10.0.1.6/"
#endif

std::string tk = "";

std::string GetExeFileName()
{
	char buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
	return std::string(buffer);
}

std::string GetExePath()
{
	std::string f = GetExeFileName();
	return f.substr(0, f.find_last_of("\\/"));
}

namespace TlHelp {
	inline int getProcID(const std::string& p_name)
	{
		HANDLE snapshot = CreateToolhelp32Snapshot(0x00000002, 0);
		PROCESSENTRY32 structprocsnapshot = { 0 };

		structprocsnapshot.dwSize = sizeof(PROCESSENTRY32);

		if (snapshot == INVALID_HANDLE_VALUE)return 0;
		if (Process32First(snapshot, &structprocsnapshot) == FALSE)return 0;

		while (Process32Next(snapshot, &structprocsnapshot))
		{
			if (!strcmp(structprocsnapshot.szExeFile, p_name.c_str()))
			{
				CloseHandle(snapshot);
				return structprocsnapshot.th32ProcessID;
			}
		}
		CloseHandle(snapshot);
		return 0;

	}

	void FindFirstWindowByPid(DWORD dwProcessID, HWND h)
	{
		HWND hCurWnd = nullptr;
		do
		{
			hCurWnd = FindWindowEx(nullptr, hCurWnd, nullptr, nullptr);
			DWORD checkProcessID = 0;
			GetWindowThreadProcessId(hCurWnd, &checkProcessID);
			if (checkProcessID == dwProcessID)
			{
				h = hCurWnd;
			}
		} while (hCurWnd != nullptr);
	}

}

namespace Revival {

	static size_t Handle(void* contents, size_t size, size_t nmemb, void* userp) {
		((std::string*)userp)->append((char*)contents, size * nmemb);
		return size * nmemb;
	}

	void WritePlayerToken(std::string response) {
		std::ofstream tk;
		tk.open(std::format("{}\\player.token", GetExePath()));
		tk << response;
		tk.close();
	}

	std::string ReadPlayerToken() {
		std::ifstream tk;
		std::string response;
		tk.open(std::format("{}\\player.token", GetExePath()));
		tk >> response;
		tk.close();
		remove(std::format("{}\\player.token", GetExePath()).c_str());
		return response;
	}

	bool DoLogin(CURL* curler, std::string username, std::string password) {
		bool auth = false;

		std::string login = std::format("{}Launcher.aspx?type=0&charactername={}&password={}", PlayersService, username, password);
		std::string response;

		std::string header_string;

		curl_easy_setopt(curler, CURLOPT_URL, login);
		curl_easy_setopt(curler, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curler, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_0);
		curl_easy_setopt(curler, CURLOPT_WRITEFUNCTION, Revival::Handle);
		curl_easy_setopt(curler, CURLOPT_WRITEDATA, &response);
		curl_easy_setopt(curler, CURLOPT_WRITEHEADER, &header_string);

		CURLcode result;
		result = curl_easy_perform(curler);

		if (result != CURLE_OK)
			printf("Failed to post login information result: [%i]\n", result);
		else
			printf("Login returned with code: %i\n", result);

		auth = response != "failure" || "user doesnt exist";
		if (auth)
			tk = response;

		curl_easy_cleanup(curler);
		return auth;
	}
}