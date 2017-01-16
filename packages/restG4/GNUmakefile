
name := rdecay01
G4TARGET := $(name)
G4EXLIB := true

ifndef G4INSTALL
  G4INSTALL = ../../../..
endif

CPPFLAGS = `root-config --cflags`  -I $(REST_INSTALL)/include
LDFLAGS = `root-config --libs` -L$(G4LIB) -L $(REST_INSTALL)/lib -lRestCore -lRestMetadata -lRestEvents `root-config --libs` -lGeom 


.PHONY: all
all: lib bin

include $(G4INSTALL)/config/architecture.gmk
include $(G4INSTALL)/config/binmake.gmk

histclean:
	rm ${G4WORKDIR}/tmp/${G4SYSTEM}/${G4TARGET}/HistoManager.o
