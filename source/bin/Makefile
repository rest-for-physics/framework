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
MODULE = MyRESTLibrary

# Library to be compiled
PATH_LIB = $(REST_PATH)/lib

# Added due to the compiler new version
ROOTLIBS := $(shell root-config --libs)

# Dependencies
DEPLIBS	=       -lRestExternal -lRestViewers -lRestTools -lRestEvents -lRestMetadata -lRestProcesses -lRestCore
DEPLIBS += -lm  -lGui ${ROOTLIBS} -lEve -lGeom -L$(PATH_LIB) -L/usr/lib -L$(GARFIELD_HOME)/lib -lGarfield

#------------------------------------------------------------------------------
#
# Defining the compiling targets
#
#------------------------------------------------------------------------------

all: restManager restPlots

#------------------------------------------------------------------------------
# Binary
#---------------------------
		
% :	%.cxx
	@echo "\033[41m\033[0;35m Compiling $@ binary \033[0m"
	@g++ $(COPTS) -s -o $@ $< $(DEPLIBS) $(LIBS) $(INC_DIRS)


#------------------------------------------------------------------------------
# Cleaning.
#------------------------------------------------------------------------------

clean: 
	@echo "\033[40m\033[0;34m  Removing restManager ........\033[0m"
	@rm -f restManager
	@echo "\033[40m\033[0;34m  Removing restPlots ........\033[0m"
	@rm -f restPlots


#------------------------------------------------------------------------------
# end of makefile
