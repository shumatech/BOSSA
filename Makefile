.DEFAULT_GOAL := all

# 
# Version
#
VERSION=1.0

#
# Source files
#
COMMON_SRCS=Samba.cpp Flash.cpp EfcFlash.cpp EefcFlash.cpp FlashFactory.cpp Applet.cpp WordCopyApplet.cpp
APPLET_SRCS=WordCopyArm.asm
BOSSA_SRCS=BossaForm.cpp BossaWindow.cpp BossaAbout.cpp BossaApp.cpp BossaBitmaps.cpp BossaInfo.cpp BossaThread.cpp BossaProgress.cpp
BOSSA_BMPS=BossaLogo.bmp BossaIcon.bmp ShumaTechLogo.bmp
BOSSAC_SRCS=bossac.cpp CmdOpts.cpp

#
# Build directories
#
BINDIR=bin
OBJDIR=obj
SRCDIR=src
RESDIR=res

#
# Determine OS
#
OS:=$(shell uname -s)

#
# Windows rules
#
ifeq ($(OS),MINGW32_NT-6.1)
EXE=.exe
COMMON_SRCS+=WinSerialPort.cpp WinPortFactory.cpp
COMMON_LDFLAGS=-Wl,--enable-auto-import -static-libstdc++ -static-libgcc
COMMON_LIBS=-lsetupapi
BOSSA_RC=BossaRes.rc
INSTALLDIR=install
WIXDIR="C:\Program Files (x86)\Windows Installer XML v3.5\bin"

$(OBJDIR)\\bossa-$(VERSION).wixobj: $(INSTALLDIR)\\bossa.wxs
	$(WIXDIR)\\candle.exe -arch x86 -out $@ -ext $(WIXDIR)\\WixUIExtension.dll -ext $(WIXDIR)\\WixDifxAppExtension.dll $<

$(OBJDIR)\\bossa64-$(VERSION).wixobj: $(INSTALLDIR)\\bossa.wxs
	$(WIXDIR)\\candle.exe -arch x64 -out $@ -ext $(WIXDIR)\\WixUIExtension.dll -ext $(WIXDIR)\\WixDifxAppExtension.dll $<

$(BINDIR)\\bossa-$(VERSION).msi: $(OBJDIR)\\bossa-$(VERSION).wixobj $(BINDIR)/bossa$(EXE) $(BINDIR)/bossac$(EXE)
	$(WIXDIR)\\light.exe -cultures:null -out $@ -pdbout $(OBJDIR)\\bossa.wixpdb -sice:ICE57 -ext $(WIXDIR)\\WixUIExtension.dll -ext $(WIXDIR)\\WixDifxAppExtension.dll $(WIXDIR)\\difxapp_x86.wixlib $<

$(BINDIR)\\bossa64-$(VERSION).msi: $(OBJDIR)\\bossa64-$(VERSION).wixobj $(BINDIR)/bossa$(EXE) $(BINDIR)/bossac$(EXE)
	$(WIXDIR)\\light.exe -cultures:null -out $@ -pdbout $(OBJDIR)\\bossa64.wixpdb -sice:ICE57 -ext $(WIXDIR)\\WixUIExtension.dll -ext $(WIXDIR)\\WixDifxAppExtension.dll $(WIXDIR)\\difxapp_x64.wixlib $<

install32: $(BINDIR)\\bossa-$(VERSION).msi
install64: $(BINDIR)\\bossa64-$(VERSION).msi
install: install32 install64

endif

#
# Linux rules
#
ifeq ($(OS),Linux)
COMMON_SRCS+=PosixSerialPort.cpp LinuxPortFactory.cpp
endif

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
WX_CXXFLAGS:=$(shell wx-config --cxxflags) -DWX_PRECOMP -Wno-ctor-dtor-privacy -O2 -fno-strict-aliasing
BOSSA_CXXFLAGS=$(COMMON_CXXFLAGS) $(WX_CXXFLAGS) 
BOSSAC_CXXFLAGS=$(COMMON_CXXFLAGS)

#
# LD Flags
#
COMMON_LDFLAGS+=-g
BOSSA_LDFLAGS=$(COMMON_LDFLAGS)
BOSSAC_LDFLAGS=$(COMMON_LDFLAGS)

#
# Libs
#
COMMON_LIBS+=
WX_LIBS:=$(shell wx-config --libs)
BOSSA_LIBS=-Wl,--as-needed $(COMMON_LIBS) $(WX_LIBS)
BOSSAC_LIBS=$(COMMON_LIBS)

#
# Main targets
#
all: $(OBJDIR) $(BINDIR) $(BINDIR)/bossa$(EXE) $(BINDIR)/bossac$(EXE)

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
ifeq ($(OS),MINGW32_NT-6.1)
$(OBJDIR)/$(BOSSA_RC:%.rc=%.o): $(RESDIR)/$(BOSSA_RC)
	@echo RC $<
	$(Q)`wx-config --rescomp` -o $@ $<
endif

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
# BMP rules
#
define bossa_bmp
$(SRCDIR)/$(1:%.bmp=%.cpp): $(RESDIR)/$(1)
	@echo BIN2C $$<
	$(Q)bin2c $$< $$@
endef
$(foreach bmp,$(BOSSA_BMPS),$(eval $(call bossa_bmp,$(bmp))))

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
$(BINDIR)/bossa$(EXE): $(foreach bmp,$(BOSSA_BMPS),$(SRCDIR)/$(bmp:%.bmp=%.cpp)) $(BOSSA_OBJS)
	@echo LD $@
	$(Q)$(CXX) $(BOSSA_LDFLAGS) -o $@ $(BOSSA_OBJS) $(BOSSA_LIBS)

$(BINDIR)/bossac$(EXE): $(BOSSAC_OBJS) 
	@echo LD $@
	$(Q)$(CXX) $(BOSSAC_LDFLAGS) -o $@ $(BOSSAC_OBJS) $(BOSSAC_LIBS)

pack-bossa: $(BINDIR)/bossa$(EXE)
	@echo UPX $^
	$(Q)upx $^

pack-bossac: $(BINDIR)/bossac$(EXE)
	@echo UPX $^
	$(Q)upx $^

pack: pack-bossa pack-bossac

strip-bossa: $(BINDIR)/bossa$(EXE)
	@echo STRIP $^
	$(Q)strip $^

strip-bossac: $(BINDIR)/bossac$(EXE)
	@echo STRIP $^
	$(Q)strip $^

strip: strip-bossa strip-bossac

clean:
	@echo CLEAN
	$(Q)rm -rf $(BINDIR) $(OBJDIR)

#
# Include dependencies
#
-include $(DEPENDS)
