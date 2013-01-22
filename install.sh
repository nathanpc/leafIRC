#!/bin/sh

# Copy the binary and chmod it.
echo "cp leaf /usr/bin"
sudo cp leaf /usr/bin
echo "chmod a+x /usr/bin/leaf"
sudo chmod a+x /usr/bin/leaf

if [ ! -d ~/.leafirc ]; then
	# Make directory and copy files.
	echo "mkdir ~/.leafirc"
	mkdir ~/.leafirc
	echo "cp conf/leaf.conf ~/.leafirc/"
	cp conf/leaf.conf ~/.leafirc/
	echo "cp conf/user.conf ~/.leafirc/"
	cp conf/user.conf ~/.leafirc/
	
	# Force the user to edit the config files.
	echo ""
	read -p "Please edit your general configuration file. [Press any key to continue]"
	nano ~/.leafirc/leaf.conf

	echo ""
	read -p "Please edit your user configuration file. [Press any key to continue]"
	nano ~/.leafirc/user.conf
else
	echo "Configuration files already copied to ~/.leafirc. Nothing to do."
fi