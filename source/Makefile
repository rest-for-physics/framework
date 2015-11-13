#------------------------------------------------------------------------------
#                                                                      
#                                                                      
#   RESTSoft : Software for Rare Event Searches with TPCs
#   Makefile to create the RESTsofts libraries
#    (temporary until unifications of makefiles)
#                                                                      
#   Created  :  June 2014                                             
#   Auth: Igor G. Irastorza                                            
#   Based on old Restsoft makefiles 
#
#------------------------------------------------------------------------------
# generic info on makefiles:
# http://www.gnu.org/software/make/manual/make.html
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Defining the directory structure and some flags.
#------------------------------------------------------------------------------

include .Makefile.arch

# Compiler to be used
CC = g++

#Flags
COPTS = -fPIC -DLINUX
FLAGS = -Wall -std=c++0x

# Relevant directories
PATH_LIB = lib
DIRECTORIES = restcore external events metadata processes viewers


#------------------------------------------------------------------------------
# Defining the compiling targets.
#------------------------------------------------------------------------------
all: Wellcome modules 

#------------------------------------------------------------------------------
# Wellcome message.
#------------------------------------------------------------------------------
Wellcome:
	@echo "\033[40m\033[0;1;32m----------------------------         \033[0m"
	@echo "\033[40m\033[0;1;32mRESTsoft General Makefile         \033[0m"
	@echo "\033[40m\033[0;1;32m----------------------------         \033[0m"

modules:
	@for subdir in $(DIRECTORIES); do \
	echo "Making all in $$subdir"; \
	cd $$subdir && $(MAKE) all; \
	cd $(REST_PATH)/source; \
	done

clean:
	@for subdir in $(DIRECTORIES); do \
	echo "Making clean in $$subdir"; \
	cd $$subdir && $(MAKE) clean; \
	cd $(REST_PATH)/source; \
	done
