#!/bin/sh

# Copy the binary and chmod it.
echo "install leaf $1"
sudo install ./leaf $1
echo "chmod a+x $1/leaf"
sudo chmod a+x $1/leaf

if [ ! -d ~/.leafirc ]; then
	# Make directory and copy files.
	echo "mkdir ~/.leafirc"
	mkdir ~/.leafirc
	echo "install -v conf/leaf.conf ~/.leafirc"
	install -v conf/leaf.conf ~/.leafirc
	echo "install -v conf/user.conf ~/.leafirc"
	install -v conf/user.conf ~/.leafirc
	
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