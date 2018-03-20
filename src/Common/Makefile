#
# Common Makefile
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
LDFLAGS		= -lnsl
                                        
# Objects to be used from Common directory
#lib_objs = Syslog.o Exception.o SysException.o DateTime.o Env.o Paths.o configreader.o ConfigException.o \
#	   Address.o Utils.o 
# Objects of the library 
#objects =  DMLibApi.o ECPMsgHeader.o Socket.cpp ECPProxy.o

#objects = EventMsgWriter.o ActiveObject.o FuncDefException.o Reactor.o Address.o FuncDefs.o ECPMsgHeader.o Socket.o ECPProxy.o sync.o ConfigException.o \
#Env.o Paths.o SysException.o Event.o Process.o Syslog.o ConfigReader.o EventException.o PTPMsg.o Thread.o DateTime.o PTPMsgHeader.o TimeValue.o \
#di.o EventMap.o PTPProtocol.o DMLibApi.o Exception.o Utils.o

objects = ActiveObject.o Env.o FuncDefs.o Socket.o Address.o EventException.o Paths.o sync.o ConfigException.o EventMap.o Process.o SysException.o \
ConfigReader.o PTPMsgHeader.o Syslog.o DateTime.o PTPMsg.o Thread.o di.o Exception.o PTPProtocol.o TimeValue.o Pipe.o NetSer.o \
ECPMsgHeader.o FuncDefException.o Reactor.o Utils.o ConfigMap.o ByteStream.o Config.o ECPMsg.o

DMLib_objects = Syslog.o Exception.o SysException.o DateTime.o Env.o Paths.o ConfigReader.o \
			Address.o Utils.o ECPMsgHeader.o Socket.o ECPMsg.o ConfigException.o \
			ByteStream.o Reactor.o Config.o ConfigMap.o TimeValue.o Pipe.o NetSer.o sync.o

Analyzer_objects = PTPProtocol.o PTPMsg.o PTPMsgHeader.o \
		   Address.o Reactor.o Socket.o NetSer.o \
		   SysException.o Exception.o TimeValue.o DateTime.o Env.o Paths.o \
		   Syslog.o ConfigReader.o FuncDefs.o Utils.o sync.o Thread.o ActiveObject.o \
		   ECPMsg.o ECPMsgHeader.o Config.o ConfigMap.o ByteStream.o \
                   ConfigException.o FuncDefException.o

AC_objects = DateTime.o TimeValue.o Exception.o \
             SysException.o Reactor.o Process.o \
             Address.o Socket.o PTPProtocol.o \
             PTPMsg.o PTPMsgHeader.o Syslog.o \
             di.o ConfigReader.o Utils.o Env.o \
	     	 Paths.o Config.o ConfigMap.o ByteStream.o \
             ConfigException.o ECPMsg.o ActiveObject.o \
             NetSer.o Thread.o sync.o

default: all

all: $(objects)
	@ar rc libDM.a $(DMLib_objects)
	@ranlib libDM.a
	@ar rc libanalyzer.a $(Analyzer_objects)
	@ranlib libanalyzer.a
	@ar rc libAC.a $(AC_objects)
	@ranlib libAC.a
      
%.o: %.cpp
	$(GXX) $(CFLAGS) -c $< 

install: all

clean:
	$(RM) *.o
	$(RM) *.a
	

