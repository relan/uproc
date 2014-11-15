µProcessor
==========
Shows which features your CPU does (or does not) support. It consists of three
utilities:

* **dump**. Prints _profile_: generic info about system and raw values of
  specific pre-defined registers. Written in C and assembly.
* **convert**. Checks profile syntax and translates it into the form
  convenient for the belowmentioned utility. Written in lex, yacc and C.
* **parse**. Interprets converted profile data and prints whether particular
  features are supported. Written in Scheme.

Prerequisites
-------------

The following tools are required to build uProcessor:

* CMake.
* C compiler.
* Lex.
* Bison.
* Chicken Scheme with the `numbers` egg.

Install prerequisites on Debian or Ubuntu:

    sudo apt-get update
    sudo apt-get install cmake gcc flex bison chicken-bin
    chicken-install -s numbers

Install prerequisites on Fedora, RHEL or CentOS:

    sudo yum install cmake gcc flex bison chicken
    chicken-install -s numbers

Install prerequisites on OS X (given that you already have
[Xcode](http://itunes.apple.com/us/app/xcode/id497799835?ls=1&mt=12) and
[Homebrew](http://brew.sh)):

    brew update
    brew install cmake chicken
    chicken-install -s numbers

Building
--------
Build commands for GNU/Linux, OS X and Solaris:

    mkdir build
    cd build
    cmake ..
    make

Build commands for Windows:

    md build
    cd build
    cmake -G "NMake Makefiles" ..
    nmake /nologo

Usage
-----
All CPU-specific information is taken from MSRs (Model Specific Registers)
and CPUID instruction. While CPUID is available to user space code, MSRs can
be accessed only from kernel space.

The `updump` utility on GNU/Linux can use `/dev/cpu/0/msr` interface provided
by `msr.ko` kernel module (available out of the box in most distributions).
On OS X `updump` can use a similar interface provided by the companion
[project](https://github.com/relan/msr.kext). In both cases `updump` needs to
be run with root privileges. MSRs reading on Windows and Solaris is not
supported.

View CPU features:

    sudo dump/updump | convert/upconvert | parse/upparse

Dump profile for further exploration:

    sudo dump/updump > profile.txt

View CPU features from the previously saved profile:

    convert/upconvert < profile.txt | parse/upparse
