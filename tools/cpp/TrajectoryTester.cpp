// -*-C++-*--------------------------------------------------------------------
// GRASP Lab UPenn
// Bernd Pfrommer 2016
//
#include "TrajectoryTester.h"
#include <iostream>
#include <cassert>
#include <math.h>
#include <string.h>
#include <iomanip>
using namespace std;

//#define DEBUG

#define FMT(X,Y,Z) fixed << setprecision(Z) << setw(Y) << X

static const double RPE_DRIFT_HORIZON = 20; // unit: meters

typedef Trajectory::TimeToPoseMap PMap;

TrajectoryTester::TrajectoryTester() :
	m_alignTime(Trajectory::INVALID_TIME),
	m_deltaT(20),
	m_makePlot(false) {
}

static void add_to_error(TrajectoryTester::Err *err, double w,
						 double S, const Pose &pose) {
	const double ooS = (S != 0)? 1.0/S : 0;
	double trans[3];
	pose.getTrans(trans);
	for (int i = 0; i < 3; i++) {
		err->err[i]  += w * fabs(trans[i]) * ooS;
		err->err3d   += w * trans[i] * trans[i] * ooS * ooS;
	}
	err->err2d   += w * (trans[0]*trans[0] + trans[1]*trans[1]) * ooS * ooS;
	// add the absolute rotation angle
	err->rotationError   += pose.getAngleMagnitude() * w;
}

static void normalize_error(TrajectoryTester::Err *err, double totW) {
	double ooTotW = (totW != 0) ? (1.0 / totW) : 0;
	for (int i = 0; i < 3; i++) {
		err->err[i]  *= 1.0 * ooTotW;
	}
	err->err3d = sqrt(err->err3d * ooTotW);
	err->err2d = sqrt(err->err2d * ooTotW);
	err->rotationError   *= ooTotW;
}


double
TrajectoryTester::getRelativePoseDifference(double t1, 
											const Trajectory &test,
											Pose *diffPose) const {
	*diffPose = Pose();
	double w(m_deltaT);	// weight is time difference
	double t2 = t1 + m_deltaT;
	const PMap &ref = m_ref.getPoses();
	PMap::const_iterator refp1 = ref.find(t1);
	assert(refp1 != ref.end());
	PMap::const_iterator refp2 = ref.lower_bound(t2);
	if (refp2 == ref.end()) {
		// we have hit the end of the reference positions, set
		// the weight to zero and return
		return (0);
	}

	const Pose &p1 = test.interpolate(t1);
	const Pose &p2 = test.interpolate(refp2->first);
	
	const Pose &rp1	= refp1->second;
	const Pose &rp2	= refp2->second;

	Pose deltapref = rp1.inverse().compose(rp2);
	Pose deltap    = p1.inverse().compose(p2);
	*diffPose = deltapref.inverse().compose(deltap);
	return (w);
}


TrajectoryTester::Result TrajectoryTester::test(const Trajectory &test) {
	Result res;
	const PMap &refPoses = m_ref.getPoses();

	assert(!test.empty() && !m_ref.empty());

	ofstream plotFile;
	if (m_makePlot) { plotFile.open("path_plot.txt"); }

	// total time of reference path
	double T = refPoses.rbegin()->first - refPoses.begin()->first;
	// total length of reference path
	double S = m_ref.getPathLength();
	double v = S / T;	// average speed over entire
	m_deltaT = RPE_DRIFT_HORIZON / v;
	double oneOverT = (T > 0) ? (1.0 / T) : 0;

	double t0 = refPoses.begin()->first;	// start time
	double transSum[3]; memset(transSum, 0, sizeof(transSum));
	double difftrans[3];
	double totATEWeight(0), totRPEWeight(0);
	for (PMap::const_iterator refIt = refPoses.begin();
		 refIt != refPoses.end(); ++refIt) {
		double t	= refIt->first;					// current timestamp
		//
		// get difference in poses and weights for both ATE and RPE
		//
		const Pose &rpose  = refIt->second;			// ref pose
		const Pose tpose   = test.interpolate(t);	// test pose
		Pose poseDiff = rpose.inverse().compose(tpose);
		Pose relPoseDiff;
		double wATE	= (t - t0) * oneOverT;			// weight
		double wRPE = getRelativePoseDifference(t, test, &relPoseDiff);
		
		totRPEWeight	+= wRPE;
		totATEWeight	+= wATE;
		if (m_makePlot) {
			plotFile << t << " " << tpose.getTransAsString() << " " <<
				rpose.getTransAsString() << " " <<
				poseDiff.getAngleMagnitude() << endl;
		}
		add_to_error(&res.ate, wATE, 1.0, poseDiff);
		add_to_error(&res.rpe, wRPE, RPE_DRIFT_HORIZON, relPoseDiff);

		poseDiff.getTrans(difftrans);
		for (int i = 0; i < 3; i++) {
			transSum[i] += wATE * difftrans[i];
		}
		t0 = t; 		// advance time
	}
	normalize_error(&res.ate, totATEWeight);
	normalize_error(&res.rpe, totRPEWeight);
#ifdef DEBUG
	// If the alignment was successful, the means of the two
	// trajectories should be almost identical. Almost: the alignment is done on
	// the time-overlapping region between test and reference trajectories,
	// whereas the test is done strictly on the entire time of the reference
	// trajectory.
	cout << "cross check: difference of means should be small: " << endl;
	for (int i = 0; i < 3; i++) { cout << " " << transSum[i]; }
	cout << endl;
#endif

	if (m_makePlot) {
		plotFile.close();
	}
	return (res);
}

void TrajectoryTester::Err::print(std::ostream &os, double mult,
								  const char *unit) {
	os << " rotational error: " << 
		FMT(rotationError * 180/M_PI, 6, 4) << "deg" << endl;
	os << " error in x: " << FMT(err[0]*mult, 6, 3) << unit << endl;
	os << " error in y: " << FMT(err[1]*mult, 6, 3) << unit << endl;
	os << " error in z: " << FMT(err[2]*mult, 6, 3) << unit << endl;
	os << " 2d error in x/y plane: " << FMT(err2d*mult,6,3) << unit << endl;
	os << " 3d positional error:   " << FMT(err3d*mult,6,3) << unit << endl;
}

void TrajectoryTester::Result::print(std::ostream &os) {
	os << "--- Average Trajectory Error:" << endl;
	ate.print(os, 1.0, "m");
	os << "--- Relative Position Error:" << endl;
	rpe.print(os, 100.0, "%");
}

std::ostream &operator<<(std::ostream &os, const TrajectoryTester::Result &r) {
	os << "ATE: " << r.ate << endl << " RPE: " << r.rpe;
	return (os);
}

std::ostream &operator<<(std::ostream &os, const TrajectoryTester::Err &e) {
	os << "err[x]: " << e.err[0] << ", err[y]: " << e.err[1]
	   << ", err[z]: " << e.err[2] << ", "
	   << "3derr: " << e.err3d << ", " << "2derr: " << e.err2d << ", roterr: "
	   << e.rotationError*180.0/M_PI << "deg";
	return (os);
}
