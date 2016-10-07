#!/bin/bash
#-------------------------------------------------------------------------------
#
# 2016 Bernd Pfrommer
# UPenn GRASP lab
#
# Script to extract frames from the PennCOSYVIO data set
#

#
# names of the sequences: a-slow, a-fast, b-slow, b-fast
#
sequences="as af bs bf"

#
# if frame extraction is done, this is the file type generated
#
img_typ="png"

#
# base directory
#
base_dir=../data

#
# the command to use (ffmpeg or avconv)
#
frametool=


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

askifremove () {
    read -r -p "${1:-  Delete existing frames [y/N]} " rc
    case $rc in
        [yY][eE][sS]|[yY]) 
            true
            ;;
        *)
            false
            ;;
    esac
}

check_for_frametool() {
    if check_for_command ffmpeg; then
	frametool="ffmpeg -hide_banner"
    else
	if check_for_command avconv; then
	    frametool=avconv
	else
	    echo "neither avconv nor ffmpeg found!"
	    echo "install either avconv with sudo apt-get libav-tools"
	    echo "or ffmpeg via PPA from http://ffmpeg.org/download.html"
	    return -1
	fi
    fi
    return 0
}

check_frame_count() {
    cam=$1
    seq=$2
    d=$base_dir/$cam/$seq
    nfiles=`ls -1 $d/frames/frame_*.${img_typ} | wc | awk '{print $1}'`
    nframes=`wc $d/timestamps.txt | awk '{print $1}'`
    if [[ !( $nframes == $nfiles) ]] ; then
	echo $d 'number of frames: ' $nframes ' does not agree with number of files: ' $nfiles
	exit
    else
	cp $d/timestamps.txt $d/frames/
    fi
}

extract_frames() {
    cam=$1
    seq=$2
    d=$base_dir/$cam/$seq
    vid_name=seq_${seq}.mp4
    fdir=$d/frames
    if [[ -d $fdir ]]; then
	echo "  WARNING: directory $fdir already exists!"
	askifremove
	if [[ $? -eq 0 ]]; then
	    echo "  removing $fdir";
	    rm -rf $fdir
	else
	    echo "  skipping sequence $cam/$seq";
	    return -1
	fi
    fi
    mkdir -p $fdir
    if check_for_frametool; then
	echo "  extracting frames for sequence $seq"
	rm -f $fdir/frame_*.${img_typ}
	if [[ $1 == *"tango"* ]] ; then
	    # for tangos, need the -r 30 to get the right number of frames on output
	    $frametool -r 30 -i $d/$vid_name -vf "select=gte(n\,0)" $fdir/frame_%04d.${img_typ}
	else
	    $frametool -i $d/$vid_name -vf "select=gte(n\,0)" $fdir/frame_%04d.${img_typ}
	fi
	return 0;
    else
	echo " ERROR: no frametool found, cannot extract frames!"
    fi
}



unpack_frames() {
    camdir=$base_dir/$1
    if [[ ! -d "$camdir" ]]; then
	echo
	echo "ERROR: data directory $camdir not there, download first!"
	echo
	return
    fi

    echo "------------- unpacking frames for ${2}"
    echo " Be patient, this will take about 10-15min/sequence!"
    echo " You can ignore deprecated pixel format warnings."
    for seq in $sequences; do
	extract_frames "$1" "$seq" "$2"
	if [[ $? -eq 0 ]]; then
	    check_frame_count "$1" "$seq" "$2"
	fi
    done
    echo " frame extraction for ${2} COMPLETED!"
}

main_menu() {
    echo "------------ select frames to extract:"
    PS3='enter your choice: '
    options=("GoPro C1" "GoPro C2" "GoPro C3" "Tango Bottom" "Tango Top" "Quit")
    select opt in "${options[@]}"
    do
	case $opt in
        "GoPro C1")
	    unpack_frames "c1" "$opt"
            ;;
        "GoPro C2")
	    unpack_frames "c2" "$opt";
            ;;
        "GoPro C3")
	    unpack_frames "c3" "$opt";
            ;;
        "Tango Bottom")
	    unpack_frames "tango_bottom" "$opt";
            ;;
        "Tango Top")
	    unpack_frames "tango_top" "$opt";
            ;;
        "Quit")
	    exit 0
            ;;
        *) echo invalid option;;
	esac
   break
   done
}


while true; do main_menu; done

