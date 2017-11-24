#pragma once
#include <fstream>

class CBinaryReader : public std::ifstream
{
public:
	CBinaryReader(const char* file) : std::ifstream(file, std::ios::binary){};
	~CBinaryReader() = default;

	template <class T>
	T Read()
	{
		T res;
		read(reinterpret_cast<char *>(&res), sizeof(T));
		return res;
	}

	template <class T, class TS>
	static T Read(TS* stream)
	{
		T res;
		stream->read(reinterpret_cast<char *>(&res), sizeof(T));
		return res;
	}
	
	template <typename TS, typename T>
	static void CopyStream(TS* input, T* output, long size)
	{
		int left = size;
		char data[1 << 16];
		while (left > 0)
		{
			int block = (int)(std::min(left, 1 << 16));
			input->read(data, block);
			if(!input)
			{
				throw std::logic_error("EndOfStreamException");
			}
			std::streamsize cnt = input->gcount();
			output->write(data, cnt);
			left -= block;
		}
	}
};
