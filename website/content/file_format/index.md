---
date: 2016-03-09T00:11:02+01:00
title: File Format
weight: 15
---

## Video Files

Raw video files are provided as `mp4` files, with corresponding
`timestamps.txt` files. For the VI sensor, we recommend using the
original data from the ROS bag or the extracted frames rather than the
provided `mp4` files as the latter suffer from quality loss due to
compression.

See the description in the [Download](../download) section on how to extract
individual frames from the mp4 files. Note 
that a bug somewhere between gstreamer and Matlab makes playing back
frames with Matlab's VideoReader unreliable. It is recommended to play
back from the extracted frames using the `CustomVideoReader` Matlab
class in the `tools/matlab/` directory.

For the orientation of the camera axis, see the [Extrinsic Calibration](../extrinsic_calib) section.

## IMU Files

The IMU data is stored in flat text files:

 - `accelerometer.txt` (Tango): timestamp in seconds, acceleration in x,y,z
   (in m/sec^2)
 - `gyroscope.txt` (Tango): timestamp in seconds, angular velocity around x,
   y, z axes (in rad/sec)
 - `imu.txt` (VI sensor): timestamp in seconds, acceleration in x,y,z
   (in m/sec^2), angular velocity around x, y, z axes (in rad/sec)

For axis orientation of the IMUs, please refer to the [Extrinsic Calibration](../extrinsic_calib) section.

## Pose (Ground Truth) Files

The ground truth and Tango pose files are flat text files that contain the pose as a
3x4 matrix
$$
\cc{\mvec{P}:=\begin{bmatrix}\mvec{R}\ \mvec{t}\end{bmatrix}}
$$
corresponding to the body(rig)-to-world transform:
$$
\cc{\ctrans{T}{B}{W} = \begin{bmatrix}\mvec{R}\ \mvec{t}\end{bmatrix}}
$$
Each line in the pose file starts with a timestamp (in seconds) and is
followed by the matrix $\cc{\mvec{P}}$ in row major format, i.e:

timestamp(sec),
$\cc{\mvec{P}\_{11}}$, $\cc{\mvec{P}\_{12}}$,$\cc{\mvec{P}\_{13}}$, $\cc{\mvec{P}\_{14}}$,
$\cc{\mvec{P}\_{21}}$, $\cc{\mvec{P}\_{22}}$,$\cc{\mvec{P}\_{23}}$, $\cc{\mvec{P}\_{24}}$,
$\cc{\mvec{P}\_{31}}$, $\cc{\mvec{P}\_{32}}$,$\cc{\mvec{P}\_{33}}$, $\cc{\mvec{P}\_{34}}$.

Input files for benchmarking must be produced in this format as well.
