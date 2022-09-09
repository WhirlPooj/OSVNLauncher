#pragma once

#include <string>
#include <curl.h>

class Launcher {
public:
	CURL* curler;
	std::string id;
	Launcher(std::string username, std::string password, std::string game);
};

class SinglePlayer {
private:
	CURL* curler;
public:
	std::string id;
	SinglePlayer(std::string username, std::string password, std::string __id);
};