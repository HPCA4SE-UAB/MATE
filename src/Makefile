include ./make.configure

.PHONY: doc
.PHONY: cleandoc
.PHONY: install

SUBDIRS = Common DMLib AC Analyzer

default:	all

all:
	@for dir in ${SUBDIRS}; do \
	  (cd $$dir && $(MAKE) all) \
	  || case "$(MFLAGS)" in *k*) fail=yes;; *) exit 1;; esac; \
	done && test -z "$$fail"

install:
	@if [ $(PREFIX) != "."  ]; then\
		$(MKDIR) $(LIBRARY_DEST);\
		$(MKDIR) $(PROGRAMS_DEST);\
	fi
	@for dir in ${SUBDIRS}; do \
	  (cd $$dir && $(MAKE) install) \
	  || case "$(MFLAGS)" in *k*) fail=yes;; *) exit 1;; esac;\
	done && test -z "$$fail";\
	echo "# Added by MATE:" >> ~/.bashrc; \
	echo 'export PATH=${PROGRAMS_DEST}:$$PATH' >> ~/.bashrc; \
	echo "Please execute 'source ~/.bashrc' for the installation to be completed";\
	
clean:
	/bin/rm -f *~
	@for dir in ${SUBDIRS}; do \
	  (cd $$dir && $(MAKE) clean) \
	  || case "$(MFLAGS)" in *k*) fail=yes;; *) exit 1;; esac; \
	done && test -z "$$fail"
	
doc:
	doxygen	$(DOXYGEN_DIR)/doxyfile

cleandoc:
	rm -rf ../docs/html ../docs/latex
