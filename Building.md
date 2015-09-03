#Building ccsponge

# Unix #

The subversion repository comes with a simple makefile which will build using gcc. It has been tested on Solaris 10 and on Windows using Cygwin. Small changes will likely be required to get a build working on a new Unix platform.

If you want to get a build going with another compiler, please implement a "proper" GNU configure setup. Try to keep compiler specific defines to the base ccsponge.h header.

# Windows #

A Visual Studio 2008 project is checked into the subversion repository. You can download a free version of Visual Studio Express to do a build (Google it). Modifications will likely be required to build on earlier versions of Visual Studio.

You can build under Cygwin, but be warned that cleartool has issues dealing with some Cygwin file names.

Because it is sometimes necessary, but few projects document it, here is how build a Visual Studio project from scratch:

  * Create an Windows console application. Make sure you check the "Empty Project" box.
  * Add the files to the project
  * Open the project properties
  * Under general, set "Character Set" to "Use Multi-Byte Character Set"
  * Under C/C++, set "Additional Include Directories" to "src;win" (If the path to those folders is different, add a complete path)
  * Under Linker/Input, set "Additional Dependencies" to "Winmm.lib User32.lib"