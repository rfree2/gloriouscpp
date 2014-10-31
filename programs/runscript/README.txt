
The runscript is a tiny program/wrapper that allows to run simple C++ programs directly from source without manual compilation.

Once done, you can directly execute C++ source code by entering directory example/ and doing: ./multiply.cpp 3.14 2

=== HOOK ===

There are different ways to hook in C++ source code as "interpreted" script for direct execution:

1. with #! interpreter, as per http://en.wikipedia.org/wiki/Shebang_%28Unix%29
this requires all such C++ programs to start with line as in example/ and probably makes them 
to have a syntax error if they would be compiled
	1.a. install the interpreter for the user, locally in his $HOME ; make sure he has proper $PATH
	1.b. install the interpreter system wide, e.g. /usr/local/bin/
2. to install binfmt_misc for it, and mark binaries with #pragma tinycpp11 as 1st line/character

Currently we support 1a and 1b.
TODO method 2.


=== BUILD ===

No building, it's a bash script.

=== INSTALL ===

Run install-root.sh to install as root, or install-user.sh to install as current user in your home.


