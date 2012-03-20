# Tools

CPP = /usr/bin/g++
RM = /bin/rm

# Setup

CFLAGS = -g -Wall -fPIC

LIB_SGP4_DIR  = Libraries/sgp4/libsgp4
LIB_SHP_DIR   = Libraries/shapelib

INCLUDES = -I $(LIB_SGP4_DIR) -I $(LIB_SHP_DIR)

LIB_SGP4 = $(LIB_SGP4_DIR)/libsgp4.a
LIB_SHP  = $(LIB_SHP_DIR)/libshp.a

LIB_GTG_DIR = Source
LIB_GTG     = $(LIB_GTG_DIR)/libgtg.a

.PHONY: libs libsgp4 libshp $(LIB_SGP4_DIR) $(LIB_SHP_DIR) clean-libs clean-sgp4 clean-shp clean clean-all libgtg

# Ground Track Generator

gtg: $(LIB_SGP4) $(LIB_SHP) $(LIB_GTG)
	$(CPP) $(LIB_SGP4) $(LIB_SHP) $(LIB_GTG) -o $@

# Program Library

$(LIB_GTG): libgtg

libgtg:
	@echo "# Making libgtg..."
	@$(MAKE) --directory=$(LIB_GTG_DIR) lib
	
# Prerequisite Libraries

libs: libsgp4 libshp

$(LIB_SGP4): libsgp4

$(LIB_SHP): libshp

libsgp4: $(LIB_SGP4_DIR)

libshp: $(LIB_SHP_DIR)

$(LIB_SGP4_DIR):
	@echo "# Making libsgp4..."
	@$(MAKE) --directory=$(LIB_SGP4_DIR) lib

$(LIB_SHP_DIR):
	@echo "# Making libshp..."
	@$(MAKE) --directory=$(LIB_SHP_DIR) lib

# Clean

clean:
	$(RM) -f gtg

clean-all: clean clean-libs

clean-libs: clean-sgp4 clean-shp

clean-sgp4:
	$(MAKE) --directory=$(LIB_SGP4_DIR) clean

clean-shp:
	$(MAKE) --directory=$(LIB_SHP_DIR) clean
