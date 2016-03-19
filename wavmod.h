#ifndef _WAVMOD_H
#define _WAVMOD_H
#include <cstring>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <endian.h>
#define IDSIZE 5

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
		WAV(string filename, string img);
		void PrintRiff() const;
		void PrintFmt() const;
		void PrintData() const;
		void PrintAll() const;
		void BruteForceEditGen(size_t start = 0, size_t len = 1); //creates n files by modifying the first n bytes
		bool HideFile();
		bool ExtractFile(string new_img, string wav_file);
	private:
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