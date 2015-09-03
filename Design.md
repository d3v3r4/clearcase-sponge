#Outline of the design

# Design #

The basic idea of the ccsponge application is that the main() function deals with the application parameters and then creates a ThreadPool object. The main function puts a priming CtFindTask into the ThreadPool and then does nothing but wait for the pool to complete all its work.

The CtFindTask constructs a string to pass to the "cleartool find" command and then reads the output line by line. Each line of output is a version that may need to be diffed against its predecessor. For each version, the CtFindTask will add an AnalyzeTask to the ThreadPool object. This allows the processing of the diff to occur asynchronously while the CtFindTask is still ongoing.

The AnalyzeTask will execute a series of cleartool queries to determine if it fully matches the user's parameters and to diff it against its predecessor. The result of each diff is accumulated in a DataStore object.

When all AnalyzeTasks complete, the main function will take its final action of writing the data in the DataStore object to disk.

# Rationale #

It might not be obvious, but if the program was single threaded it would take vastly longer. A single "cleartool find" query can take a good part of an hour to complete if it exhaustively searches a large vob. A single threaded approach tends to be IO bound on the find command.

While the find is ongoing, we can analyze the intermediate results of the find instead of sitting there doing nothing while ClearCase does its thing. Using multiple threads also lets us take advantage of multiple CPUs or CPU cores if they are present for the diffs.