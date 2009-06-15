# ccsponge makefile

# output executable name
OUTNAME = ccsponge

# compiler name
CC = g++

# current directory
PWD = $(shell pwd)

# include flags
INCFLAGS = -I$(PWD)/src -I$(PWD)/unix

# compiler flags
CFLAGS = -g -O2 $(INCFLAGS)

# libraries to link
LIBS = -lpthread

# mainfile name
MAINFILE = src/main.cpp

# object files needed
OBJS = src/Help.o \
	src/Settings.o \
	src/clearcase/AnalyzeTask.o \
	src/clearcase/CtFindTask.o \
	src/clearcase/DataEntry.o \
	src/clearcase/DataStore.o \
	src/clearcase/Description.o \
	src/clearcase/FileDiff.o \
	src/io/TextReader.o \
	src/io/TextWriter.o \
	src/text/String.o \
	src/thread/ThreadPool.o \
	src/thread/WorkerThread.o \
	src/util/Date.o \
	src/util/Locker.o \
	unix/io/FileInputStream.o \
	unix/io/FileOutputStream.o \
	unix/thread/Condition.o \
	unix/thread/Mutex.o \
	unix/thread/Process.o \
	unix/thread/Thread.o \
	unix/util/UnixUtil.o \

##################
# Default Target #
##################

$(OUTNAME): $(OBJS) $(MAINFILE)
	$(CC) $(CFLAGS) $(LIBS) -o $(OUTNAME) $(OBJS) $(MAINFILE)

############################
# Generic object file rule #
############################

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

##############
# Clean rule #
##############

.PHONY : clean
.IGNORE : clean
clean:
	rm $(OBJS) $(OUTNAME) $(OUTNAME).exe *~ core 2>/dev/null