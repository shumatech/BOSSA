.DEFAULT_GOAL := all

#
# Version, maintained only in the 64-bit Release configuration of the MSVC project
#
VERSION?=$(shell sed -n "/<ItemDefinitionGroup.*'Release|x64'/,/<\/ItemDefinitionGroup>/s/.*VERSION=\"\([^\"]*\)\".*/\1/p" Bossa.vcxproj | head -1)
WXVERSION=3.2

#
# Source files
#
COMMON_SRCS=Samba.cpp Flash.cpp D5xNvmFlash.cpp D2xNvmFlash.cpp EfcFlash.cpp EefcFlash.cpp Applet.cpp WordCopyApplet.cpp Flasher.cpp Device.cpp
APPLET_SRCS=WordCopyArm.asm
BOSSA_SRCS=BossaForm.cpp BossaWindow.cpp BossaAbout.cpp BossaApp.cpp BossaBitmaps.cpp BossaInfo.cpp BossaThread.cpp BossaProgress.cpp
BOSSA_BMPS=BossaLogo.bmp BossaIcon.bmp ShumaTechLogo.bmp
BOSSAC_SRCS=bossac.cpp CmdOpts.cpp
BOSSASH_SRCS=bossash.cpp Shell.cpp Command.cpp

#
# Build directories
#
BINDIR=bin
OBJDIR=obj
SRCDIR=src
RESDIR=res
INSTALLDIR=install

#
# Determine OS and machine
# Windows sets OS to Windows_NT in the environment, MSYS2 included, so uname is only needed elsewhere
#
ifneq ($(OS),Windows_NT)
OS:=$(shell uname -s | cut -c -7)
endif
MACHINE?=$(shell uname -m)

#
# Windows rules
#
ifeq ($(OS),Windows_NT)
# Use wxWindows development branch to work around font scaling issues on Windows
# Following line commented out by droftarts, as libwxgtk3.2-dev is the current version 06/11/24
# WXVERSION=3.1
EXE=.exe
COMMON_SRCS+=WinSerialPort.cpp WinPortFactory.cpp
COMMON_LDFLAGS=-Wl,--enable-auto-import -static -static-libstdc++ -static-libgcc
COMMON_LIBS=-ltermcap -Wl,--as-needed -lsetupapi
# The shared wx config defines WXUSINGDLL, whose import symbols a -static link cannot resolve
WX_CONFIG_FLAGS=--static=yes
BOSSA_RC=BossaRes.rc
WIXDIR="C:\Program Files (x86)\WiX Toolset v3.11\bin"
CODE_SIGN=$(INSTALLDIR)\\code_sign.p12
TIMESTAMP=http://timestamp.comodoca.com/authenticode
SIGNTOOL="C:\Program Files (x86)\Windows Kits\10\bin\x64\signtool.exe"
INF2CAT="C:\Program Files (x86)\Windows Kits\10\bin\x86\Inf2Cat.exe"
WINVER=$(shell echo $(VERSION) | cut -d - -f 1)

define bossa_msi
$(OBJDIR)\\bossa-$(1)-$(VERSION).wixobj: $(INSTALLDIR)\\bossa.wxs
	$(WIXDIR)\\candle.exe -dVersion=$(WINVER) -arch $(1) -out $$@ -ext $(WIXDIR)\\WixUIExtension.dll -ext $(WIXDIR)\\WixDifxAppExtension.dll $$<

$(BINDIR)\\bossa-$(1)-$(VERSION).msi: $(OBJDIR)\\bossa-$(1)-$(VERSION).wixobj
	$(WIXDIR)\\light.exe -cultures:null -out $$@ -pdbout $(OBJDIR)\\bossa.wixpdb -sice:ICE57 -ext $(WIXDIR)\\WixUIExtension.dll -ext $(WIXDIR)\\WixDifxAppExtension.dll $(WIXDIR)\\difxapp_$(1).wixlib $$<
	$$(Q)read -p "Password:" -rs PASSWORD; \
	cmd /C '$(SIGNTOOL) sign /v /fd sha256 /f $(CODE_SIGN) -t $(TIMESTAMP) /p '$$$$PASSWORD' $$@'
endef

$(eval $(call bossa_msi,x86))
$(eval $(call bossa_msi,x64))

$(INSTALLDIR)\\bossa.cat: $(INSTALLDIR)\\bossa.inf
	export TMP=$$(mktemp -d); \
	cp $< $$TMP; \
	cmd /C '$(INF2CAT) /v /driver:'$$(cygpath -w $$TMP)' /os:XP_X86,Vista_X86,Vista_X64,7_X86,7_X64,8_X86,8_X64,6_3_X86,6_3_X64,10_x86,10_x64'; \
	mv $$TMP/bossa.cat $@; \
	rm -rf $$TMP; \
	read -p "Password:" -rs PASSWORD; \
	cmd /C '$(SIGNTOOL) sign /v /fd sha256 /f $(CODE_SIGN) -t $(TIMESTAMP) /p '$$PASSWORD' $@'

bossa.cat: $(INSTALLDIR)\\bossa.cat

install32: $(BINDIR)\\bossa-x86-$(VERSION).msi
install64: $(BINDIR)\\bossa-x64-$(VERSION).msi
.PHONY: install
install: strip install32 install64

.PHONY: dist
dist: strip
	cd $(BINDIR) && zip -9 bossa-$(VERSION)-windows-$(MACHINE).zip bossa$(EXE) bossac$(EXE) bossash$(EXE)

endif

#
# Linux rules
#
ifeq ($(OS),Linux)
COMMON_SRCS+=PosixSerialPort.cpp LinuxPortFactory.cpp
COMMON_LIBS=-Wl,--as-needed
COMMON_CXXFLAGS=-std=c++11
WX_LIBS+=-lX11

.PHONY: dist install
dist: strip
	tar cvzf $(BINDIR)/bossa-$(VERSION)-linux-$(MACHINE).tgz -C $(BINDIR) bossa$(EXE) bossac$(EXE) bossash$(EXE)
install: dist
endif

#
# OS X rules
#
ifeq ($(OS),Darwin)
COMMON_SRCS+=PosixSerialPort.cpp OSXPortFactory.cpp
# arm64 only exists from macOS 11 on
ifeq ($(MACHINE),arm64)
MACOSX_MIN=11.0
else
MACOSX_MIN=10.9
endif
COMMON_CXXFLAGS=-arch $(MACHINE) -mmacosx-version-min=$(MACOSX_MIN)
COMMON_LDFLAGS=-arch $(MACHINE) -mmacosx-version-min=$(MACOSX_MIN)
# Homebrew lives in /opt/homebrew on Apple Silicon and /usr/local on Intel, and
# both can be installed side by side, so use the wxWidgets matching the target.
# Homebrew installs the 3.2 tool as wx-config-3.2 to keep it out of the way of wxwidgets 3.3
ifeq ($(MACHINE),arm64)
WX_BREW_PREFIX=/opt/homebrew
else
WX_BREW_PREFIX=/usr/local
endif
WXCONFIG?=$(firstword $(wildcard $(WX_BREW_PREFIX)/bin/wx-config-$(WXVERSION) $(WX_BREW_PREFIX)/bin/wx-config) wx-config)
APP=BOSSA.app
DMG=bossa-$(VERSION).dmg
VOLUME=BOSSA
BACKGROUND=$(INSTALLDIR)/background.png
.PHONY: app install
# The wxWidgets libraries and their dependencies are copied into the bundle so
# that it runs without Homebrew.  Rewriting the library paths invalidates the
# code signatures, so the bundle is signed again (ad hoc) afterwards.
app: strip-bossa
	rm -rf $(BINDIR)/$(APP)
	mkdir -p $(BINDIR)/$(APP)/Contents/MacOS
	mkdir -p $(BINDIR)/$(APP)/Contents/Resources
	cp -f $(INSTALLDIR)/Info.plist $(BINDIR)/$(APP)/Contents
	echo -n "APPL????" > $(BINDIR)/$(APP)/Contents/PkgInfo
	cp -f $(BINDIR)/bossa $(BINDIR)/$(APP)/Contents/MacOS/bossa
	cp -f $(RESDIR)/BossaIcon.icns $(BINDIR)/$(APP)/Contents/Resources
	dylibbundler -od -b -x $(BINDIR)/$(APP)/Contents/MacOS/bossa \
		-d $(BINDIR)/$(APP)/Contents/Frameworks -p @executable_path/../Frameworks/ \
		-s $$($(WXCONFIG) --prefix)/lib
	codesign --force -s - $(BINDIR)/$(APP)/Contents/Frameworks/*
	codesign --force -s - $(BINDIR)/$(APP)
install: strip app
	hdiutil create -ov -megabytes 5 -fs HFS+ -volname $(VOLUME) $(BINDIR)/$(DMG)
	hdiutil attach -noautoopen $(BINDIR)/$(DMG)
	cp -R $(BINDIR)/$(APP) /Volumes/$(VOLUME)/
	cp $(BINDIR)/bossac$(EXE) /Volumes/$(VOLUME)/
	cp $(BINDIR)/bossash$(EXE) /Volumes/$(VOLUME)/
	ln -s /Applications /Volumes/$(VOLUME)/Applications
	ln -s /usr/local/bin /Volumes/$(VOLUME)/bin
	mkdir /Volumes/$(VOLUME)/.background
	cp $(BACKGROUND) /Volumes/$(VOLUME)/.background
	osascript < $(INSTALLDIR)/dmgwin.osa
	hdiutil detach /Volumes/$(VOLUME)/
	hdiutil convert -format UDBZ -o $(BINDIR)/tmp$(DMG) $(BINDIR)/$(DMG)
	mv -f $(BINDIR)/tmp$(DMG) $(BINDIR)/$(DMG)

# Unlike the install DMG this needs no Finder, so it also works on a headless build machine
DIST_DMG=bossa-$(VERSION)-macos-$(MACHINE).dmg
.PHONY: dist
dist: strip app
	rm -rf $(OBJDIR)/dmg
	mkdir -p $(OBJDIR)/dmg
	cp -R $(BINDIR)/$(APP) $(BINDIR)/bossac$(EXE) $(BINDIR)/bossash$(EXE) $(OBJDIR)/dmg/
	ln -s /Applications $(OBJDIR)/dmg/Applications
	hdiutil create -ov -volname $(VOLUME) -srcfolder $(OBJDIR)/dmg -format UDZO $(BINDIR)/$(DIST_DMG)
endif

#
# OpenBSD rules
# (This is likely to work without changes, but not tested, on other BSDs)
#
ifeq ($(OS),OpenBSD)

COMMON_SRCS+=PosixSerialPort.cpp BSDPortFactory.cpp

# This is only needed for bossash, but we can't add it to BOSSASH_LIBS here
# because that one is redefined later.
COMMON_LIBS+=-ltermcap

# As of 5.7, OpenBSD packages WxWidgets 2.8
# bossa builds, runs, and appears to play nicely with this version,
# but fails to do anything useful on systems that don't have hardware
# serial ports because of USB detection problems.
# (The SAM's USB programming port doesn't get recognized as a ucom
# device, and a USB serial adaptor attached to the UART gets detected
# by bossa as a USB interface and doesn't fall back to the serial
# programming protocol.)
WXVERSION=2.8

endif

ifeq (${OS},FreeBSD)

# This is only needed for bossash, but we can't add it to BOSSASH_LIBS here
# because that one is redefined later.
COMMON_SRCS+=PosixSerialPort.cpp BSDPortFactory.cpp

endif

#
# Homebrew installs the 3.2 tool as wx-config-3.2 to keep it out of the way of
# wxwidgets 3.3.  macOS chooses its own above.
#
WXCONFIG?=wx-config

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
BOSSASH_OBJS=$(APPLET_OBJS) $(COMMON_OBJS) $(foreach src,$(BOSSASH_SRCS),$(OBJDIR)/$(src:%.cpp=%.o))

#
# Dependencies
#
DEPENDS=$(COMMON_SRCS:%.cpp=$(OBJDIR)/%.d)
DEPENDS+=$(APPLET_SRCS:%.asm=$(OBJDIR)/%.d)
DEPENDS+=$(BOSSA_SRCS:%.cpp=$(OBJDIR)/%.d)
DEPENDS+=$(BOSSAC_SRCS:%.cpp=$(OBJDIR)/%.d)
DEPENDS+=$(BOSSASH_SRCS:%.cpp=$(OBJDIR)/%.d)

#
# Tools
#
Q?=@
CXX?=g++
ARM=arm-none-eabi-
ARMAS=$(ARM)as
ARMOBJCOPY=$(ARM)objcopy

#
# CXX Flags
#
COMMON_CXXFLAGS+=-Wall -Werror -MT $@ -MD -MP -MF $(@:%.o=%.d) -DVERSION=\"$(VERSION)\" -g -O2 $(CXXFLAGS)
WX_CXXFLAGS:=$(shell $(WXCONFIG) $(WX_CONFIG_FLAGS) --cxxflags --version=$(WXVERSION)) -DWX_PRECOMP -Wno-ctor-dtor-privacy -O2 -fno-strict-aliasing
BOSSA_CXXFLAGS=$(COMMON_CXXFLAGS) $(WX_CXXFLAGS)
BOSSAC_CXXFLAGS=$(COMMON_CXXFLAGS)
BOSSASH_CXXFLAGS=$(COMMON_CXXFLAGS)

#
# LD Flags
#
COMMON_LDFLAGS+=-g $(LDFLAGS)
BOSSA_LDFLAGS=$(COMMON_LDFLAGS)
BOSSAC_LDFLAGS=$(COMMON_LDFLAGS)
BOSSASH_LDFLAGS=$(COMMON_LDFLAGS)

#
# Libs
#
COMMON_LIBS+=
WX_LIBS:=$(shell $(WXCONFIG) $(WX_CONFIG_FLAGS) --libs --version=$(WXVERSION)) $(WX_LIBS)
BOSSA_LIBS=$(COMMON_LIBS) $(WX_LIBS)
BOSSAC_LIBS=$(COMMON_LIBS)
BOSSASH_LIBS=-lreadline $(COMMON_LIBS)

#
# Main targets
#
all: $(BINDIR)/bossa$(EXE) $(BINDIR)/bossac$(EXE) $(BINDIR)/bossash$(EXE)
bossac: $(BINDIR)/bossac$(EXE)

.PHONY: version
version:
	@echo $(VERSION)

#
# Common rules
#
define common_obj
$(OBJDIR)/$(1:%.cpp=%.o): $(SRCDIR)/$(1)
	@echo CPP COMMON $$<
	$$(Q)$$(CXX) $$(COMMON_CXXFLAGS) -c -o $$@ $$<
endef
$(foreach src,$(COMMON_SRCS),$(eval $(call common_obj,$(src))))

#
# Applet rules
#
define applet_obj
$(SRCDIR)/$(1:%.asm=%.cpp): $(SRCDIR)/$(1)
	@echo APPLET $(1:%.asm=%)
	$$(Q)$$(ARMAS) -o $$(@:$(SRCDIR)/%.cpp=$(OBJDIR)/%.obj) $$<
	$$(Q)$$(ARMOBJCOPY) -O binary $$(@:$(SRCDIR)/%.cpp=$(OBJDIR)/%.obj) $$(@:$(SRCDIR)/%.cpp=$(OBJDIR)/%.bin)
	$$(Q)./appletgen $(1:%.asm=%) $(SRCDIR) $(OBJDIR)
$(OBJDIR)/$(1:%.asm=%.o): $(SRCDIR)/$(1:%.asm=%.cpp)
	@echo CPP APPLET $$<
	$$(Q)$$(CXX) $$(COMMON_CXXFLAGS) -c -o $$(@) $$(<:%.asm=%.cpp)
endef
$(foreach src,$(APPLET_SRCS),$(eval $(call applet_obj,$(src))))

#
# BOSSA rules
#
define bossa_obj
$(OBJDIR)/$(1:%.cpp=%.o): $(SRCDIR)/$(1)
	@echo CPP BOSSA $$<
	$$(Q)$$(CXX) $$(BOSSA_CXXFLAGS) -c -o $$@ $$<
endef
$(foreach src,$(BOSSA_SRCS),$(eval $(call bossa_obj,$(src))))

#
# Resource rules
#
ifeq ($(OS),Windows_NT)
$(OBJDIR)/$(BOSSA_RC:%.rc=%.o): $(RESDIR)/$(BOSSA_RC)
	@echo RC $<
	$(Q)`$(WXCONFIG) $(WX_CONFIG_FLAGS) --rescomp --version=$(WXVERSION)` -o $@ $<
endif

#
# BOSSAC rules
#
define bossac_obj
$(OBJDIR)/$(1:%.cpp=%.o): $(SRCDIR)/$(1)
	@echo CPP BOSSAC $$<
	$$(Q)$$(CXX) $$(BOSSAC_CXXFLAGS) -c -o $$@ $$<
endef
$(foreach src,$(BOSSAC_SRCS),$(eval $(call bossac_obj,$(src))))

#
# BOSSASH rules
#
define bossash_obj
$(OBJDIR)/$(1:%.cpp=%.o): $(SRCDIR)/$(1)
	@echo CPP BOSSASH $$<
	$$(Q)$$(CXX) $$(BOSSASH_CXXFLAGS) -c -o $$@ $$<
endef
$(foreach src,$(BOSSASH_SRCS),$(eval $(call bossash_obj,$(src))))

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
$(BOSSA_OBJS): | $(OBJDIR)
$(BINDIR)/bossa$(EXE): $(foreach bmp,$(BOSSA_BMPS),$(SRCDIR)/$(bmp:%.bmp=%.cpp)) $(BOSSA_OBJS) | $(BINDIR)
	@echo LD $@
	$(Q)$(CXX) $(BOSSA_LDFLAGS) -o $@ $(BOSSA_OBJS) $(BOSSA_LIBS)

$(BOSSAC_OBJS): | $(OBJDIR)
$(BINDIR)/bossac$(EXE): $(BOSSAC_OBJS) | $(BINDIR)
	@echo LD $@
	$(Q)$(CXX) $(BOSSAC_LDFLAGS) -o $@ $(BOSSAC_OBJS) $(BOSSAC_LIBS)

$(BOSSASH_OBJS): | $(OBJDIR)
$(BINDIR)/bossash$(EXE): $(BOSSASH_OBJS) | $(BINDIR)
	@echo LD $@
	$(Q)$(CXX) $(BOSSASH_LDFLAGS) -o $@ $(BOSSASH_OBJS) $(BOSSASH_LIBS)

strip-bossa: $(BINDIR)/bossa$(EXE)
	@echo STRIP $^
	$(Q)strip $^

strip-bossac: $(BINDIR)/bossac$(EXE)
	@echo STRIP $^
	$(Q)strip $^

strip-bossash: $(BINDIR)/bossash$(EXE)
	@echo STRIP $^
	$(Q)strip $^

strip: strip-bossa strip-bossac strip-bossash

clean:
	@echo CLEAN
	$(Q)rm -rf $(BINDIR) $(OBJDIR)

#
# Include dependencies
#
-include $(DEPENDS)
