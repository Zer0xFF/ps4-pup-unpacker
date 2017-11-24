#pragma once
#include <cstdint>
#include <string>
#include <sstream>

struct Entry
{
	uint32_t Flags;
	uint32_t Offset;
	uint32_t CompressedSize;
	uint32_t UncompressedSize;
	std::string NamePath;

	uint32_t Id()
	{
		return Flags >> 20;
	}

	bool IsCompressed()
	{
		return (Flags & 8) != 0;
	}

	bool IsBlocked()
	{
		return (Flags & 0x800) != 0;
	}

	std::string ToString()
	{
		std::ostringstream stringStream;
		stringStream << " ID: " << Id() << " - IsBlocked: " << ((IsBlocked()) ? "Yes" : "No") << " - IsCompressed: " << ((IsCompressed()) ? "Yes" : "No")  <<  " - Offset: " << std::hex << Offset
		<< " Flags: " << std::hex << Flags;
		std::string copyOfStr = stringStream.str();
		return copyOfStr;
	}
};
