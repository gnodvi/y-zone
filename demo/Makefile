# ============================================================================#
#                                                                             #
#                      Makefile for  YZONE/demo                               #
#                                                                             #
# ============================================================================#


YDIR_ROOT = ..
include ../Make_def

PROG = dial_1 dial_2

#DEMO_C = $(YDIR_ROOT)/demo/demons.c
DEMO_C = demons.c

# =============================================================================

local : $(PROG) 

# =============================================================================

all : 
	@-(cd $(DIR_ROOT) &&  make)

# =============================================================================

dial_1 : $(DEMO_C) 
	$(CC) $(OPTS) -o dial_1 -DD_DIAL_1 $(DEMO_C) -I../src  -I/usr/X11R6/include  -L/usr/X11R6/lib ../src/y_zone.a -lX11

dial_2 : $(DEMO_C) 
	$(CC) $(OPTS) -o dial_2 -DD_DIAL_2 $(DEMO_C) -I../src  -I/usr/X11R6/include  -L/usr/X11R6/lib ../src/y_zone.a -lX11 

# =============================================================================

clean :   
	$(RM) $(PROG) *.flc *.o core *~ 

# =============================================================================
   
