TOPTARGETS := all clean

SUBDIRS := $(wildcard */.)

$(TOPTARGETS): $(SUBDIRS)
$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)

.PHONY: $(TOPTARGETS) $(SUBDIRS)

all: str
	ar rcs libfiesta.a str/str.o 

str:
	cd str && $(MAKE) $(MAKECMDGOALS)

clean:
	$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)
