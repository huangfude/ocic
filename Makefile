#This is a oci library that builded by myself
OLIB = liboci.so
CC = gcc
INCLUDE = ./src
OPT = -g -w -fPIC -g3 -gdwarf-2
OCIINCLUDE = $(ORACLE_HOME)/rdbms/public
 
ALLOBJS = mocioper.o
 
all: $(OLIB) clean install
 
clean: $(OLIB)
        -rm -f *.o
 
install:        clean
        -mv -f  $(OLIB) /usr/local/lib/
        -cp     $(INCLUDE)/*.h  /usr/local/include
        -ln -sf /usr/local/lib/$(OLIB) /usr/local/lib/$(OLIB).0
        -ln -sf /usr/local/lib/$(OLIB) /usr/local/lib/$(OLIB).0.1
        -ldconfig
 
$(OLIB): $(ALLOBJS)
        $(CC) -shared -o $(OLIB) $(ALLOBJS) -I$(INCLUDE) $(OPT)
 
mocioper.o:     $(INCLUDE)/mocidef.h $(INCLUDE)/mocioper.h $(INCLUDE)/mocioper.c
        $(CC) -c $(INCLUDE)/mocioper.c -I$(INCLUDE) -I$(OCIINCLUDE) $(OPT)