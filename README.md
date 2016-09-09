BOSSA 1.7.0

BOSSA is a flash programming utility for Atmel's SAM family of flash-based ARM microcontrollers.
The motivation behind BOSSA is to create a simple, easy-to-use, open source utility to replace Atmel's SAM-BA software.
BOSSA is an acronym for Basic Open Source SAM-BA Application to reflect that goal.

The software has been originally created by Scott Shumate and improved over time by Arduino LLC and many
other [contributors](https://github.com/shumatech/BOSSA/graphs/contributors).

the software is released under the terms of the BSD license as specificed in the LICENSE file.

How To Compile on Linux (Ubuntu)
--------------------------------
Make sure you have the 'build-essentials' package installed.
```
sudo apt-get install build-essentials
```

You will also need libreadline-dev, and the wxWidgets libraries.
```
sudo apt-get install libreadline-dev wx2.8-headers libwxgtk2.8-0 libwxgtk2.8-dev
```

Now, from the top most directory of the BOSSA checkout, run the following in your bash terminal.
```
# Compile the 'bossa' CLI source into the bin folder, use up to four jobs while compiling
make bin/bossac -j4
```

```
# Compile the 'bossa' shell source into the bin folder, use up to four jobs while compiling
make bin/bossash -j4
```
