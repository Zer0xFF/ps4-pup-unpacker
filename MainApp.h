#pragma once
#include <string>
class MainApp
{
public:
	MainApp() = default;
	~MainApp() = default;

	void loop(int argc, const char **argv);

private:
	void ProcessArgs(int argc, const char **argv);
	void PrintUsage();

	std::string m_binfile;
	std::string m_inputfile;
	std::string m_baseoutputpath;

};
