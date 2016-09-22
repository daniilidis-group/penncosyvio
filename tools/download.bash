#!/bin/bash
#-------------------------------------------------------------------------------
#
# 2016 Bernd Pfrommer
# UPenn GRASP lab
#
# Download script for the PennCOSYVIO data set
#

#
#
# -------------------- functions ---------------------------------
#
check_for_command() {
    if hash $1 2>/dev/null; then
	return 0
    fi
    return -1
}

download() {
    echo "------------------------------------------"
    topdir=../data
    outdir=${topdir}/$1
    mkdir -p $topdir
    tarfname=${1}.tar
    targname=${topdir}/$tarfname
    if [[ "$1" != "tango_top" ]]; then
	echo "ERROR: sorry, only Tango Top dataset is online so far!"
	return
    fi

    if [[ -d "$outdir" ]]; then
	echo
	echo "ERROR: output directory $outdir already exists, remove first!"
	echo
	return
    fi
    echo "downloading " $2 " please be patient "
    wget -O $targname "https://www.dropbox.com/s/1ah9tn3gg5uiphs/tango_top.tar?dl=0"
    if [[ $? -eq 0 ]]; then
	echo "download successful!"
    else
	echo "download failed!"
	return
    fi
    echo "untaring    " $2 " please be patient "
    pushd $topdir > /dev/null
    tar xf $tarfname
    rm $tarfname
    popd > /dev/null
    echo "untaring complete!"
}

main_menu() {
    PS3='enter your choice: '
    echo "------------ select sensor to download:"
    options=("GoPro C1" "GoPro C2" "GoPro C3" "Tango Bottom" "Tango Top" "VI Sensor" "Intrinsic Calibration" "Extrinsic Calibration" "Quit")
    select opt in "${options[@]}"
    do
	case $opt in
        "GoPro C1")
	    download "c1" "$opt"
            ;;
        "GoPro C2")
	    download "c2" "$opt"
            ;;
        "GoPro C3")
	    download "c3" "$opt"
            ;;
        "Tango Bottom")
	    download "tango_bottom" "$opt"
            ;;
        "Tango Top")
	    download "tango_top" "$opt"
            ;;
        "VI Sensor")
	    download "visensor" "$opt"
            ;;
        "Intrinsic Calibration")
	    download "intrinsic_calib" "$opt"
	    ;;
        "Extrinsic Calibration")
	    download "extrinsic_calib" "$opt"
            ;;
        "Quit")
	    exit 0
            ;;
        *) echo invalid option;;
	esac
   break
   done
}

#
#  ------------------ here starts the main code -----------------------
#

if ! check_for_command wget; then
    echo "wget command not found, install it with sudo apt-get wget"
    exit -1
fi

if ! check_for_command tar; then
    echo "tar command not found, install it with sudo apt-get tar"
    exit -1
fi

echo "================ PENNCOSYVIO INSTALLER ======================"

while true; do main_menu; done

