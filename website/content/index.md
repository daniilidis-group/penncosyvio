---
date: 2016-03-08T21:07:13+01:00
title: The PennCOSYVIO Data Set
type: index
weight: 0
---
![ground truth from markers](pics/trajectory_animation.gif)
## What is PennCOSYVIO?

The PennCOSYVIO data set is collection of synchronized video and IMU data recorded at the University of Pennsylvania's Singh Center in April 2016. It is geared towards benchmarking of Visual Inertial Odometry algorithms on hand-held devices, but can also be used for other platforms such as micro aerial vehicles or ground robots.

## Trajectory
What sets this benchmark apart from previous ones is that it goes from outdoors to indoors:<br>
![Singh Center from the outside](pics/singh_outdoors_gopro.jpg)<br>
![inside the Singh Center](pics/singh_indoors_tango_rgb.jpg)<br>

The total path length is about 150m. We use an optical method to localize the sensors via fiducial markers (AprilTags) to within about 10cm. The animation at the top of the page shows which markers along the path are visible. These "ground truth" positions can be used to benchmark the results of VIO algorithms such as the Google Tango's:
![sequence AS trajectory](pics/sequence_as.jpg)

## Sensors
We loaded a bunch of sensors onto the rig: seven cameras and three IMUs total, including two Google Project Tango tablets, four GoPro Hero 4 Cameras, and a VI (Visual-Inertial) sensor. 
Nitin then hauled the rig into and through the Singh center:<br>
<table>
<tr>
<td><img src="pics/rig.jpg" width="500"></td>
<td><img src="pics/rig_carried.jpg" width="600"></td></tr>
</table>

Here are the sensor characteristics:
<table>
<tr><td>Sensor</td><td>Characteristics</td></tr>
<tr>
<td>C1,C2,C3</td>
<td>
<ul><li>GoPro Hero 4 Black</li>
<li><RGB 1920x1080 at 30fps on 'W' (wide) setting</li>
<li>rolling shutter</li>
<li>FOV: 69.5deg vert., 118.2deg horiz</li>
</ul>
</td>
</tr>
<tr>
<td>VI-Sensor</td>
<td>
<ul>
<li>Skybotix integrated VI-sensor</li>
<li>stereo camera: 2 x Aptina MT9V034</li>
<li>gray 2x752x480 at 20fps (rectified), global shutter</li>
<li>FOV: 57deg vert., 2 x 80deg horiz.</li>
<li>IMU: ADIS16488 at 200Hz</li>
</ul>
</td>
</tr>
<tr>
<td>Tango Bottom</td>
<td>
<ul>
<li>Google Project Tango 'Yellowstone' 7in tablet</li>
<li>RGB 1920x1080 at 30fps, rolling shutter</li>
<li>FOV: 31deg vert., 52deg horiz.</li>
<li>proprietary VIO pose estimation</li>
<li>accelerometer at 128Hz</li>
<li>gyroscope at 100Hz</li>
</ul>
</td>
</tr>
<tr>
<td>Tango Top</td>
<td>
<ul>
<li>Google Project Tango 'Yellowstone' 7in tablet</li>
<li>gray 640x480 at 30fps, global shutter</li>
<li>FOV: 100deg vert., 132deg horiz.</li>
<li>accelerometer at 128Hz</li>
<li>gyroscope at 100Hz</li>
</ul>
</td>
</tr>
</table>

## Citations
If you are using this dataset, please cite the [following publication](docs/penncosyvio.pdf):

	@inproceedings{DBLP:conf/icra/PfrommerSDC17,
	author    = {Bernd Pfrommer and
	Nitin Sanket and
	Kostas Daniilidis and
	Jonas Cleveland},
	title     = {PennCOSYVIO: {A} challenging Visual Inertial Odometry benchmark},
	booktitle = {2017 {IEEE} International Conference on Robotics and Automation, {ICRA}
	2017, Singapore, Singapore, May 29 - June 3, 2017},
	pages     = {3847--3854},
	year      = {2017},
	crossref  = {DBLP:conf/icra/2017},
	url       = {https://doi.org/10.1109/ICRA.2017.7989443},
	doi       = {10.1109/ICRA.2017.7989443},
	timestamp = {Wed, 26 Jul 2017 15:17:30 +0200},
	biburl    = {http://dblp.uni-trier.de/rec/bib/conf/icra/PfrommerSDC17},
	bibsource = {dblp computer science bibliography, http://dblp.org}
	}
