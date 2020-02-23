# ============================================================================ #
#                                                                              #
#                         Makefile for YZONE/src                               #
#                                                                              #
# ============================================================================ #
 
YDIR_ROOT = ..
include ../Make_def


# =============================================================================
all : y_zone.a  
	$(MV) y_zone.a $(YDIR_LIB) 
	$(CP) y_zone.h $(YDIR_INC) 
	$(RM) *.o 
# =============================================================================
y_zone.a : y_open.o y_more.o y_wind.o    
	ar -cr y_zone.a  y_open.o y_more.o y_wind.o
# =============================================================================
y_open.o : kernel.c y_zone.h  
	 $(CC) $(OPTS) -o y_open.o -c -DD_OPEN $(INCS) kernel.c
y_more.o : kernel.c y_zone.h 
	 $(CC) $(OPTS) -o y_more.o -c -DD_MORE $(INCS) kernel.c
y_wind.o : kernel.c y_zone.h  
	 $(CC) $(OPTS) -o y_wind.o -c -DD_WIND $(INCS) kernel.c
# =============================================================================
clean :   
	@-(cd $(YDIR_LIB)  && rm *)   
	@-(cd $(YDIR_INC)  && rm *)   
	$(RM)  *.flc *.o *.a *~ 
# =============================================================================
   
