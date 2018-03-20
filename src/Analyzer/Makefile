#
# MPI Analyzer Makefile
#
# (c) 2010 Anna Morajko (UAB)
#
# Version for LINUX.
#

# Relative path to source directory
TO_SRC = ..

# Include the make configuration specification
include ${TO_SRC}/make.configure

# Libraries to be linked with the Analyzer
# LDFLAGS		+= $(DYNINST_LIB) -lnsl -lpthread -lrt $(ARCHLIB) -ldl
LDFLAGS		+= -lnsl -lpthread -lrt $(ARCHLIB) -ldl
                                        
# Analyzer objects 
objects =  AppEvent.o AppTask.o AppModel.o main.o Ctrl.o Service.o ECPHandler.o \
		   ACProxy.o ECPProtocol.o \
		  EventCollector.o DTAPI.o FactoringStats_nw.o FactoringTunlet_nw.o \
		  EventMsgReader.o ShutDownManager.o
	
#		   EventCollector.o DTAPI.o FactoringStats.o AdjustingNWTunlet.o 
#FactoringTunlet.o FactoringOptimizer.o FactoringStats.o 
           
default: all

install: all
	@if [ $(PREFIX) = "." ]; then\
		cp ./bin/Analyzer /bin/Analyzer;\
	else\
		cp $(TO_SRC)/$(PROGRAMS_CONF)/analyzer_template.ini $(PREFIX)/bin/Analyzer.ini;\
		cp $(TO_SRC)/$(PROGRAMS_CONF)/tunlet_template.ini $(PREFIX)/bin/Tunlet.ini;\
		cp ./bin/Analyzer $(PROGRAMS_DEST)/Analyzer;\
	fi

all: bin/Analyzer
	@echo "Analyzer finished"

bin/Analyzer: $(objects)
	$(GXX) $(CFLAGS) -o $@ $(objects) $(lib_objs) $(LDFLAGS) -I../Common -L../Common -lanalyzer

%.o: %.cpp
	$(GXX) $(CFLAGS) -c $< -I../Common

clean:
	$(RM) *.o $(TO_SRC)/Common/*.o $(PROGRAMS_DEST)/Analyzer
