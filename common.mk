BBDEBUG ?= 1

OBJDIR      := $(PWD)/obj
INCLUDES    := $(wildcard $(INCDIR)/*.h) \
	       $(wildcard $(INCDIR)/*.inl ) \
	       $(wildcard $(SRCDIR)/*.h ) \
	       $(wildcard $(SRCDIR)/*.inl )
SOURCES_C   := $(wildcard $(SRCDIR)/*.c)
SOURCES_CPP := $(wildcard $(SRCDIR)/*.cpp)
OBJECTS_C   := $(addprefix $(OBJDIR)/,$(subst .c,.o,$(notdir $(SOURCES_C))))
OBJECTS_CPP := $(addprefix $(OBJDIR)/,$(subst .cpp,.o,$(notdir $(SOURCES_CPP))))

.SUFFIXES:
.SUFFIXES: .h .inl .c .cpp .o .a

LCINCS := $(INCDIR)
LCDEFS := -DbbOS=1 -DbbCFGENC=2

ifeq ($(BBDEBUG),1)
  LCDEFS += -DbbDEBUG
  CPPFLAGS += -ggdb
else
  CPPFLAGS += -Os -fomit-frame-pointer
endif

CPPFLAGS += $(LCDEFS) -I$(LCINCS)

default : $(TARGET)

$(PROJECT).a : $(OBJDIR) $(OBJECTS_C) $(OBJECTS_CPP)
	ar cru $@ $(OBJECTS_C) $(OBJECTS_CPP)
	ranlib $@

$(PROJECT).exe : $(OBJDIR) $(OBJECTS_C) $(OBJECTS_CPP)
	$(CXX) -o $(PROJECT).exe $(OBJECTS_C) $(OBJECTS_CPP) -L/usr/local/lib $(LIBS) 

$(PROJECT).dll : $(OBJDIR) $(OBJECTS_C) $(OBJECTS_CPP)
	ar cru $(subst .dll,.a,$@) $(OBJECTS_C) $(OBJECTS_CPP)
	ranlib $(subst .dll,.a,$@)
	dlltool -z $(subst .dll,.def,$@) $(subst .dll,.a,$@)
	g++ -shared -o $@ $(OBJECTS_C) $(OBJECTS_CPP) $(LDFLAGS) $(LIBS) $(subst .dll,.def,$@) -Wl,--out-implib,$(subst .dll,.dll.a,$@)

$(OBJDIR) :
	mkdir -p $@

$(OBJECTS_C) : $(SOURCES_C) $(INCLUDES)
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $(addprefix $(SRCDIR)/,$(subst .o,.c,$(notdir $@))) -o $@

$(OBJECTS_CPP) : $(SOURCES_CPP) $(INCLUDES)
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $(addprefix $(SRCDIR)/,$(subst .o,.cpp,$(notdir $@))) -o $@

clean:
	rm -rf $(TARGET) $(OBJDIR)

