# Modify this line to indicate the default version to build

VERSION=full

# Comment this out if you don't have Tcl/Tk on your system

#GUIMODE=-DHAS_GUI


<<<<<<< HEAD
#TKLIBS=-L/usr/lib -ltk8.5 -ltcl8.5
=======
#TKLIBS=-L/usr/lib -ltk -ltcl
>>>>>>> 88c86736547f5ef90cb46ec9534a0259e1303750

# Modify the following line so that gcc can find the tcl.h and tk.h
# header files on your system. Comment this out if you don't have
# Tcl/Tk.

<<<<<<< HEAD
#TKINC=-isystem /usr/include /usr/include/tcl8.5/tk.h /usr/include/tcl8.5/tcl.h
#TKINC=-I/usr/include/tcl8.5/tk.h -I/usr/include/tcl8.5/tcl.h
=======
#TKINC=-isystem /usr/include
>>>>>>> 88c86736547f5ef90cb46ec9534a0259e1303750

# Modify these two lines to choose your compiler and compile time
# flags.

CC=gcc
CFLAGS=-Wall -O2

##################################################
# You shouldn't need to modify anything below here
##################################################

MISCDIR=../misc
HCL2C=$(MISCDIR)/hcl2c
INC=$(TKINC) -I$(MISCDIR) $(GUIMODE)
LIBS=$(TKLIBS) -lm
YAS = ../misc/yas

all: psim drivers

# This rule builds the PIPE simulator
psim: psim.c sim.h pipe-$(VERSION).hcl $(MISCDIR)/isa.c $(MISCDIR)/isa.h
	# Building the pipe-$(VERSION).hcl version of PIPE
	$(HCL2C) -n pipe-$(VERSION).hcl < pipe-$(VERSION).hcl > pipe-$(VERSION).c
	$(CC) $(CFLAGS) $(INC) -o psim psim.c pipe-$(VERSION).c \
		$(MISCDIR)/isa.c $(LIBS)

# This rule builds driver programs for Part C of the Architecture Lab
drivers: 
	./gen-driver.pl -n 4 -f ncopy.ys > sdriver.ys
	../misc/yas sdriver.ys
	./gen-driver.pl -n 63 -f ncopy.ys > ldriver.ys
	../misc/yas ldriver.ys

# These are implicit rules for assembling .yo files from .ys files.
.SUFFIXES: .ys .yo
.ys.yo:
	$(YAS) $*.ys


clean:
	rm -f psim pipe-*.c *.o *.exe *~ 


