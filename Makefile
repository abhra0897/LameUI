#####################################################################
# Created by Avra Mitra                                             #
# First created on Apr 30, 2020                                     #
# Last modified on Oct 27, 2023                                     #
#                                                                   #
# This Makefile creates static lib for LameUI (liblameui.a) and     #
# also creates documentation using Doxygen. Created docs is stored  #
# in `docs/html/` dir.                                              #
# -------------------------------------------------------------     #
#    command        purpose                                         #
# -------------------------------------------------------------     #
# 1. make           Creates the liblameui.a library in lib/ dir     #
# 2. make docs      Creates html docs in docs/html/ dir             #
# 3. make clean     Cleans the library and the html docs            #
#####################################################################

# Project name
PROJ_NAME=LameUI
# Output library name
TARGET=liblameui.a


######################################################################
#                         SETUP SOURCES                              #
######################################################################

#------------ SOURCE DIRECTORIES -----------------------------------
# User source directory
SRCDIR	 =  ./
# Objects directory
OBJDIR		 = ./
# Output static library directory
TARGDIR		 =  ./
# Output docs html path
DOCHTMLDIR  = docs/html/

#--------------------------------------------------------------------

#------------ C/CPP SOURCE FILES ------------------------------------
SRCS     =  lame_ui.c
#--------------------------------------------------------------------

#------------ COMPILED OBJECT FILES ---------------------------------
# Object file names (source_name.o)
OBJ			 =  $(SRCS:.c=.o)	
#--------------------------------------------------------------------

#------------ INCLUDE DIRECTORIES -----------------------------------
# Includes (headers)
INCLS		 =  -I.
#--------------------------------------------------------------------


#------------ DON'T EDIT BELOW PART ---------------------------------
# Add the directory with the file names
SRCS_MAIN	 =  $(addprefix $(SRCDIR),$(SRCS))
OBJ_MAIN	 =  $(addprefix $(OBJDIR),$(OBJ))
TARGET_MAIN	 =  $(addprefix $(TARGDIR),$(TARGET))
#--------------------------------------------------------------------

######################################################################
#                         SETUP TOOLS                                #
######################################################################

# Cross compiler and archiver information
CC			 =  gcc
AR			 =  ar

# Compiler flags used to create .o files from .c/cpp files
# CFLAGS  	 =   -g
CFLAGS  	+=  -O0
CFLAGS  	+=  -Wall -Wextra -Warray-bounds

# Archiver flag
ARFLAGS		 =  rvs

DOXY_VER := $(shell doxygen --version | sed -n 's/^\([0-9]\+\(\.[0-9]\+\)*\).*/\1/p')

######################################################################
#                         SETUP TARGETS                              #
######################################################################

# Rule to make all
.PHONY: all
all: $(TARGET_MAIN)


# Rule to clean
.PHONY: clean
clean: 
	@echo [Cleaning...]
	rm -rf $(OBJ_MAIN) $(TARGET_MAIN) $(DOCHTMLDIR)

# Rule to make docs
.PHONY: docs
docs:
	DOXY_VER=$(shell doxygen --version | sed -n 's/^\([0-9]\+\(\.[0-9]\+\)*\).*/\1/p')
ifneq "$(DOXY_VER)" "1.9.6"
	@echo Warning: Doxygen version 1.9.6 is tested. But found $(DOXY_VER)! Still proceed? [Y/n]
	@read line; if [ $$line = "n" ]; then echo aborting; exit 1 ; fi
endif
	@echo [Building LameUI docs...]
	doxygen docs/Doxyfile.in

# Rule to make object file by compiling source file
$(OBJDIR)%.o: $(SRCDIR)%.c
	@echo [Compiling...]
	$(CC) -c -o $@ $< $(INCLS) $(CFLAGS)

# Rule to make a static library archive (.a) from object (.o) file
$(TARGET_MAIN): $(OBJ_MAIN)
	@echo [Archiving...]
	$(AR) $(ARFLAGS) $(TARGET_MAIN) $(OBJ_MAIN)
	rm $(OBJ_MAIN)
