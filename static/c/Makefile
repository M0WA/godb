TARGET      := libDB

SRCDIR      := src
INCDIR      := include
BUILDDIR    := obj
TARGETDIR   := bin
SRCEXT      := c
DEPEXT      := d
OBJEXT      := o

SOURCES     := $(wildcard $(SRCDIR)/*.c)
OBJECTS     := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.$(OBJEXT)))

include settings.mk

ifeq ($(DISABLE_POSTGRES),1)
ifeq ($(DISABLE_MYSQL),1)
ifeq ($(DISABLE_DBI),1)
$(error no database type selected)
endif
endif
endif

include tests.mk

all: directories
	$(MAKE) $(TARGET).so
	$(MAKE) $(TARGET).a

remake: cleaner all

directories:
	if [ ! -d $(TARGETDIR) ]; then mkdir -p $(TARGETDIR); fi
	if [ ! -d $(TARGETDIR) ]; then mkdir -p $(BUILDDIR); fi

clean:
	@$(RM) -rf $(BUILDDIR)

cleaner: clean
	@$(RM) -rf $(TARGETDIR)


#Pull in dependency info for *existing* .o files
-include $(OBJECTS:.$(OBJEXT)=.$(DEPEXT))

#Link shared object
$(TARGET).so: $(OBJECTS)
	$(CC) -shared -o $(TARGETDIR)/$(TARGET).so $^ $(LIB)

#Link static lib
$(TARGET).a: $(OBJECTS)
	ar rs $(TARGETDIR)/$(TARGET).a $^

#Compile
$(BUILDDIR)/%.$(OBJEXT): $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<
	@$(CC) $(CFLAGS) $(INCDEP) -MM $(SRCDIR)/$*.$(SRCEXT) > $(BUILDDIR)/$*.$(DEPEXT)
	@cp -f $(BUILDDIR)/$*.$(DEPEXT) $(BUILDDIR)/$*.$(DEPEXT).tmp
	@sed -e 's|.*:|$(BUILDDIR)/$*.$(OBJEXT):|' < $(BUILDDIR)/$*.$(DEPEXT).tmp > $(BUILDDIR)/$*.$(DEPEXT)
	@sed -e 's/.*://' -e 's/\\$$//' < $(BUILDDIR)/$*.$(DEPEXT).tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> $(BUILDDIR)/$*.$(DEPEXT)
	@rm -f $(BUILDDIR)/$*.$(DEPEXT).tmp

.PHONY: all remake clean cleaner