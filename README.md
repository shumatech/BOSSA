BOSSA 1.4 for Arduino

This version of BOSSA is a fork of the original project and contains some
patches specific for the Arduino products that are unlike to be accepted
upstream.

The original software is developed by Scott Shumate and can be found here:

http://www.shumatech.com/web/products/bossa

BOSSA 1.5-arduino
-----------------

* Added Codelite project for easier development and debug.
* Improved support for SAMD21 chip by using existing applet already in use for other devices.
  * Added time count for operations erase, write, read and verify.
  * Added Devices.h for global definitions for each devices.
  * NVMFlash::write() rewritten.
  * Removed some useless functions from 1.4 release.
  * Added a mask for SAMD chipid: DIE and REV bitfields are not taken into account as they may vary in the time without impacting the features.
  * Improved write time performance by a factor of 2x


BOSSA 1.4-arduino
-----------------

* Added support for SAMD21 chip


BOSSA 1.2
---------

FILES
bossa-1.2.msi -- Windows 2000+
bossa64-1.2.msi -- Windows 2000+ 64-bit
bossa-i686-1.2.tgz -- Linux GTK
bossa-x86_64-1.2.tgz -- Linux GTK 64-bit
bossa-1.2.dmg -- MAC OS X 10.6+

NEW IN THIS RELEASE
* New BOSSA shell command line application to do basic memory, flash, and PIO diagnostics
* Workaround for SAM3U firmware bug
* Fixed a bug with setting boot to flash bit on SAM3 devices

RELEASE NOTES
* The OS X USB driver detects an Atmel device as a USB modem.  When prompted about a new network interface, click Cancel to continue.
* Some stability issues have been seen with the OS X USB driver using BOSSA.  When running BOSSA a second time to the same Atmel device, the USB driver can lock up causing BOSSA to freeze.  As a workaround, always disconnect and reconnect the Atmel device before running BOSSA again.
* The firmware inside of SAM3U devices has a bug where non-word flash reads return zero instead of the real data.  BOSSA implements a transparent workaround for flash operations that copies flash to SRAM before reading.  Direct reads using the BOSSA shell will see the bug.
* There are reports that the USB controller in some AMD-based systems has difficulty communicating with SAM devices.  The only known workaround is to use a different, preferrably Intel-based, system.
