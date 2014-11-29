#!/bin/bash

cd $( dirname "${BASH_SOURCE[0]}" )

echo -n "Looking for git... "
if [ ! `command -v git 2>/dev/null` ] ; then
	echo -e "FAILED!\n\nPlease install git and retry."
	exit 1
fi
echo "OK!"

# Get cross compiler #

echo "Getting cross compiler..."

NEW=""

if [ ! $CROSS_COMPILER ] ; then
	if [ ! -d rpi-tools/ ] ; then
		git clone git://github.com/raspberrypi/tools rpi-tools
		if [ $? != "0" ] ; then
			echo -e "Cannot clone repository" >&2
			exit 1
		fi
		
		NEW="true"
	else
		cd rpi-tools
		git pull
		if [ $? != "0" ] ; then
			echo -e "Cannot pull repository" >&2
			exit 1
		fi
		
		cd ..
	fi

	#export CROSS_COMPILE=$(pwd)/rpi-tools/arm-bcm2708/arm-bcm2708hardfp-linux-gnueabi/bin/arm-bcm2708hardfp-linux-gnueabi-
	export CROSS_COMPILE=$(pwd)/rpi-tools/arm-bcm2708/arm-bcm2708-linux-gnueabi/bin/arm-bcm2708-linux-gnueabi-
	#export CROSS_COMPILE=$(pwd)/rpi-tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian/arm-linux-gnueabihf/bin/
fi

# Get U-Boot #

echo "Getting uboot..."

NEW=""

if [ ! -d u-boot/ ] ; then
	git clone git://git.denx.de/u-boot.git
	if [ $? != "0" ] ; then
		echo -e "Cannot clone repository" >&2
		exit 1
	fi
	
	NEW="true"
fi

cd u-boot/

if [ ! $NEW ] ; then
	git pull
	if [ $? != "0" ] ; then
		echo -e "Cannot pull repository" >&2
		exit 1
	fi
fi

make distclean
make rpi_b_defconfig
if [ $? != "0" ] ; then
	echo -e "Error configuring uboot" >&2
	exit 1
fi

make -j$(grep -c ^processor /proc/cpuinfo) -s
if [ $? != "0" ] ; then
	echo -e "Error compiling uboot" >&2
	exit 1
fi

cd ..

# Copy required files into a directory #

echo "Copying files into folder..."

mkdir -p sd-card/
if [ $? != "0" ] ; then
	echo -e "Cannot create output directory" >&2
	exit 1
fi

cd sd-card/
cp ../u-boot/u-boot.bin ./kernel.img
wget https://github.com/raspberrypi/firmware/raw/master/boot/bootcode.bin -O bootcode.bin
wget https://github.com/raspberrypi/firmware/raw/master/boot/fixup.dat -O fixup.dat
wget https://github.com/raspberrypi/firmware/raw/master/boot/fixup_cd.dat -O fixup_cd.dat
wget https://github.com/raspberrypi/firmware/raw/master/boot/fixup_x.dat -O fixup_x.dat
wget https://github.com/raspberrypi/firmware/raw/master/boot/start.elf -O start.elf
wget https://github.com/raspberrypi/firmware/raw/master/boot/start_cd.elf -O start_cd.elf
wget https://github.com/raspberrypi/firmware/raw/master/boot/start_x.elf -O start_x.elf

echo -e "\n\nDONE!"
