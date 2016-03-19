#include "wavmod.h"
#define WAVDATASTART 43
#define MAGICNUMBER 299009
WAV::WAV(string filename, string img)
{	
	ifstream imgstream(img, ios::ate | ios::binary);
	ifstream wavstream(filename, ios::ate | ios::binary);
	img_file_size = imgstream.tellg();
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
			cerr << filename << ": not a valid wav" << endl;
		}
		wav_file.close();
	}
	else
	{
		cerr << filename << " failed to open" << endl;
	}
	file_name = filename;
	img_name = img;
	cout << "SIZE NEEDED: " << img_file_size * 8 * 2 * 2 << " of " << data_size << endl;
	if (img_file_size * 8 * 2 * 2 < data_size) //change this data_size to the amount hideable
	{
		HideFile();
		cout << "EXTRACTING" << endl;
		ExtractFile("myimg.img",string(file_name + ".wav"));
	}
	else
	{
		cerr << filename << ": too small to hide image: " << img << endl;
	}
}
bool WAV::HideFile()
{
	string file_copy = file_name + ".wav";
	cout << "COPYING FILE " << file_copy << endl;
	ifstream wav_file(file_name, ios::binary);
	ofstream copy(file_copy, ios::binary|ios::out);
	copy << wav_file.rdbuf();
	wav_file.close();
	copy.close();
	fstream output(file_copy, ios::in | ios::out | ios::binary);
	fstream imgstream(img_name, ios::in | ios::out | ios::binary);
	if (output.is_open() && imgstream.is_open())
	{ 	
		output.seekp(WAVDATASTART, ios::beg); //where data begins
		unsigned char readCH, writeCH = 0, imgreadCH, temp;
		size_t pos = output.tellp();
		size_t write = 0;
		//size_t checkValue = 132093;
		while (imgstream.good())
		{
			//if (write > checkValue && write < checkValue + 10) cout << endl << "##############" << write << "#################" <<  endl;
			//first bits
			readCH = 0;
			output.read((char*)&readCH, sizeof(char));
			output.read((char*)&readCH, sizeof(char));
			//if (write > checkValue && write < checkValue + 10) cout << static_cast<int>(readCH) << "=read|";
			writeCH = ((readCH >> 4) << 4);
			//if (write > checkValue && write < checkValue + 10)  cout << static_cast<int>(writeCH) << "=shifted|";
			imgstream.read((char*)&imgreadCH, sizeof(char));
			//if (write > checkValue && write < checkValue + 10)  cout << static_cast<int>(imgreadCH) << "=img|";
			temp = (imgreadCH << 4);
			//if (write > checkValue && write < checkValue + 10)  cout << static_cast<int>(temp) << "=temp|";
			writeCH |= (temp >> 4);
			pos = output.tellp();
			output.seekp(pos - 1);
			output.write((char*)&writeCH, sizeof(char));
			//if (write > checkValue && write < checkValue + 10)  cout << static_cast<int>(writeCH) << "=write|" << endl;
			
			
			//output.seekp(pos - 1);
			//output.read((char*)&readCH, sizeof(char));
			//if (write > checkValue && write < checkValue + 10)  cout << static_cast<int>(readCH) << "=readagain|" << endl;

			
			//second bits
			readCH = 0;
			output.read((char*)&readCH, sizeof(char));
			output.read((char*)&readCH, sizeof(char));
			//if (write > checkValue && write < checkValue + 10)  cout << static_cast<int>(readCH) << "=read|";
			temp = (readCH >> 4);
			writeCH = (temp << 4);
			writeCH |= (imgreadCH >> 4);
			
			pos = output.tellp();
			output.seekp(pos - 1);
			output.write((char*)&writeCH, sizeof(char));

			output.seekp(pos - 1);
			output.write((char*)&writeCH, sizeof(char));
			//if (write > checkValue && write < checkValue + 10)  cout << static_cast<int>(writeCH) << "=write|" << endl;
			
			output.seekp(pos - 1);
			output.read((char*)&readCH, sizeof(char));
			//if (write > checkValue && write < checkValue + 10)  cout << static_cast<int>(readCH) << "=readagain|" << endl;
			
			write++;
		}
		cout << "DONE: " << write << " # of modifications" << endl;
		output.close();
		imgstream.close();
	}
	return true;
}

bool WAV::ExtractFile(string new_img, string wav_file)
{
	fstream output(new_img, ios::out | ios::binary);
	fstream extract_stream(wav_file, ios::in | ios::out | ios::binary);
	if (output.is_open() && extract_stream.is_open())
	{ 	
		extract_stream.seekp(WAVDATASTART, ios::beg); //where data begins
		unsigned char readCH, writeCH = 0, temp;
		size_t write = 0;
		//size_t checkValue = 132093;
		while (MAGICNUMBER > write)
		{
			//first bits
			readCH = 0;
			extract_stream.read((char*)&readCH, sizeof(char));
			extract_stream.read((char*)&readCH, sizeof(char));
			//if (write > checkValue && write < checkValue + 10)  cout << static_cast<int>(readCH) << "=read(1st4)|";
			temp = (readCH << 4);
			writeCH = (temp >> 4);
			readCH = 0;
			extract_stream.read((char*)&readCH, sizeof(char));
			extract_stream.read((char*)&readCH, sizeof(char));
			//if (write > checkValue && write < checkValue + 10)  cout << static_cast<int>(readCH) << "=read2(2nd4)|";
			writeCH |= (readCH << 4);
			output.write((char*)&writeCH, sizeof(char));
			//if (write > checkValue && write < checkValue + 10)  cout << static_cast<int>(writeCH) << endl;
			write++;
		}
		cout << "DONE: " << write << " # of modifications" << endl;
		output.close();
		extract_stream.close();
	}	
	return true;
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
void WAV::PrintAll() const
{
	cout << "All chunks for {" << file_name << "}" << endl; 
	cout << "=============" << endl;
	PrintRiff();
	cout << "=============" << endl;
	PrintFmt();
	cout << "=============" << endl;
	PrintData();
	cout << "=============" << endl;
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