#include "PUP.h"
#include "zstr.hpp"

#include <iostream>
#ifndef __APPLE__
#ifdef __unix__
#include <experimental/filesystem>
#else
#include <filesystem>
#endif
namespace fs = std::experimental::filesystem;
#else
#include <sys/syslimits.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif

CPUP::CPUP(std::string inputfile) :
	m_inputfile(inputfile)
	, m_reader(inputfile.c_str())
	, m_header(&m_reader)
{
	PopulateMaps();
	ProcessHeaderEntries();
}

bool CPUP::isValid()
{
	return m_header.magic == 0x1D3D154F;
}

std::map<int, Entry> CPUP::GetEntriesMap()
{
	return m_entries;
}

void CPUP::PopulateMaps()
{
	{
		m_fileNames[3] = "wlan_firmware.bin";
		m_fileNames[6] = "system_fs_image.bin";
		m_fileNames[9] = "recovery_fs_image.bin";
		m_fileNames[11] = "preinst_fs_image.bin";
		m_fileNames[12] = "system_ex_fs_image.bin";
		m_fileNames[257] = "eula.xml";
		m_fileNames[514] = "orbis_swu.self";
	}
	
	{
		m_deviceNames[1] = "/dev/sflash0s0x32b";
		m_deviceNames[13] = "/dev/sflash0s0x32b";
		m_deviceNames[2] = "/dev/sflash0s0x33";
		m_deviceNames[14] = "/dev/sflash0s0x33";
		m_deviceNames[3] = "/dev/sflash0s0x38";
		m_deviceNames[4] = "/dev/sflash0s1.cryptx2b";
		m_deviceNames[5] = "/dev/sflash0s1.cryptx3b";
		m_deviceNames[10] = "/dev/sflash0s1.cryptx40";
		m_deviceNames[9] = "/dev/da0x0.crypt";
		m_deviceNames[11] = "/dev/da0x1.crypt";
		m_deviceNames[7] = "/dev/da0x2";
		m_deviceNames[8] = "/dev/da0x3.crypt";
		m_deviceNames[6] = "/dev/da0x4b.crypt";
		m_deviceNames[12] = "/dev/da0x5b.crypt";
		m_deviceNames[3328] = "/dev/sc_fw_update0";
		m_deviceNames[3336] = "/dev/sc_fw_update0";
		m_deviceNames[3335] = "/dev/sc_fw_update0";
		m_deviceNames[3329] = "/dev/cd0";
		m_deviceNames[3330] = "/dev/da0";
		m_deviceNames[16] = "/dev/sbram0";
		m_deviceNames[17] = "/dev/sbram0";
		m_deviceNames[18] = "/dev/sbram0";
	}
}

void CPUP::ProcessHeaderEntries()
{
	for (int i = 0; i < m_header.entryCount; i++)
	{
		Entry entry;
		entry.Flags = m_reader.Read<uint32_t>();
		m_reader.seekg(4, std::ios_base::cur);
		entry.Offset = m_reader.Read<uint64_t>();
		entry.CompressedSize = m_reader.Read<uint64_t>();
		entry.UncompressedSize = m_reader.Read<uint64_t>();
		m_entries[i] = entry;
	}

	for (int i = 0; i < m_header.entryCount; i++)
	{
		m_tableEntries[i] = -2;
	}

	for (int i = 0; i < m_header.entryCount; i++)
	{
		Entry entry = m_entries[i];
		if (entry.IsBlocked() == false)
		{
			m_tableEntries[i] = -2;
		}
		else
		{
			if (((entry.Id() | 0x100) & 0xF00) == 0xF00)
			{
				throw std::logic_error("InvalidOperationException");
			}

			int tableIndex = -1;
			for (int j = 0; j < m_header.entryCount; j++)
			{
				if ((m_entries[j].Flags & 1) != 0)
				{
					if (m_entries[j].Id() == i)
					{
						tableIndex = j;
						break;
					}
				}
			}

			if (tableIndex < 0)
			{
				throw std::logic_error("InvalidOperationException");
			}

			if (m_tableEntries[tableIndex] != -2)
			{
				throw std::logic_error("InvalidOperationException");
			}

			m_tableEntries[tableIndex] = i;
		}
	}
	
	for (int i = 0; i < m_entries.size(); i++)
	{
		Entry* entry = &m_entries[i];
		
		int special = entry->Flags & 0xF0000000;
		if (special == 0xE0000000 || special == 0xF0000000)
		{
			continue;
		}
		
		std::string name;
		
		if (m_tableEntries[i] < 0)
		{
			if (m_fileNames.find(entry->Id()) != m_fileNames.end())
			{
				name = m_fileNames.at(entry->Id());
			}
			else if(m_deviceNames.find(entry->Id()) != m_deviceNames.end())
			{
				name = m_deviceNames.at(entry->Id());
			}
			else
			{
				name = "/unknown/" +  std::to_string(entry->Id()) + ".img";
			}
		}
		else
		{
			name = "/tables/" + std::to_string(entry->Id())  + "_for_" + std::to_string(m_entries[m_tableEntries[i]].Id()) + ".img";
		}
		
		entry->NamePath = name;
	}
}

void CPUP::CreateDir(std::string outputPath)
{
#ifndef __APPLE__
	auto outputParentPath = (fs::path(outputPath).parent_path().native());
	if (!outputParentPath.empty())
	{
		fs::create_directories(outputParentPath);
	}
#else
	std::string outputParentPath = outputPath.substr(0, outputPath.find_last_of("/\\"));
	if (!outputParentPath.empty())
	{
		mkdir(outputParentPath.c_str(), 0775);
	}
#endif
}

void CPUP::ExtractAllEntries(std::string baseoutputpath)
{
	std::cout << "Processing PUP: " << m_inputfile.c_str() << std::endl;

	for (int i = 0; i < m_entries.size(); i++)
	{
		Entry* entry = &m_entries[i];
		
		int special = entry->Flags & 0xF0000000;
		if (special == 0xE0000000 || special == 0xF0000000)
		{
			continue;
		}
		

		std::cout << "Processing Entry: " << entry->NamePath.c_str()
#ifndef NDEBUG
		<< " -" << entry->ToString()
#endif
		<< std::endl;
		std::string outputPath = baseoutputpath+ "/" + entry->NamePath;
		ExtractSingleEntry(i, entry, outputPath);
	}
}

void CPUP::ExtractSingleEntry(int index, Entry* entry, std::string outputfilepath)
{
	CreateDir(outputfilepath);
	std::ofstream output(outputfilepath.c_str(), std::ios::binary);
	if (entry->IsBlocked() == false)
	{
		//Start reading from
		m_reader.seekg(entry->Offset);
		if (entry->IsCompressed() == false)
		{
			CBinaryReader::CopyStream(&m_reader, &output, entry->CompressedSize);
		}
		else
		{
			zstr::istreambuf zbuf(m_reader.rdbuf(), 1 << 16, false);
			std::istream zlib(&zbuf);
			CBinaryReader::CopyStream(&zlib, &output, entry->UncompressedSize);
			
		}
	}
	else
	{
		if (((entry->Id() | 0x100) & 0xF00) == 0xF00)
		{
			throw std::logic_error("InvalidOperationException");
		}
		
		int tableIndex = -1;
		for (int j = 0; j < m_entries.size(); j++)
		{
			if ((m_entries[j].Flags & 1) != 0)
			{
				if (m_entries[j].Id() == index)
				{
					tableIndex = j;
					break;
				}
			}
		}
		
		if (tableIndex < 0)
		{
			throw std::logic_error("InvalidOperationException");
		}
		
		Entry tableEntry = m_entries[tableIndex];
		
		int blockSize = 1u << (int)(((entry->Flags & 0xF000) >> 12) + 12);
		int blockCount = (blockSize + entry->UncompressedSize - 1) / blockSize;
		int tailSize = entry->UncompressedSize % blockSize;
		if (tailSize == 0)
		{
			tailSize = blockSize;
		}
		
		std::map<int, CPUP::BlockInfo> blockInfos;
		if (entry->IsCompressed())
		{
			std::stringstream tableData;
			m_reader.seekg(tableEntry.Offset);
			if (tableEntry.IsCompressed() == false)
			{
				CBinaryReader::CopyStream(&m_reader, &tableData, tableEntry.CompressedSize);
			}
			else
			{
				zstr::istreambuf zbuf(m_reader.rdbuf(), 1 << 16, false);
				std::istream zlib(&zbuf);
				CBinaryReader::CopyStream(&zlib, &tableData, tableEntry.UncompressedSize);
			}
			
			tableData.seekg(32 * blockCount);
			for (int j = 0; j < blockCount; j++)
			{
				CPUP::BlockInfo blockInfo;
				blockInfo.Offset = CBinaryReader::Read<uint32_t, std::stringstream>(&tableData);
				blockInfo.Size = CBinaryReader::Read<uint32_t, std::stringstream>(&tableData);
				blockInfos[j] = blockInfo;
			}
		}
		
		m_reader.seekg(entry->Offset);
		
		int compressedRemainingSize = entry->CompressedSize;
		int uncompressedRemainingSize = entry->UncompressedSize;
		int lastIndex = blockCount - 1;
		
		for (int i = 0; i < blockCount; i++)
		{
			long compressedReadSize, uncompressedReadSize;
			bool blockIsCompressed = false;
			
			if (entry->IsCompressed() == true)
			{
				CPUP::BlockInfo blockInfo = blockInfos[i];
				int unpaddedSize = (blockInfo.Size & ~0xFu) - (blockInfo.Size & 0xFu);
				
				compressedReadSize = blockSize;
				if (unpaddedSize != blockSize)
				{
					compressedReadSize = blockInfo.Size;
					if (i != lastIndex || tailSize != blockInfo.Size)
					{
						compressedReadSize &= ~0xFu;
						blockIsCompressed = true;
					}
				}
				
				if (blockInfo.Offset != 0)
				{
					int blockOffset = entry->Offset + blockInfo.Offset;
					m_reader.seekg(blockOffset);
					output.seekp(i * blockSize);
				}
			}
			else
			{
				compressedReadSize = compressedRemainingSize;
				if (blockSize < compressedReadSize)
				{
					compressedReadSize = blockSize;
				}
			}
			
			if (blockIsCompressed)
			{
				{
					std::stringstream temp;
					CBinaryReader::CopyStream<std::ifstream, std::stringstream>(&m_reader, &temp, compressedReadSize - (blockInfos[i].Size & 0xF));
					temp.seekg(0);
					
					uncompressedReadSize = uncompressedRemainingSize;
					if (blockSize < uncompressedReadSize)
					{
						uncompressedReadSize = blockSize;
					}
					
					zstr::istreambuf zbuf(temp.rdbuf(), 1 << 16, false);
					std::istream zlib(&zbuf);
					CBinaryReader::CopyStream(&zlib, &output, uncompressedReadSize);
				}
			}
			else
			{
				CBinaryReader::CopyStream(&m_reader, &output, compressedReadSize);
			}
			
			compressedRemainingSize -= compressedReadSize;
			uncompressedRemainingSize -= uncompressedReadSize;
		}
	}
}
