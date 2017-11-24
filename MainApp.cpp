#include <algorithm>
#include <iostream>

#include "MainApp.h"
#include "PUP.h"

bool iendsWith(std::string string, std::string const &ending)
{
	std::transform(string.begin(), string.end(), string.begin(), ::tolower);
	if (string.length() >= ending.length())
	{
		return string.compare(string.length() - ending.length(), ending.length(), ending) == 0;
	}
	else
	{
		return false;
	}
}

void MainApp::loop(int argc, const char **argv)
{
	ProcessArgs(argc, argv);
	CPUP pup(m_inputfile);
	if(pup.isValid())
	{
		pup.ExtractAllEntries(m_baseoutputpath);
	}
	else
	{
		std::cout << "Invalid PUP image provideded" << std::endl
		<< "Please ensure the image was decypted first." <<std::endl;
		PrintUsage();
	}
}

void MainApp::ProcessArgs(int argc, const char **argv)
{
	bool showHelp = false;

	if (argc == 2)
	{
		m_binfile = argv[0];
		m_inputfile = argv[1];
		if(!iendsWith(m_inputfile, ".pup.dec"))
		{
			showHelp = true;
		}
		m_baseoutputpath = m_inputfile.substr(0, m_inputfile.length() - 8) + "/";
	}
	else if (argc == 3)
	{
		m_binfile = argv[0];
		m_inputfile = argv[1];
		m_baseoutputpath = argv[2];
		if(!iendsWith(m_inputfile, ".pup.dec"))
		{
			showHelp = true;
		}
	}
	else
	{
		showHelp = true;
	}
	
	if(showHelp)
	{
		PrintUsage();
	}
}

void MainApp::PrintUsage()
{
	std::cerr << "Usage: pup_unpacker 405.pup.dec [outputpath]" << std::endl;
	exit(404);

}
