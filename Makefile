# 
# Version
#
VERSION=0.1.0b
#
# Source files
#
COMMON_SRCS=Samba.cpp Flash.cpp EfcFlash.cpp EefcFlash.cpp FlashFactory.cpp Applet.cpp WordCopyApplet.cpp
APPLET_SRCS=WordCopyArm.asm
BOSSA_SRCS=BossaForm.cpp BossaWindow.cpp BossaAbout.cpp BossaApp.cpp BossaBitmaps.cpp BossaInfo.cpp BossaThread.cpp BossaProgress.cpp
BOSSA_PNGS=BossaLogo.png BossaIcon.png ShumaTechLogo.png
BOSSAC_SRCS=bossac.cpp CmdOpts.cpp

#
# Windows sources
#
OS:=$(shell uname -s)
ifeq ($(OS),MINGW32_NT-6.1)
EXE=.exe
COMMON_SRCS+=WinSerialPort.cpp WinPortFactory.cpp
COMMON_LDFLAGS=-Wl,--enable-auto-import
COMMON_LIBS=-lsetupapi
BOSSA_RC=BossaRes.rc
endif
ifeq ($(OS),Linux)
COMMON_SRCS+=PosixSerialPort.cpp LinuxPortFactory.cpp
endif

#
# Build directories
#
OBJDIR=obj
SRCDIR=src
RESDIR=res

#
# Object files
#
COMMON_OBJS=$(foreach src,$(COMMON_SRCS),$(OBJDIR)/$(src:%.cpp=%.o))
APPLET_OBJS=$(foreach src,$(APPLET_SRCS),$(OBJDIR)/$(src:%.asm=%.o))
BOSSA_OBJS=$(APPLET_OBJS) $(COMMON_OBJS) $(foreach src,$(BOSSA_SRCS),$(OBJDIR)/$(src:%.cpp=%.o))
ifdef BOSSA_RC
BOSSA_OBJS+=$(OBJDIR)/$(BOSSA_RC:%.rc=%.o)
endif
BOSSAC_OBJS=$(APPLET_OBJS) $(COMMON_OBJS) $(foreach src,$(BOSSAC_SRCS),$(OBJDIR)/$(src:%.cpp=%.o))

#
# Dependencies
#
DEPENDS=$(COMMON_SRCS:%.cpp=$(OBJDIR)/%.d) 
DEPENDS+=$(APPLET_SRCS:%.asm=$(OBJDIR)/%.d) 
DEPENDS+=$(BOSSA_SRCS:%.cpp=$(OBJDIR)/%.d) 
DEPENDS+=$(BOSSAC_SRCS:%.cpp=$(OBJDIR)/%.d) 

#
# Tools
#
#Q=@
CXX=g++
ARM=arm-none-eabi-
ARMAS=$(ARM)as
ARMOBJCOPY=$(ARM)objcopy

#
# CXX Flags
#
COMMON_CXXFLAGS+=-Wall -Werror -MT $@ -MD -MP -MF $(@:%.o=%.d) -DVERSION=\"$(VERSION)\" -g
WX_CXXFLAGS:=$(shell wx-config --cxxflags) -DWX_PRECOMP -pthread -Wno-ctor-dtor-privacy -O2 -fno-strict-aliasing
BOSSA_CXXFLAGS=$(COMMON_CXXFLAGS) $(WX_CXXFLAGS) 
BOSSAC_CXXFLAGS=$(COMMON_CXXFLAGS)

#
# LD Flags
#
COMMON_LDFLAGS+=-g -static-libstdc++ -static-libgcc
BOSSA_LDFLAGS=$(COMMON_LDFLAGS)
BOSSAC_LDFLAGS=$(COMMON_LDFLAGS)

#
# Libs
#
COMMON_LIBS+=
WX_LIBS:=$(shell wx-config --libs)
BOSSA_LIBS=$(COMMON_LIBS) $(WX_LIBS)
BOSSAC_LIBS=$(COMMON_LIBS)

#
# Main targets
#
all: bossa bossac
release: all bossa-release bossac-release

#
# Common rules
#
define common_obj
$(OBJDIR)/$(1:%.cpp=%.o): $(SRCDIR)/$(1)
	@echo CPP $$<
	$$(Q)$$(CXX) $$(COMMON_CXXFLAGS) -c -o $$@ $$<
endef
$(foreach src,$(COMMON_SRCS),$(eval $(call common_obj,$(src))))

#
# Applet rules
#
define applet_obj
$(OBJDIR)/$(1:%.asm=%.o): $(SRCDIR)/$(1)
	@echo APPLET $(1:%.asm=%)
	$$(Q)$$(ARMAS) -o $$(@:%.o=%.obj) $$<
	$$(Q)$$(ARMOBJCOPY) -O binary $$(@:%.o=%.obj) $$(@:%.o=%.bin)
	$$(Q)appletgen $(1:%.asm=%) $(SRCDIR) $(OBJDIR)
	$$(Q)$$(CXX) $$(COMMON_CXXFLAGS) -c -o $$(@) $$(<:%.asm=%.cpp)
endef
$(foreach src,$(APPLET_SRCS),$(eval $(call applet_obj,$(src))))

#
# BOSSA rules
#
define bossa_obj
$(OBJDIR)/$(1:%.cpp=%.o): $(SRCDIR)/$(1)
	@echo CPP $$<
	$$(Q)$$(CXX) $$(BOSSA_CXXFLAGS) -c -o $$@ $$<
endef
$(foreach src,$(BOSSA_SRCS),$(eval $(call bossa_obj,$(src))))

#
# Resource rules
#
$(OBJDIR)/$(BOSSA_RC:%.rc=%.o): $(RESDIR)/$(BOSSA_RC)
	@echo RC $<
	$(Q)`wx-config --rescomp` -o $@ $<

#
# BOSSAC rules
#
define bossac_obj
$(OBJDIR)/$(1:%.cpp=%.o): $(SRCDIR)/$(1)
	@echo CPP $$<
	$$(Q)$$(CXX) $$(BOSSAC_CXXFLAGS) -c -o $$@ $$<
endef
$(foreach src,$(BOSSAC_SRCS),$(eval $(call bossac_obj,$(src))))

#
# PNG rules
#
define bossa_png
$(SRCDIR)/$(1:%.png=%.cpp): $(RESDIR)/$(1)
	@echo BIN2C $$<
	$(Q)bin2c $$< $$@
endef
$(foreach png,$(BOSSA_PNGS),$(eval $(call bossa_png,$(png))))

#
# Directory rules
#
$(OBJDIR):
	@mkdir $@
    
$(BINDIR):
	@mkdir $@

#
# Target rules
#
bossa: $(OBJDIR) $(BINDIR) $(foreach png,$(BOSSA_PNGS),$(SRCDIR)/$(png:%.png=%.cpp)) $(BOSSA_OBJS)
	@echo LD $@
	$(Q)$(CXX) $(BOSSA_LDFLAGS) -o $@ $(BOSSA_OBJS) $(BOSSA_LIBS)

bossa-release: bossa
	@echo STRIP $^
	$(Q)strip $^$(EXE)
	@echo UPX $^
	$(Q)upx $^$(EXE)

bossac: $(OBJDIR) $(BINDIR) $(BOSSAC_OBJS) 
	@echo LD $@
	$(Q)$(CXX) $(BOSSAC_LDFLAGS) -o $@ $(BOSSAC_OBJS) $(BOSSAC_LIBS)

bossac-release: bossac
	@echo STRIP $^
	$(Q)strip $^$(EXE)
	@echo UPX $^
	$(Q)upx $^$(EXE)

clean:
	@echo CLEAN
	$(Q)rm -rf bossa$(EXE) bossac$(EXE) $(OBJDIR)

#
# Include dependencies
#
-include $(DEPENDS)
