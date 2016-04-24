#include "wavmod.h"
#define WAVDATASTART 43
#define STEGMODIFIER 4 //only 1 byte of 2 can be modified, only 1/2 the bits can be taken = 2 * 2 = 4

WAV::WAV(string filename)
{	
	ifstream wavstream(filename, ios::ate | ios::binary);
	wav_file_size = wavstream.tellg();
	wavstream.close();
	ifstream wav_file(filename, ios::binary);
	if (wav_file.is_open())
	{
		try
		{
			wav_file.read((char*)&rchunk, sizeof(riff_chunk));
			wav_file.read((char*)&fchunk, sizeof(fchunk));
			wav_file.read(data_id, sizeof(IDSIZE - 1));
			wav_file.read((char*)&data_size, sizeof(unsigned int));
			SetChunkStrings();
			num_samples = data_size / ((fchunk.bitsPerSample / 8) * fchunk.numChannels);
		}
		catch (const exception& e)
		{
			cerr << ERROR << filename << ": not a valid wav." << endl;
			return;
		}
		wav_file.close();
	}
	else
	{
		cerr << ERROR << filename << ": failed to open." << endl;
		return;
	}
	file_name = filename;
	cout << UPDATEBAR << filename << ": metadata verified." << endl;
}

bool WAV::SetHiddenFileMetaData(string img)
{
	ifstream imgstream(img, ios::ate | ios::binary);
	img_file_size = imgstream.tellg();
	imgstream.close();
	img_name = img;
	if (img_file_size * STEGMODIFIER > data_size) 
	{
		cerr << ERROR << file_name << ": too small to hide image: " << img << endl;
		return false;
	}
	return true;
}


bool WAV::HideFile(string wav_file, string img_name)
{
	//set metadata
	*this = WAV(wav_file);
	if (!SetHiddenFileMetaData(img_name)) return false;

	//copy wav file
	string file_copy = file_name + ".wav";
	ifstream wavfilestream(file_name, ios::binary);
	ofstream copy(file_copy, ios::binary|ios::out);
	copy << wavfilestream.rdbuf();
	wavfilestream.close();
	copy.close();

	//begin injection process
	fstream output(file_copy, ios::in | ios::out | ios::binary);
	fstream imgstream(img_name, ios::in | ios::out | ios::binary);
	if (output.is_open() && imgstream.is_open())
	{ 	
		cout << UPDATEBAR << file_copy << ": creating file copy." << endl;
		output.seekp(WAVDATASTART, ios::beg); //where data section begins
		unsigned char readCH, writeCH, imgreadCH, temp;
		size_t write = 0, pos = output.tellp();
		while (imgstream.good())
		{
			try 
			{
				//inject 4 most significant bits
				output.read((char*)&readCH, sizeof(char));
				output.read((char*)&readCH, sizeof(char));
				writeCH = ((readCH >> 4) << 4);
				imgstream.read((char*)&imgreadCH, sizeof(char));
				temp = (imgreadCH << 4);
				writeCH |= (temp >> 4);
				pos = output.tellp();
				output.seekp(pos - 1);
				output.write((char*)&writeCH, sizeof(char));
				
				//inject 4 least significant bits
				output.read((char*)&readCH, sizeof(char));
				output.read((char*)&readCH, sizeof(char));

				temp = (readCH >> 4);
				writeCH = (temp << 4);
				writeCH |= (imgreadCH >> 4);
				
				pos = output.tellp();
				output.seekp(pos - 1);
				output.write((char*)&writeCH, sizeof(char));
			
				write++;
			}
			catch (const exception& e)
			{
				cerr << ERROR << wav_file << ": file corrupt." << endl;
				output.close();
				imgstream.close();
				return false;	
			}
		}
		cout << UPDATEBAR << file_copy << ": injection completed - ";
		cout << write - 1 << " of " << img_file_size << " bytes." << endl;
		
		output.close();
		imgstream.close();
		return true;
	}
	return false;
}

bool WAV::ExtractFile(string wav_file, string new_img, unsigned long bytes)
{
	*this = WAV(wav_file);
	if (bytes * STEGMODIFIER > data_size) 
	{
		cerr << ERROR << wav_file << ": extraction of " << bytes << " bytes is not possible." << endl;
		return false;
	}
	
	fstream output(new_img, ios::out | ios::binary);
	fstream extract_stream(wav_file, ios::in | ios::out | ios::binary);
	if (output.is_open() && extract_stream.is_open())
	{ 	
		extract_stream.seekp(WAVDATASTART, ios::beg); //where data begins
		unsigned char readCH, writeCH, temp;
		size_t write = 0;
		while (write < bytes)
		{
			try
			{
				//extract 4 most significant bits
				extract_stream.read((char*)&readCH, sizeof(char));
				extract_stream.read((char*)&readCH, sizeof(char));
				temp = (readCH << 4);
				writeCH = (temp >> 4);
				
				//extract 4 least significant bits
				extract_stream.read((char*)&readCH, sizeof(char));
				extract_stream.read((char*)&readCH, sizeof(char));
				writeCH |= (readCH << 4);
				output.write((char*)&writeCH, sizeof(char));
				write++;
				if (!extract_stream.good()) break;
			}
			catch (const exception& e)
			{
				cerr << ERROR << wav_file << ": file corrupt." << endl;
				output.close();
				extract_stream.close();
				return false;	
			}
		}
		cout << UPDATEBAR << new_img << ": extraction completed - ";
		cout << write << " of " << bytes << " bytes." << endl;		
		output.close();
		extract_stream.close();
		return true;
	}	
	return false;
}

void WAV::SetChunkStrings()
{
	data_id[IDSIZE - 1] = 0; 
	memcpy(riff_id, rchunk.id, IDSIZE - 1);
	riff_id[IDSIZE - 1] = 0;
	memcpy(riff_format, rchunk.format,  IDSIZE - 1);
	riff_format[IDSIZE - 1] = 0;
	memcpy(fmt_id, fchunk.id, IDSIZE - 1);
	fmt_id[IDSIZE - 1] = 0;
}
void WAV::PrintMetaData() const
{
	cout << BR;
	cout << "{" << file_name << "} METADATA" << BR; 
	PrintRiff();
	cout << BR;
	PrintFmt();
	cout << BR;
	PrintData();
	cout << BR;
}

void WAV::PrintRiff() const
{
	//cout << "RChunk SIZE  : " << sizeof(rchunk) << endl;	
	cout << "RIFF ID      : " << riff_id << endl;
	cout << "size         : " << rchunk.size << endl;
	cout << "format       : " << riff_format << endl;
}
void WAV::PrintFmt() const
{
	//cout << "FChunk SIZE  : " << sizeof(fchunk) << endl;
	cout << "FMT ID       : " << fmt_id << endl;
	cout << "size         : " << fchunk.size << endl;
	cout << "format       : " << fchunk.format << GetCompressionMessage() << endl;
	cout << "numChannels  : " << fchunk.numChannels << GetChannelMessage() << endl;
	cout << "sampleRate   : " << fchunk.sampleRate << endl;
	cout << "byteRate     : " << fchunk.byteRate << endl;
	cout << "blockAlign   : " << fchunk.blockAlign << endl;
	cout << "bitsPerSample: " << fchunk.bitsPerSample << endl;
}

void WAV::PrintData() const
{
	cout << "DATA ID      : " << data_id << endl;
	cout << "size         : " << data_size << endl;
	cout << ">num_samples : " << num_samples << endl;
	
}
string WAV::GetCompressionMessage() const
{
	if (fchunk.format == 1) return " (No Compression - Linear Quantization)";
	return " (Compressed)";
}
string WAV::GetChannelMessage() const
{
	if (fchunk.numChannels == 1) return " (Mono)";
	if (fchunk.numChannels == 2) return " (Stereo)";
	return " (Surround)";
}