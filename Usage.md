#Running ccsponge

# Introduction #

Quick tutorial on how to run ccsponge.

# Terms #

Some terms for those familiar with subversion.

  * vob : The ClearCase vob is the repository of data that it stores all files, versions, labels and other information in. The vob is both a database and a virtual file system.
  * element : An element is essentially a file stored in the vob. An element may have many versions and many branches.
  * version : A version is a single instance of a committed change to an element in the vob. It is comparable to a subversion commit of a single file.
  * brtype : A brtype is close to the idea of a branch in subversion. It is called a brtype because files are branched individually, not en-mass.
  * branch : When a user checks out an element, branches are created as needed. A branch is an instance of some brtype.
  * label : Elements, versions and other objects can have labels applied to them. Labels are pretty much what they sound like. Many labels can be applied to a single object.
  * config spec : A config spec defines a way of looking at the vob file system. You can, for example, view certain brtypes but not others, view versions before a specific date, etc.
  * merge : A merge is basically the same concept as it is in subversion. If someone else updates a file you have checked out, you need to resolve the differences. ClearCase allows you to merge from any version on any branch, to any other version, so not all merges are the result of conflicts.

# Using ccsponge #

The basic idea of the application is that the user enters a path (or set of paths) and a series of optional filters. The filters will be used to limit the files looked at. Lets look at a simple example.

ccsponge /vobs/dev/ -users 'spongebob,patrick' -exts '.java, .h, .c, .cpp' -nomerge

The first parameter specifies a path to search for versions on. The -users parameter indicates we are only interested in versions created by the listed users. The -exts parameter indicates we are only interested in versions with one of the listed extensions. The -nomerge parameter indicates we are not interested in any versions that were merged to.

When this command is run, assuming it is successful, it will produce a file called 'sponge.out' (output file changed with -o). The contents will look something like this:

```
Date,Lines Added,Lines Changed,Lines Removed,Total Lines
2008-05-04,200,23,77,300
2008-05-11,99,153,511,763
2008-05-18,0,0,0,0
2008-05-25,0,0,0,0
2008-06-01,2,33,75,110
```

We now have some data on all the code changes made by spongebob and patrick in the dev vob. Note that we used the -nomerge parameter. This can be useful for excluding versions created by merges, but it can be dangerous. Depending on how you use ClearCase, versions with merge arrows to them might be legitimate changes.

The data is outputted in weekly amounts by default. You can specify the -period command to change the collection period. If we had passed in "-period monthly" the result would have been:

```
Date,Lines Added,Lines Changed,Lines Removed,Total Lines
2008-05-01,299,176,588,1063
2008-06-01,2,33,75,110
```

Several other possible constraints can be passed in. If you type "ccsponge -help" it will print a listing of options. You can get the details on any option by typing "ccsponge -help (option name)".

# Supported Parameters #

Here is a dump of the help text for each of the supported options.

  * -o -out : Specifies an output file name. If -o is not passed, a file named "sponge.out" is created.

  * -nomain : Ignores versions on the "main" branch. Useful if all development is done off of "main" and you want to ignore main all-together. Ignored if the -brtypes parameter is also passed.

  * -nomerge : Ignores versions with a merge arrow to them. Useful for ignoring rebase versions. Be cautious with this option as it may ignore legitamate changes.

  * -after DATE : Ignores versions before the specified DATE. The passed date must be a date in a format clearcase recognizes. If -after is not passed, no minimum date restriction is applied.

  * -before DATE : Ignores versions after the specified DATE. The passed date must be a date in a format clearcase recognizes. If -before is not passed, no maximum date restriction is applied.

  * -period PERIOD : Specifies a period of time to write output data in. Supported PERIOD values are: "daily", "weekly", "monthly". If -period is not passed, defaults to "weekly"

  * -users USER-LIST : Specifies a list of users to look for when examining versions. Versions created by users that are not in the passed list will be ignored. The list of users should be comma delimited: "jsmith, alincoln, bcosby". If -users is not passed, any creator is accepted.

  * -brtypes BRTYPE-LIST : Specifies a list of clearcase brtypes to look for when examining versions. Versions that are not on any of the passed brtypes will be ignored. The BRTYPE-LIST should be comma delimited: "fixes1, fixes2, new\_dev". If -brtypes is not passed, all brtypes are considered. Passing -brtypes causes the -nomain option to be ignored.

  * -exts EXTENSION-LIST : Specifies a list of file extensions when examining versions. Versions that do not have any of the passed extensions will be ignored. The EXTENSION-LIST should be comma delimited: ".h, .cpp,.hpp". If -exts is not passed, all extensions are allowed.