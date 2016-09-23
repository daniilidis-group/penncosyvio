// -*-C++-*--------------------------------------------------------------------
// GRASP Lab UPenn
// Bernd Pfrommer 2016
//

#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <math.h>
#include "TrajectoryTester.h"

using namespace std;
#define FMT(X,Y,Z) fixed << setprecision(Z) << setw(Y) << (X)

static int usage() {
	cout << "test_trajectory [-a <align_time>] [-p (plot)] "
		"[-s (allow scaling)] -r <reference_trajectory_file> "
		"-t <test_trajectory_file>" << endl;
	return (-1);
}

int main(int argc, char* argv[]) {
	int c;
	const char *refFile(NULL);
	const char *testFile(NULL);
	double alignTime(Trajectory::INVALID_TIME);
	bool makePlot(false), allowScale(false);
	while ((c = getopt(argc, argv, "hsr:t:a:p")) != -1) {
		switch (c) {
		default:
		case 'h':	return(usage());			break;
		case 'r':	refFile			= optarg;	break;
		case 's':	allowScale		= true;		break;
		case 'a':	alignTime 		= atof(optarg);	break;
		case 'p':	makePlot		= true;		break;
		case 't':	testFile		= optarg;	break;
		}
	}
	if (optind != argc) return (usage());
	if (!refFile) {
		cout << "must specify reference pose file!" << endl;
		return (usage());
	}
	if (!testFile) {
		cout << "must specify test pose file!" << endl;
		return (usage());
	}
	TrajectoryTester trajectoryTester;
	try {
		Trajectory refTrajectory  = Trajectory::s_read(refFile);
		Trajectory testTrajectory = Trajectory::s_read(testFile);
		Pose trans;
		Trajectory alignedTestTrajectory  =
			Trajectory::s_align(refTrajectory, testTrajectory, &trans,
								alignTime, allowScale);
		cout << "alignment transform: " << endl << trans << endl;
		trajectoryTester.setMakePlot(makePlot);
		trajectoryTester.setReferenceTrajectory(refTrajectory);
		TrajectoryTester::Result res =
			trajectoryTester.test(alignedTestTrajectory);
		cout << "---------------------------- " << endl;
		cout << "length of reference trajectory: " << 
			 FMT(refTrajectory.getPathLength(),6,3) << "m" << endl;
		cout << "length of test trajectory:      " << 
			 FMT(testTrajectory.getPathLength(),6,3) << "m" << endl;
		cout << "---------------------------- " << endl;
		res.print(cout);
	} catch (const exception &e) {
		cout << "ERROR: " << e.what() << endl;
		return (-1);
	}
	return (0);
}
