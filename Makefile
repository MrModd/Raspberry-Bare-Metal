##########################################################################
# Raspberry Bare Metal
# Copyright (C) 2014  Federico "MrModd" Cosentino (http://mrmodd.it/)
# 
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
##########################################################################

# Select all subdirectory
SUBDIR = $(shell find . -mindepth 1 -maxdepth 1 -type d \
			-not -path ./.git -not -path ./rpi-tools \
			-not -path ./u-boot -not -path ./sd-card)

# Compile every sudbirectory
all:
	for dir in $(SUBDIR) ; do \
		$(MAKE) -C $$dir ; \
	done

# Compile tags in every subdirectory
tags:
	for dir in $(SUBDIR) ; do \
		$(MAKE) -C $$dir tags ; \
	done

clean:
	for dir in $(SUBDIR) ; do \
		$(MAKE) -C $$dir clean ; \
	done

.PHONY: all tags
