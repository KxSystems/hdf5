OSFLAG :=
MS :=

# Additions of the root information and 'more intelligent' library finding mechanisms can be added moving forward
KAFKA_ROOT     = ${HOME}
KFK_INCLUDE    = ${KAFKA_ROOT}/include
W_OPTS         := #-Wall -Wno-strict-aliasing -Wno-parentheses -Wextra -Werror -Wsign-compare
OPTS           = -DKXVER=3 -shared -fPIC $(W_OPTS)
LD_COMMON      = -lz -lpthread -lssl -g -O2
LDOPTS_DYNAMIC = -L/usr/lib/ -lhdf5
MS             = $(shell getconf LONG_BIT)
TGT            = hdf5.so

ifeq ($(shell uname),Linux)
 LNK     = -lrt
 OSFLAG  = l
 OSXOPTS:=
else ifeq ($(shell uname),Darwin)
 OSFLAG  = m
 LNK:=
 OSXOPTS = -undefined dynamic_lookup  -mmacosx-version-min=10.12
endif

QARCH = $(OSFLAG)$(MS)
Q     = $(QHOME)/$(QARCH)

all: headers/k.h headers/hdf5.h
	$(CC) hdf5.c -m$(MS) $(OPTS) $(LDOPTS_DYNAMIC) $(LD_COMMON) $(LNK) -o $(TGT) $(OSXOPTS)

install:
	mkdir -p $(QARCH)
	install  $(TGT) $(Q)
	install  $(TGT) $(QARCH)

clean:
	rm -f hdf5.so
	rm -f $(QARCH)/hdf5.so
