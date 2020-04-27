ifndef HDF5_HOME
 $(error HDF5_HOME must be set to the location of your cloned and installed hdf5-group repository)
endif

ifeq ($(shell uname),Linux)
 LNK     = -lrt
 OSFLAG  = l
 OSXOPTS =
 RP = -Wl,-rpath=${HDF5_HOME}/lib
else ifeq ($(shell uname),Darwin)
 LNK     =
 OSFLAG  = m
 OSXOPTS = -undefined dynamic_lookup  -mmacosx-version-min=10.13
endif

W_OPTS    = -Wall -Wno-strict-aliasing -Wno-parentheses -Wextra -Werror -Wsign-compare
I_OPTS    = -I${HDF5_HOME}/include -Isrc
L_OPTS    = -L${HDF5_HOME}/lib $(RP) -lhdf5 -lz -lpthread -lssl -shared
OPTS      = -DKXVER=3 -fPIC

MS        = $(shell getconf LONG_BIT)
TGT       = hdf5.so
OBJFILES  = src/kdb_utils.o src/hdf5_utils.o src/hdf5_create.o src/hdf5_general.o src/hdf5_read.o src/hdf5_write.o src/hdf5_ls.o src/hdf5_groups.o src/hdf5_links.o src/hdf5_del.o

QARCH = $(OSFLAG)$(MS)
QLIB  = $(QHOME)/$(QARCH)

all: $(TGT)

$(TGT):src/k.h $(OBJFILES)
	$(CC) $(OBJFILES) $(L_OPTS) $(LNK) -o hdf5.so $(OSXOPTS)

$(OBJFILES):%.o: %.c
	$(CC) -c -m$(MS) $(OPTS) $(I_OPTS) $(W_OPTS) $< -o $@

src/k.h:
	curl -s -L https://github.com/KxSystems/kdb/raw/master/c/c/k.h -o src/k.h

install: hdf5.so
	install hdf5.so $(QLIB)
	install hdf5.q $(QHOME)

clean:
	rm -f src/k.h src/*.o hdf5.so
