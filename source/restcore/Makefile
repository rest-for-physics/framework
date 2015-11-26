#------------------------------------------------------------------------------
#                                                                      
#                                                                      
#   RESTSoft : Software for Rare Event Searches with TPCs
#   Makefile to create the RESTsofts libraries
#    (temporary until unifications of makefiles)
#                                                                      
#      submodule: RESTCORE
#    
#   Created  :  June 2014                                             
#   Auth: Igor G. Irastorza                                            
#   Based on old Restsoft makefiles 
#
#------------------------------------------------------------------------------
# generic info on makefiles:
# http://www.gnu.org/software/make/manual/make.html
#------------------------------------------------------------------------------

include ../.Makefile.arch
include ../.Makefile.REST

# Name of RESTsoft module(s) to compile
MODULE = RestCore

# Library to be compiled
PATH_LIB = $(REST_PATH)/lib
RESTLIB	= $(PATH_LIB)/lib$(MODULE).so

# Added due to the compiler new version
ROOTLIBS := $(shell root-config --libs)

# Dependencies
DEPLIBS = -lm -lGui ${ROOTLIBS} -lEve -lGeom -L$(PATH_LIB) -L/usr/lib -L$(GARFIELD_HOME)/lib -lGarfield

# Classes and objects of the library
# add names to this list whenever a new class is added to the library
CLASSES = TRestEvent TRestMetadata TRestEventProcess TRestRun TRestGeometry TRestRunMerger TRestEventViewer TRestBrowser# TRestSignal# TRestManager
CLASS_DICT = ${CLASSES}

# with this syntax I generate, for every classname, two object files to compile,
# the normal one and the dictionary
OBJS = $(patsubst %, dict/%.o ,$(CLASS_DICT))
OBJS += $(patsubst %,obj/%.o ,$(CLASSES))



#------------------------------------------------------------------------------
#
# Defining the compiling targets
#
#------------------------------------------------------------------------------

all: Wellcome $(RESTLIB)

#------------------------------------------------------------------------------
# Wellcome message
#------------------------------------------------------------------------------

Wellcome:
	@echo "\033[40m\033[0;1;32m----------------------------         \033[0m"
	@echo "\033[40m\033[0;1;32mRESTsoft Module Makefile         \033[0m"
	@echo "\033[40m\033[0;1;32m----------------------------         \033[0m"
	@echo "\033[40m\033[0;1;32mCompiling $(MODULE) module            \033[0m"
	@echo "\033[40m\033[0;1;32m----------------------------        \033[0m"
	

#------------------------------------------------------------------------------
# Generation of the library.
#------------------------------------------------------------------------------

$(RESTLIB): $(OBJS)
	@echo "\033[40m\033[0;34m  Creating shared library "$(RESTLIB)"........\033[0m"  
	@$(CC) $(FLAGS) -shared -o $(RESTLIB) $^ $(DEPLIBS)



#------------------------------------------------------------------------------
# Object generation of each class.
#------------------------------------------------------------------------------

obj/%.o: src/%.cxx inc/%.h 
# removed dict dependency...
	@echo "\033[40m\033[0;31m  Compiling object $@ ........\033[0m"
	@$(CC) $(FLAGS) $(INC_DIRS) -c -o $@ $<

dict/%.cxx: inc/%.h 
	@echo "\033[40m\033[0;31m Generating dictionary $@... \033[0m"
	@rootcint -f $@ -c $(INC_GARFIELD) $^

dict/%.o: dict/%.cxx
	@echo "\033[40m\033[0;32m Compiling object $@ \033[0m"
	@$(CC) $(FLAGS) $(INC_DIRS) -c -o $@ $<

#------------------------------------------------------------------------------


#------------------------------------------------------------------------------
# Cleaning.
#------------------------------------------------------------------------------

clean: 
	@echo "\033[40m\033[0;34m  Removing objects ........\033[0m"
	@rm -f $(OBJS)
	@echo "\033[40m\033[0;34m  Removing main library ........\033[0m"
	@rm -f $(RESTLIB)
	@echo "\033[40m\033[0;34m  Removing dictionaries ........\033[0m"
	@rm -f dict/*

install:
	@echo "\033[40m\033[0;34m  Installing "$(RESTLIB)" in /usr/lib ........\033[0m"
	@cp $(RESTLIB) /usr/lib


#------------------------------------------------------------------------------
# end of makefile
