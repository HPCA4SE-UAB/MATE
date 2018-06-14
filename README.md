# README #

## What is this repository for? ##
This repository contains the latest stable version of MATE. It also includes a template example with the XFire application.

## How do I get set up? ##

##### Dependencies #####
Programs: [`Dyninst`](https://github.com/dyninst/dyninst), [`Papi`](http://icl.cs.utk.edu/papi/)

Libraries:  [`Boost`](https://www.boost.org/), [`binutils`](https://www.gnu.org/software/binutils/), [`libdwarf`](https://github.com/tomhughes/libdwarf), [`libdw`](), [`libelf`](https://directory.fsf.org/wiki/Libelf), [`libiberty`](https://github.com/gcc-mirror/gcc/tree/master/libiberty)


##### Configuration and Installation #####
It is only required to specify the path of those libraries that are not in the system library paths (e.g. `/lib`, `/usr/lib` etc.) or not included in `LD_LIBRARY_PATH`. 

* Configure help:
    `$ ./configure --help`

* Installation :

    `$ ./configure --with-dyninst=<DYNINST_PATH> --prefix=<PATH>`
  
    `$ make && make install`
    
    Finally refresh the .bashrc file:

    `$ source ~/.bashrc`


Note: By default, if `--prefix` is not specified, MATE will be installed in `/lib` and `/bin` (with sudo). 

***
## XFire example
##### How to install and run XFire #####
* Install XFire from examples/xfire and following the instructions in file INSTALL
* Once installed, change the following two lines in AC.ini inside the XFire download directory:

    `ACPath=/path/to/MATEs/AC`

    `DMLib=/path/to/MATEs/libDMLib.so`

`ACPath` is the full path to the AC binary and `DMLib` the full path to libDMLib. If no prefix was used when installing, these will be in `/bin` and `/lib` respectively. 

* Now, run the Analyzer:
    
    `$ Analyzer -config Analyzer.ini  xfire`

* In a new terminal and inside the same download directory of XFire, run the AC:

    `$ mpirun -np 4 AC xfire demo`