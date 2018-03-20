#
# MPI AC Makefile
#
# (c) 2010 Anna Morajko (UAB)
#
# Version for LINUX.
#

# Relative path to source directory
TO_SRC = ..

# Include the make configuration specification
include ${TO_SRC}/make.configure

# Libraries to be linked with the AC
LDFLAGS		+= $(DYNINST_LIB) -lnsl $(ARCHLIB) -ldl -lpthread

# Includes from Common
COMMON_HEADERS = -I../Common
      
# MPI AC objects 
objects =  main.o Ctrl.o TaskManager.o \
           Monitor.o Tuner.o InstrSet.o \
           TaskInstr.o Task.o SnippetMaker.o \
           PTPAcceptor.o PTPHandler.o ShutDownSlave.o

# Events.o 
default: all

install: all
	@if [ $(PREFIX) = "." ]; then\
		cp ./bin/AC /bin/AC;\
	else\
		cp $(TO_SRC)/$(PROGRAMS_CONF)/ac_template.ini $(PROGRAMS_DEST)/AC.ini;\
		cp ./bin/AC $(PROGRAMS_DEST)/AC;\
	fi
	
all: bin/AC
	@echo "AC finished"

bin/AC: $(objects)
	$(MKDIR) bin/
	$(GXX) $(CFLAGS) -o $@ $(objects) $(LDFLAGS) $(COMMON_HEADERS) -L../Common -Wl,--rpath,$(DYNINST_LDIR) -lAC -lpapi
	
%.o: %.cpp
	$(GXX) $(CFLAGS) $(LDFLAGS) $(COMMON_HEADERS) -c $< 

clean:
	$(RM) *.o $(TO_SRC)/Common/*.o $(PROGRAMS_DEST)/AC
