ifndef HDF5_HOME
 $(error HDF5_HOME must be set to the location of your cloned and installed hdf5-group repository)
endif

HDF5_INCLUDE   = ${HDF5_HOME}/include
HDF5_LIB       = ${HDF5_HOME}/lib

I_OPTS         = -I${HDF5_INCLUDE}
W_OPTS         = -Wall -Wno-strict-aliasing -Wno-parentheses -Wextra -Werror -Wsign-compare
L_OPTS         = -L${HDF5_LIB} -lhdf5 -lz -lpthread -lssl -g -O2
OPTS           = -DKXVER=3 -shared -fPIC

MS             = $(shell getconf LONG_BIT)
TGT            = hdf5.so

ifeq ($(shell uname),Linux)
 LNK     = -lrt
 OSFLAG  = l
 OSXOPTS =
else ifeq ($(shell uname),Darwin)
 LNK     =
 OSFLAG  = m
 OSXOPTS = -undefined dynamic_lookup  -mmacosx-version-min=10.12
endif

QARCH = $(OSFLAG)$(MS)
Q     = $(QHOME)/$(QARCH)

all: src/k.h
	$(CC) hdf5.c -m$(MS) $(OPTS) $(I_OPTS) $(W_OPTS) $(L_OPTS) $(LNK) -o $(TGT) $(OSXOPTS)

src/k.h:
	curl -s -L https://github.com/KxSystems/kdb/raw/master/c/c/k.h -o src/k.h

install:
	mkdir -p $(QARCH)
	install  $(TGT) $(Q)
	install  $(TGT) $(QARCH)

clean:
	rm -f hdf5.so
	rm -f $(QARCH)/hdf5.so
