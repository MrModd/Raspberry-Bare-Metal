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
