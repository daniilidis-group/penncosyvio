---
date: 2016-03-09T00:11:02+01:00
title: Download
weight: 10
---

## Preparing for Download

The installation instructions and scripts have been tested on
Ubuntu 14.04. Porting the scripts to any unix-like operating system
should be straight forward. The Windows OS is not supported, but we
provide raw download links so you can [piece the data together
yourself](#manual_download).

The full data set, without the frames extracted, has a size of about 25GB. If frames are extracted for all cameras and all sequences, the total size balloons to more than 100GB, so having some head room is definitely a good idea. For most purposes, 50GB should be more than enough space.

## Running the Installer

The install scripts are in the "master" branch of the
github repository, and can be run like this (you may need to install
`git` if it's not on your machine yet, using `sudo apt-get install git`):

```nohighlight
git clone https://github.com/daniilidis-group/penncosyvio.git
cd penncosyvio/tools
./download.bash 
================ PENNCOSYVIO INSTALLER ======================
------------ select sensor to download:
1) GoPro C1		  4) Tango Bottom	    7) Intrinsic Calibration
2) GoPro C2		  5) Tango Top		    8) Extrinsic Calibration
3) GoPro C3		  6) VI Sensor		    9) Quit
enter your choice: 
```

Pick the sensors that you are interested in and download the
data, which will end up in the `data` subdirectory. Once you quit out
of the installer tool, change into the data directory and see what's
there. This is how it looks if you downloaded only the `Tango Top` data:

```nohighlight
cd ..
tree data
data
└── tango_top
    ├── af
    │   ├── accelerometer.txt
    │   ├── gyroscope.txt
    │   ├── seq_af.mp4
    │   └── timestamps.txt
    ├── as
    │   ├── accelerometer.txt
    │   ├── gyroscope.txt
    │   ├── seq_as.mp4
    │   └── timestamps.txt
    ├── bf
    │   ├── accelerometer.txt
    │   ├── gyroscope.txt
    │   ├── seq_bf.mp4
    │   └── timestamps.txt
    └── bs
        ├── accelerometer.txt
        ├── gyroscope.txt
        ├── seq_bs.mp4
        └── timestamps.txt
```



## Running the Frame Extractor

Because reading mp4 files programmatically can be difficult (and notably because Matlab on Ubuntu skips frames due to some issues with the gstreamer framework), we provide a bash script to extract frames in PNG format. The script relies on either ffmpeg or avconv as an extraction tool. Either of them must be installed beforehand, avconf being easier to install as it is part of the Ubuntu distibution. Here is how to use the extraction tool:

```nohighlight
cd tools
./extract_frames.bash
------------ select frames to extract:
1) GoPro C1	 3) GoPro C3	  5) Tango Top
2) GoPro C2	 4) Tango Bottom  6) Quit
enter your choice: 5
------------- unpacking frames for Tango Top
 Be patient, this will take about 10-15min/sequence!
....
```

The extracted frames will end up in the respective subdirectory of the `data` subdirectory.

## Manual Download

<a name="manual_download"></a>
Here are links for direct download of the data files:

- [GoPro C1 (29.4GB)](http://visiondata.cis.upenn.edu/penncosyvio/tarfiles/c1.tar)
- [GoPro C2 (29.4GB)](http://visiondata.cis.upenn.edu/penncosyvio/tarfiles/c2.tar)
- [GoPro C3 (29.5GB)](http://visiondata.cis.upenn.edu/penncosyvio/tarfiles/c3.tar)
- [Tango Bottom (9.2GB)](http://visiondata.cis.upenn.edu/penncosyvio/tarfiles/tango_bottom.tar)
- [Tango Top (2.7GB)](http://visiondata.cis.upenn.edu/penncosyvio/tarfiles/tango_top.tar)
- [VI Sensor (10.5GB)](http://visiondata.cis.upenn.edu/penncosyvio/tarfiles/visensor.tar)

After download, untar them in the `data` directory.