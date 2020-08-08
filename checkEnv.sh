#!/bin/sh

# Prepare a debian or centos environment for deployment of the mysql_api checker

command_exists() {
	type "$1" &> /dev/null;
}

install_not_exists() {
	if [ $1 = "Fedora" ]; then
		if command_exists dnf; then
			dnf install $2 -y
		elif command_exists yum; then
			yum install $2 -y
		else
			echo "Unable to install" $2
		fi
	fi

	if [ $1 = "Debian" ]; then
		if command_exists apt; then
			apt install $2 -y
		else
			echo "Unable to install " $2
		fi
	fi
}

check_os() {
	if [ $1 = 0 ]; then
		systemOs="Fedora"
	else
		systemOs="Debian"
	fi
}

os=`cat /etc/os-release | grep -e "^ID_LIKE="| cut -d = -f2 | sed 's/"//g'`
testFed=`echo $os | grep -c "rhel"`
testDeb=`echo $os | grep -c "debian"`

#	0 is used to identify fedora based systems
#	1 is used to identify debian based systems

if [ $testFed = 1 ]; then
	check_os 0
elif [ $testDeb = 1 ]; then
	check_os 1
else
	echo "unknown os-type, quitting program"
	exit
fi

#	Check for GCC
echo "Installing gcc"
install_not_exists $systemOs gcc

#	Check for mysql-server
echo "installing mysql-server"
install_not_exists $systemOs mysql-server

#	Independent of install_not_exists() function, because
#	the package names are different for mysql_devel options
#	Negates running un-necessary code as much as possible
echo "Installing mysql developer packages, for use in the C api"

if command_exists apt; then
	apt install default-libmysqlclient-dev -y
	exit
fi
#	Exit command is there because on centos 8 (and possibly other versions of RHEL), 
#	Both yum and dnf package managers are present. Saves a bit of time, in this instance.
if command_exists yum; then
	yum install mysql-devel -y
	exit
fi

if command_exists dnf; then
	dnf install mysql-devel -y
fi

