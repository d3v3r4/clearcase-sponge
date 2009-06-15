// ccsponge.cpp

#include <ccsponge.h>
#include <Help.h>
#include <Settings.h>
#include <clearcase/CtFindTask.h>
#include <exception/IOException.h>
#include <exception/ParsingException.h>
#include <io/FileOutputStream.h>
#include <thread/ThreadPool.h>

#include <iostream>
using namespace std;

#include <thread/Process.h>

int main(int argc, char* argv[])
{
	Process process;
	process.execCommand("ls", true);

	String str;

	do
	{
		str = process.readStdOutLine();
		cout << str << endl;
	}
	while (str.length() > 0);

	process.waitFor();
	return 0;

	/*
	try
	{
		// Make the arguments into an Array of String
		Array<String> args(argc);

		for (int i = 0; i < argc; i++)
		{
			args.set(i, String(argv[i]));
		}

		// Print help if needed
		if (Help::isHelpRequest(args))
		{
			cout << Help::getHelpMessage(args) << endl;
			return 0;
		}

		// Parse the options
		String settingsError;
		Settings settings;

		settings.populate(args, settingsError);

		if (settingsError.length() > 0)
		{
			cerr << settingsError.c_str() << endl;
			return 1;
		}

		// Open an output stream to the output file
		String outputFileName = settings.getOutputFile();
		FileOutputStream outStream;
		outStream.open(outputFileName);

		// Make the DataStore object to hold the result
		DataStore dataStore(&settings);

		// Make our thread pool
		// 4 thread max
		// Threads stop if idle for three seconds
		// Max queue size of 200 items
		ThreadPool threadPool(4, 3000, 200);

		// Put the first task in the thread pool
		CtFindTask* ctFindTask = new CtFindTask(&threadPool, &dataStore, &settings);
		threadPool.execute(ctFindTask);

		// This will block until all every task in the thread pool has completed
		threadPool.shutdownWhenEmpty();

		// Print the data to the output file
		dataStore.writeToStream(outStream);
		outStream.close();

		return 0;
	}
	catch (exception& e)
	{
		cout << e.what() << endl;
	}
	catch (...)
	{
		cout << "Unknown Exception" << endl;
	}
	*/
}
