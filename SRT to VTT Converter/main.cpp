#include <cstdlib>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <limits>
#include <string>
#include <tclap/CmdLine.h>

#if defined(_WIN32) || defined(WIN32)
#include <Windows.h>
#else
#include <sys/stat.h>
#endif

#include "Converter.h"

using namespace std;


bool isDir(const string &path)
{
#if defined(_WIN32) || defined(WIN32)
	DWORD ftype = GetFileAttributesA(path.c_str());
	if (ftype == FILE_ATTRIBUTE_DIRECTORY)
		return true;
#else
	struct stat st;
	lstat(dent->d_name, &st);
	if (S_ISDIR(st.st_mode))
		return true;
#endif

	return false;
}

int main(int argc, char* argv[])
{
	try {
		//Initialze the tclap command line parser
		TCLAP::CmdLine cmd(string("`srt-vtt` converts SubRip subtitles to the WebVTT subtitle format.\n")
				+ "If called without any arguments, all .srt files in the current directory will be converted and left within the current directory.\n"
				+ "Written by Nathan Woltman and distributed under the MIT license.",
			' ', "0.0.1");

		//Define arguements
		TCLAP::UnlabeledValueArg<string> inputArg("input",
			"File to convert or direcotry containing files to convert (to convert files in a directory and it's subdirectories, include the -r switch.",
			false, ".", "string");
		cmd.add(inputArg);

		TCLAP::ValueArg<int> offsetArg("t", "offset",
			"Timing offset in milliseconds. Can be any integer between " + to_string(INT_MAX) + " and " + to_string(INT_MIN) + ".",
			false, 0, "integer");
		cmd.add(offsetArg);

		TCLAP::ValueArg<string> outputArg("o", "output-dir",
			"The path to a directory where all output VTT files will be saved.",
			false, "", "string");
		cmd.add(outputArg);

		TCLAP::SwitchArg recursiveArg("r", "recursive", string()
			+ "If the input is a directory, this flag indicates its subdirectories will be searched recursively for .srt files to convert."
			+ " This flag is ignored if the input is a file.");
		cmd.add(recursiveArg);

		TCLAP::SwitchArg quietArg("q", "quiet", "Prevents details about the conversion from being printed to the console.");
		cmd.add(quietArg);

		//Parse the argv array.
		cmd.parse(argc, argv);

		//Get the value parsed by each arg.
		string input = inputArg.getValue();
		int timeOffset = offsetArg.getValue();
		string outputDir = outputArg.getValue();
		bool recursive = recursiveArg.getValue();
		bool quiet = quietArg.getValue();

		//Debugging
		cout << "Input is: " << input << endl;
		cout << "Offset time is: " << timeOffset << endl;
		cout << "Output directory is: " << outputDir << endl;
		cout << "Recursive? - " << recursive << endl;
		cout << "Quiet? - " << quiet << endl;

		if (outputDir.length() && !isDir(outputDir)) {
			system(string("mkdir \"" + outputDir + "\"").c_str());
		}

		//Convert
		Converter converter(timeOffset, outputDir, quiet);
		if (isDir(input)) {
			converter.convertDirectory(input, recursive);
		}
		else {
			converter.convertFile(input);
		}
	}
	catch (TCLAP::ArgException &e) {
		cerr << "error: " << e.error() << " for arg " << e.argId() << endl;
	}

	return 0;
}