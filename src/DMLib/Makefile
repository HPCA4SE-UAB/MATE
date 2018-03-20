#
# MPI DMLib Makefile
#
# (c) 2010 Anna Morajko (UAB)
#
# Version for LINUX.
#

# Relative path to source directory
TO_SRC = ..

# Include the make configuration specification
include ${TO_SRC}/make.configure

OPT_FLAG	= -g -fPIC -Wno-deprecated

# Libraries to be linked with the DMLib
LDFLAGS		+= -lnsl
                                                  
# Objects of the library 
objects =  DMLibApi.o EventMsgWriter.o ECPProxy.o

default: all

install: all
	@if [ $(PREFIX) = "." ]; then\
		cp DMLib.so /lib/libDMLib.so;\
	else\
		cp DMLib.so $(PREFIX)/lib/libDMLib.so;\
	fi
	
all: DMLib.so
	@echo "DMLib finished"

./DMLib.so: $(objects)
	$(GXX) $(CFLAGS) -shared -o $@ $(objects) $(lib_objs) -I../Common $(LDFLAGS) -L../Common -lDM -lpapi

%.o: %.cpp
	$(GXX) -I../Common $(CFLAGS) -c $< 

clean:
	$(RM) *.o $(LIBRARY_DEST)/DMLib.so
	
#%.o:	%.c %.cpp
#	$(GCC) $(CFLAGS) -c $< -o $@

