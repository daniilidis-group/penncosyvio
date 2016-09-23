// -*-C++-*--------------------------------------------------------------------
// GRASP Lab UPenn
// Bernd Pfrommer 2016
//

#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include "Trajectory.h"

using namespace std;

static int usage() {
	cout << "align_trajectory [-a <align_time>] [-T truncate] "
		"[-s allow_scale] "
		"-r <reference_trajectory_file> -t <test_trajectory_file> "
		"-o <output_file>" << endl;
	return (-1);
}

int main(int argc, char* argv[]) {
	int c;
	const char *refFile(NULL);
	const char *testFile(NULL);
	const char *outputFile(NULL);
	double alignTime(Trajectory::INVALID_TIME);
	bool truncate(false), allowScale(false);
	while ((c = getopt(argc, argv, "hr:sTt:a:o:")) != -1) {
		switch (c) {
		case 'a':	alignTime	= atof(optarg);	break;
		default:
		case 'h':	return(usage());		break;
		case 'o':	outputFile	= optarg;	break;
		case 'r':	refFile		= optarg;	break;
		case 't':	testFile	= optarg;	break;
		case 'T':	truncate 	= true;		break;
		case 's':	allowScale 	= true;		break;
		}
	}
	if (optind != argc) return (usage());
	if (!refFile) {
		cout << "must specify reference trajectory file!" << endl;
		return (usage());
	}
	if (!testFile) {
		cout << "must specify test trajectory file!" << endl;
		return (usage());
	}
	if (!outputFile) {
		cout << "must specify output trajectory file!" << endl;
		return (usage());
	}
	try {
		Trajectory refTrajectory  = Trajectory::s_read(refFile);
		Trajectory testTrajectory = Trajectory::s_read(testFile);
		Pose trans;
		Trajectory aligned =
			Trajectory::s_align(refTrajectory, testTrajectory, &trans,
								alignTime, allowScale);
		if (truncate) {
			aligned = Trajectory::s_truncate(aligned,
											 refTrajectory.getFirstTime(),
											 refTrajectory.getLastTime());
		}
		cout << "alignment transform: " << endl << trans << endl;
		Trajectory::s_write(aligned, outputFile);
	} catch (const exception &e) {
		cout << "ERROR: " << e.what() << endl;
		return (-1);
	}
	return (0);
}
