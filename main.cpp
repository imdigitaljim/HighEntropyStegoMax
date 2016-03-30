#include <iostream>
#include <string>
#include <vector>
#include "wavmod.h"

using namespace std;

bool IsValidSelection(char c);
void GetInput(char& data);
void PromptFileMenu();
const vector<string> wav_options = 
{
	"PRINT WAV METADATA [prompts(wavfile)]",
	"INJECT DATA INTO WAV [prompts(wavfile, hidefile)]",
	"EXTRACT DATA FROM UPLOADED WAV [prompts(wavfile, namefile, bytes)]"
};

int main(int argc, char** argv)
{
	string wavfile, datafile, bytes;
	char selection;
	WAV x;
	while (true)
	{
		PromptFileMenu();
		GetInput(selection);
		switch (selection)
		{
			case '0':
				cout << "WAV FILENAME?: ";
				getline(cin, wavfile);
				x = WAV(wavfile);
				x.PrintMetaData();
				break;
			case '1':
				cout << "WAV FILENAME?: ";
				getline(cin, wavfile);
				cout << "TO INJECT FILENAME?: ";
				getline(cin, datafile);
				x.HideFile(wavfile, datafile);
				break;				
			case '2':
				cout << "WAV FILENAME?: ";
				getline(cin, wavfile);
				cout << "NAME THE EXTRACTED FILE: ";
				getline(cin, datafile);
				cout << "SECRET FILESIZE(bytes)?: ";
				getline(cin, bytes);
				x.ExtractFile(wavfile, datafile, stoul(bytes));
				break;
			default:
				cerr << selection << ": invalid input" << endl;
		}
	}
	return 0;
}

void PromptFileMenu()
{
	cout << "STEGANOGRAPHIC INJECTION/EXTRACTION MENU:";
	cout << BR << "RIFF MICROSOFT 16BIT PCM WAV TOOLS" << BR;
	for (size_t i = 0; i < wav_options.size(); ++i)
	{
		cout << i << ") " << wav_options[i] << endl;
	}
	cout << "X) EXIT" << BR;
}
void GetInput(char& data)
{
	string input;
	cout << "Selection: ";
	getline(cin, input);
	while (!IsValidSelection(input[0]))
	{
		cout << input[0] << ": invalid option" << endl;
		cout << "Selection:" << flush;
		getline(cin, input);
	}
	data = input[0];
}

bool IsValidSelection(char c)
{
	if (c - '0' >= 0 && c - '0' <= 3)
	{
		return true;
	}
	c = toupper(c);
	switch(c)
	{
		case 'X': exit(0);
		default: return false;
	}
}




 