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

include ../source/.Makefile.arch
include ../source/.Makefile.REST

# Name of RESTsoft module(s) to compile
MODULE = MyRESTLibrary

# Library to be compiled
PATH_LIB = $(REST_PATH)/lib
RESTLIB	= $(PATH_LIB)/lib$(MODULE).so

# Added due to the compiler new version
ROOTLIBS := $(shell root-config --libs)

# Dependencies
DEPLIBS	=       -lRestExternal -lRestViewers -lRestTools -lRestEvents -lRestMetadata -lRestProcesses -lRestCore
DEPLIBS += -lm  -lGui ${ROOTLIBS} -lEve -lGeom -L$(PATH_LIB) -L/usr/lib -L$(GARFIELD_HOME)/lib -lGarfield

# Classes and objects of the library
# add names to this list whenever a new class is added to the library
CLASSES = myManager myProcess myMetadata
CLASS_DICT = ${CLASSES}

INC_DIRS += -I$(REST_PATH)/examples/inc



# with this syntax I generate, for every classname, two object files to compile,
# the normal one and the dictionary
OBJS = $(patsubst %, dict/%.o ,$(CLASS_DICT))
OBJS += $(patsubst %,obj/%.o ,$(CLASSES))

#------------------------------------------------------------------------------
#
# Defining the compiling targets
#
#------------------------------------------------------------------------------

all: $(RESTLIB) $(OBJS) restLauncher

#------------------------------------------------------------------------------
# Object generation of each class.
#------------------------------------------------------------------------------

obj/%.o: src/%.cxx inc/%.h 
# removed dict dependency...
	@echo "\033[40m\033[0;31m  Compiling object $@ ........\033[0m"
	@$(CC) $(FLAGS) $(INC_DIRS) -c -o $@ $<

dict/%.cxx: inc/%.h 
	@echo "\033[40m\033[0;31m Generating dictionary $@... \033[0m"
	@rootcint -f $@ -c $(INC_DIRS) $(INC_GARFIELD) $^

dict/%.o: dict/%.cxx
	@echo "\033[40m\033[0;32m Compiling object $@ \033[0m"
	@$(CC) $(FLAGS) $(INC_DIRS) -c -o $@ $<

#------------------------------------------------------------------------------
# Binary
#---------------------------
		
% :	%.cc $(OBJS)
	@echo "\033[41m\033[0;35m Compiling $@ binary \033[0m"
	@g++ $(COPTS) -s -o $@ $< $(OBJS) $(LIBS) $(INC_DIRS) $(DEPLIBS)


#------------------------------------------------------------------------------
# Library
#---------------------------

$(RESTLIB): $(OBJS)
	@echo "\033[40m\033[0;34m  Creating shared library "$(RESTLIB)"........\033[0m"  
	@$(CC) $(FLAGS) -shared -o $(RESTLIB) $(OBJS) $(LIBS)

#------------------------------------------------------------------------------
# Cleaning.
#------------------------------------------------------------------------------

clean: 
	@echo "\033[40m\033[0;34m  Removing objects ........\033[0m"
	@rm -f $(OBJS)
	@echo "\033[40m\033[0;34m  Removing dictionaries ........\033[0m"
	@rm -f dict/*
	@echo "\033[40m\033[0;34m  Removing restLauncher ........\033[0m"
	@rm -f restLauncher
	@echo "\033[40m\033[0;34m  Removing library ........\033[0m"
	@rm -f $(RESTLIB)


#------------------------------------------------------------------------------
# end of makefile
