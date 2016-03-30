#ifndef _WAVMOD_H
#define _WAVMOD_H
#include <cstring>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <endian.h>
#define IDSIZE 5
#define BR "\n===============================================================================\n"
#define UPDATEBAR ">>>> "
#define ERROR "<ERROR>: "
using namespace std;

struct __attribute((packed)) riff_chunk
{
	char id[4];
	unsigned int size;
	char format[4];
};

struct __attribute((packed)) fmt_chunk
{
	char id[4];
	unsigned int size;
	unsigned short format;
	unsigned short numChannels;
	unsigned int sampleRate;
	unsigned int byteRate;
	unsigned short blockAlign;
	unsigned short bitsPerSample;
};

class FileMod{
	private:
	public:
	protected:
};
class WAV : public FileMod
{
	public:
		WAV(){}
		WAV(string filename);

		void PrintMetaData() const;		
		bool HideFile(string wav_file, string img_name);
		bool ExtractFile(string wav_file, string new_img, unsigned long bytes);
	private:
		void PrintRiff() const;
		void PrintFmt() const;
		void PrintData() const;
		bool SetHiddenFileMetaData(string img);
		string GetCompressionMessage() const;
		string GetChannelMessage() const;
		void SetChunkStrings();
		string file_name;
		string img_name;
		riff_chunk rchunk;
		fmt_chunk fchunk;
		char riff_id[IDSIZE];
		char riff_format[IDSIZE];
		char fmt_id[IDSIZE];
		char data_id[IDSIZE];
		unsigned int data_size;
		unsigned int num_samples;
		size_t wav_file_size;
		size_t img_file_size;
};

#endif