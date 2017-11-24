#pragma once
#include "BinaryReader.h"
#include "Entry.h"
#include <map>

class CPUP
{
public:
	CPUP(std::string inputfile);
	~CPUP() = default;
	
	bool isValid();
	void ExtractSingleEntry(int index, Entry* entry, std::string outputfilepath);
	void ExtractAllEntries(std::string outputpath);
	std::map<int, Entry> GetEntriesMap();

private:
	void ProcessHeaderEntries();
	void PopulateMaps();
	void CreateDir(std::string outputPath);
	
	std::string m_inputfile;
	
	CBinaryReader m_reader;
	std::map<int, Entry> m_entries;
	std::map<int, int> m_tableEntries;
	
	std::map<int, const char *> m_fileNames;
	std::map<int, const char *> m_deviceNames;
	

public:
	struct Header
	{
		Header(CBinaryReader* reader) :
			magic(reader->Read<uint32_t>()),
			unknown04(reader->Read<uint32_t>()),
			unknown08(reader->Read<uint16_t>()),
			flags(reader->Read<char>()),
			unknown0B(reader->Read<char>()),
			headerSize(reader->Read<uint16_t>()),
			hashSize(reader->Read<uint16_t>()),
			fileSize(reader->Read<uint64_t>()),
			entryCount(reader->Read<uint16_t>()),
			hashCount(reader->Read<uint16_t>()),
			unknown1C(reader->Read<uint32_t>())
	 		{}
		const uint32_t magic;
		const uint32_t unknown04;
		const uint16_t unknown08;
		const char flags;
		const char unknown0B;
		const uint16_t headerSize;
		const uint16_t hashSize;
		const uint64_t fileSize;
		const uint16_t entryCount;
		const uint16_t hashCount;
		const uint32_t unknown1C;
	};
	struct BlockInfo
	{
		uint32_t Offset;
		uint32_t Size;
	};
	Header m_header;
};
