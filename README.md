# Raspberry Bare Metal

## License

Copyright (C) 2014  Federico "MrModd" Cosentino (http://mrmodd.it/)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

## Compiling notes

There's a Makefile in the top level folder that compiles every project.
Before using *make* command you must export **CROSS_COMPILE** environment
variable.
CROSS_COMPILE variable should contain just the prefix of the cross compiler,
such as "arm-linux-gnueabi-".
Follow instructions on http://elinux.org/RPi_U-Boot for a detailed explanation.

From project *08-uart* it is possible to use **UART0** as well as **UART1**. Compile with
"make" to compile for *UART0* or "make mini_uart" to use *UART1*.

## Preparing SD Card

**get-boot-files.sh** is all you need to use to download GPU firmware files,
official Raspberry cross compiler and u-boot. Just run the script and
wait until all files are downloaded and u-boot get compiled. If no error
occurred you will find a new **sd-card** folder with all and just the files
you need to put in the SD Card in order to run u-boot.
This script download u-boot git, if already present it tries to pull new
commits. If no *CROSS_COMPILE* variable is set, it does the same with the
compiler git.
Note that you can use the downloaded repository as cross compiler for the
projects too!

## Launching programs

If you have u-boot on your SD Card, you can load and run the programs directly
from the serial interface. Read u-boot documentation on how to load files
and use kermit, xmodem or similar to upload the .bin files.
Remember to upload the program at the same address specified as memory start
in .lds file.
