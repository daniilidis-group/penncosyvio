// -*-C++-*--------------------------------------------------------------------
// GRASP Lab UPenn
// Bernd Pfrommer 2016
//
#include "Trajectory.h"
#include "Quaternion.h"
#include <string.h>
#include <fstream>
#include <stdexcept>
#include <sstream>
#include <iostream>
#include <cassert>
#include <iomanip>
#include <math.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_eigen.h>
using namespace std;


const int Trajectory::INVALID_TIME(-1e9);

/*
 * finds rotation by determining the eigenvector of the
 * largest eigenvalue.
 */
static Pose findRotFromEig(double *eigmat) {
	Pose dp;
	//
	// use the GSL library to do eigen decomposition of
	// N, and pick the eigenvector of the largest eigen
	// value.

	gsl_matrix_view m = gsl_matrix_view_array(eigmat, 4, 4);
	gsl_vector *eval = gsl_vector_alloc(4);
	gsl_matrix *evec = gsl_matrix_alloc(4, 4);
	gsl_eigen_symmv_workspace *w = gsl_eigen_symmv_alloc(4);
	gsl_eigen_symmv(&m.matrix, eval, evec, w);
	gsl_eigen_symmv_free(w);
	gsl_eigen_symmv_sort(eval, evec, GSL_EIGEN_SORT_VAL_DESC);
	double qdiff[4];
	for (int i = 0; i < 4; i++) {
		qdiff[i] = gsl_matrix_get(evec, i, 0);
	}
	gsl_vector_free(eval);
	gsl_matrix_free(evec);

	dp.setQuaternion(qdiff);
	return (dp);
}

/*
 * reads poses from file in the following format:
 * 
 * time[sec] H_11 H_12 H_13 H_14  H_21 H_22 H_23 H_24 H_31 H_32 H_33 H_34
 *
 * where H is the 3x4 twist with H(1:3,1:3) being the rotation
 * and H(:,4) being the translation
 */

Trajectory Trajectory::s_read(const char *fname) {
	Trajectory t;
	ifstream file(fname);
	if (!file) {
		throw runtime_error("cannot open file " + string(fname));
	}
	double tstamp;
	while (file >> tstamp) {
		Pose p(tstamp);
		if (!p.readFromStream(file)) {
			throw runtime_error("unexpected end of file " + string(fname));
		}
		if (!t.addPose(tstamp, p)) {
			ostringstream strs;
			strs << tstamp;
			throw runtime_error("duplicate timestamp " + strs.str() +
								" in " + string(fname));
		}
	}
	if (t.empty()) {
		throw runtime_error("no poses found in file " + string(fname));
	}
	cout << "read " << t.size() << " poses from " << fname << endl;
	return (t);
}


void Trajectory::s_write(const Trajectory &t, const char *fname) {
	ofstream file(fname);
	if (!file) {
		throw runtime_error("cannot open file " + string(fname));
	}
	for(TimeToPoseMap::const_iterator it = t.m_poses.begin();
		it != t.m_poses.end(); ++it) {
		Pose p(it->second);
		p.setTime(it->first);
		if (!p.writeToStream(file)) {
			throw runtime_error("write failed to " + string(fname));
		}
		file << endl;
	}
}

bool Trajectory::addPose(double ts, const Pose &p) {
	if (m_poses.find(ts) == m_poses.end()) {
		m_poses[ts] = p;
		return (true);
	}
	return (false);
}

double Trajectory::getFirstTime() const {
	return (m_poses.empty() ? INVALID_TIME : m_poses.begin()->first);
}

double Trajectory::getLastTime() const {
	return (m_poses.empty() ? INVALID_TIME : m_poses.rbegin()->first);
}

Pose Trajectory::interpolate(double t) const {
	Pose ipose;
	// lower_bound returns element greater than or equal
	TimeToPoseMap::const_iterator p2 = m_poses.lower_bound(t);
	if (p2 == m_poses.end()) {
		// beyond last timestamp, return last pose
		return (m_poses.rbegin()->second);
	}
	if (p2 == m_poses.begin()) {
		// before first timestamp, return first pose
		return (p2->second);
	}
	TimeToPoseMap::const_iterator p1 = p2;
	p1--;
	double dt = p2->first - p1->first;  // time diff between bins
	double h  = (t - p1->first) / dt; // between 0 and 1
	// interpolate between lower bin (p1) and upper bin (p2)
	ipose = p1->second.interpolate(p2->second, h);
	return (ipose);
}

std::vector<double> Trajectory::mean() const {
	std::vector<double> sum(3);
	sum[0] = 0; sum[1] = 0; sum[2] = 0;
	for (TimeToPoseMap::const_iterator it = m_poses.begin();
		 it != m_poses.end(); ++it) {
		double tr[3];
		it->second.getTrans(tr);
		for (int i = 0; i < 3; i++) {
			sum[i] += tr[i];
		}
	}
	double oon = (m_poses.size() == 0) ? 0 : (1.0/m_poses.size());
	for (int i = 0; i < 3; i++) {
		sum[i] *= oon;
	}
	return (sum);
}

/** total path length of trajectory */
double Trajectory::getPathLength() const {
	if (m_poses.empty()) {
		return (0);
	}
	double l(0);
	Pose lastPose(m_poses.begin()->second);
	for (TimeToPoseMap::const_iterator it = m_poses.begin();
		 it != m_poses.end(); ++it) {
		l += it->second.positionDistance(lastPose);
		lastPose = it->second;
	}
	return(l);
}


void Trajectory::scale(double sc, const double *center) {
	for (TimeToPoseMap::iterator it = m_poses.begin();
		 it != m_poses.end(); ++it) {
		it->second.scale(sc, center);
	}
}



double
Trajectory::s_getTimeLimits(const Trajectory &ref, const Trajectory &test,
							TimeToPoseMap::const_iterator *startIt,
							TimeToPoseMap::const_iterator *endIt) {
	const TimeToPoseMap &rposes = ref.m_poses;
	double tstart = max(rposes.begin()->first,
						test.m_poses.begin()->first);
	double tend   = min(rposes.rbegin()->first,
						test.m_poses.rbegin()->first);

	*startIt	= rposes.lower_bound(tstart);
	*endIt		= rposes.upper_bound(tend);
	if (*startIt == rposes.end()) {
		throw runtime_error("error finding time overlap between trajectories");
	}
	assert(*startIt != rposes.end());
	TimeToPoseMap::const_iterator lastTimeIt(*endIt);
	double tlast(0);
	if (lastTimeIt == rposes.end()) {
		tlast = rposes.rbegin()->first;
	} else {
		if (lastTimeIt != rposes.begin()) {
			tlast = (--lastTimeIt)->first;
		} else {
			tlast = rposes.begin()->first;
		}
	}
	double T = tlast - (*startIt)->first;
	if (T <= 0) {
		throw runtime_error("time span in overlap of trajectories is zero!");
	}
	return (T);
} 


void Trajectory::s_getMeans(const Trajectory &ref,
							const Trajectory &test,
							double *meanRef, double *meanTest) {
	TimeToPoseMap::const_iterator startIt;
	TimeToPoseMap::const_iterator endIt;
	const double T = s_getTimeLimits(ref, test, &startIt, &endIt);
	const double ooT = 1.0/T;

	memset(meanRef,  0, 3 * sizeof(double));
	memset(meanTest, 0, 3 * sizeof(double));
	double t0(startIt->first);
	for (TimeToPoseMap::const_iterator refIt = startIt;
		 refIt != endIt; ++refIt) {
		Pose testPose    = test.interpolate(refIt->first);
		const Pose &refPose = refIt->second;
		double w = (refIt->first - t0) * ooT;
		testPose.addToTrans(meanTest, w);
		refPose.addToTrans(meanRef, w);
		t0 = refIt->first;
	}
}

Trajectory Trajectory::applyTransform(const Pose &transform) const {
	Trajectory transformed;
	for (TimeToPoseMap::const_iterator it = m_poses.begin();
		 it != m_poses.end(); ++it) {
		transformed.addPose(it->first, transform.compose(it->second));
	}
	return (transformed);
}

Trajectory Trajectory::s_matchMeans(const Trajectory &ref,
									const Trajectory &test,
									Pose *p) {
	double meanRef[3], meanTest[3];
	s_getMeans(ref, test, meanRef, meanTest);

	double trans[3];
	for (int i = 0; i < 3; i++) {
		trans[i] = meanRef[i] - meanTest[i];
	}
	Pose meanDiff;
	meanDiff.setTrans(trans);
	p->setTrans(trans);
	Trajectory matched = test.applyTransform(meanDiff);
	return (matched);
}

Trajectory Trajectory::s_matchOrientation(const Trajectory &ref,
										  const Trajectory &test,
										  Pose *p,
										  double alignTime) {
	Pose trans;
	if (alignTime != INVALID_TIME) {
		cout << "alignment time set, skipping automatic orientation" << endl;
		trans = s_findPoseDifferenceAtTime(ref, test, alignTime);
	} else {
		trans = s_getOptimalRotation(ref, test);
	}
	*p = trans;
	return (test.applyTransform(trans));
}


Pose Trajectory::s_getOptimalRotation(const Trajectory &ref,
									  const Trajectory &test) {
	//
	// implements finding the optimal rotation according to 
	// Berthold K.P. Horn, "Closed-form solution of absolute orientation
	// using unit quaternions"
	//
	TimeToPoseMap::const_iterator startIt;
	TimeToPoseMap::const_iterator endIt;
	const double T   = s_getTimeLimits(ref, test, &startIt, &endIt);
	const double ooT = 1.0/T;
	double meanTest[3];
	double meanRef[3];
	double M[3][3];		memset(M, 0, sizeof(M));

	s_getMeans(ref, test, meanRef, meanTest);

	// first calculate covariance matrix between the corresponding positions
	double t0(startIt->first);
	for (TimeToPoseMap::const_iterator refIt = startIt;
		 refIt != endIt; ++refIt) {
		Pose testPose       = test.interpolate(refIt->first);
		const Pose &refPose = refIt->second;
		double rref[3], rtest[3];
		refPose.getTrans(rref);
		testPose.getTrans(rtest);
		double w = (refIt->first - t0) * ooT;
		// update the M matrix
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				M[i][j] += w * (rref[j]-meanRef[j]) * (rtest[i]-meanTest[i]);
			}
		}
		t0 = refIt->first;
	}
	// this is the N matrix in Horn's paper
	double N[16] = {
		M[0][0]+M[1][1]+M[2][2],	M[1][2]-M[2][1],	 M[2][0]-M[0][2],		M[0][1]-M[1][0],
		M[1][2]-M[2][1],		M[0][0]-M[1][1]-M[2][2], M[0][1]+M[1][0],		M[2][0]+M[0][2],
		M[2][0]-M[0][2],		M[0][1]+M[1][0],	 -M[0][0]+M[1][1]-M[2][2],	M[1][2]+M[2][1],
		M[0][1]-M[1][0],		M[2][0]+M[0][2],	 M[1][2]+M[2][1],		-M[0][0]-M[1][1]+M[2][2]};
	return (findRotFromEig(N));
}

double Trajectory::s_getScale(const Trajectory &ref,
							  const Trajectory &test) {
	TimeToPoseMap::const_iterator startIt;
	TimeToPoseMap::const_iterator endIt;
	const double T   = s_getTimeLimits(ref, test, &startIt, &endIt);
	const double ooT = 1.0/T;
	double meanTest[3];
	double meanRef[3];
	s_getMeans(ref, test, meanRef, meanTest);

	double sdenom(0), snumer(0);
	double t0(startIt->first);
	for (TimeToPoseMap::const_iterator refIt = startIt;
		 refIt != endIt; ++refIt) {
		Pose testPose       = test.interpolate(refIt->first);
		const Pose &refPose = refIt->second;
		double rref[3], rtest[3];
		refPose.getTrans(rref);
		testPose.getTrans(rtest);
		double w = (refIt->first - t0) * ooT;
		// update the M matrix
		for (int i = 0; i < 3; i++) {
			snumer += w * (rref[i]-meanRef[i]) * (rtest[i]-meanTest[i]);
			sdenom += w * (rref[i]-meanRef[i]) * (rref[i]-meanRef[i]);
		}
		t0 = refIt->first;
	}
	
	return (sdenom > 0? snumer/sdenom : 1);
}

Pose Trajectory::s_findPoseDifferenceAtTime(const Trajectory &ref,
											const Trajectory &test,
											double lineupTime) {
	assert(!ref.empty() && !test.empty());
	double t = max(max(ref.m_poses.begin()->first,
						  test.m_poses.begin()->first), lineupTime);
	t = min(t, min(ref.m_poses.rbegin()->first, test.m_poses.rbegin()->first));
	Pose refPose  = ref.interpolate(t);
	Pose testPose = test.interpolate(t);
	Pose p = refPose.compose(testPose.inverse());
	return (p);
}

Trajectory Trajectory::s_align(const Trajectory &ref, const Trajectory &test,
							   Pose *p, double alignTime, bool allowScale) {
	Trajectory rotated = Trajectory::s_matchOrientation(ref, test,
														p, alignTime);
	double scale = s_getScale(ref, rotated);
	if (allowScale) {
		cout << "scaled by: " << scale << endl;
		double meanRotated[3];
		double meanRef[3];
		s_getMeans(ref, rotated, meanRef, meanRotated);
		rotated.scale(scale, meanRotated);
		p->scaleRotation(scale);
	} else {
		cout << "*not* scaling by scale " << scale << endl;
	}
	Trajectory aligned = Trajectory::s_matchMeans(ref, rotated, p);
	return (aligned);
}

Trajectory Trajectory::s_truncate(const Trajectory &t, double tstart, double tend) {
	Trajectory tt(t);
	if (tstart == INVALID_TIME || tend == INVALID_TIME) {
		return (tt);
	}
	//
	// first remove the head
	//
	TimeToPoseMap::iterator istart = tt.m_poses.lower_bound(tstart); // >=
	if (istart != tt.m_poses.begin()) {
		--istart;
	} else {
		istart = tt.m_poses.end();
	}
	if (istart != tt.m_poses.end()) {
		tt.m_poses.erase(tt.m_poses.begin(), istart);
	}
	//
	// now remove the tail
	//

	TimeToPoseMap::iterator iend = tt.m_poses.upper_bound(tend); // >
	tt.m_poses.erase(tt.m_poses.upper_bound(tend), tt.m_poses.end());
	return (tt);
}

std::ostream &operator<<(std::ostream &os, const Trajectory &t) {
	int i(0);
	const int NPRINT(5);
	os << "[T:";
	for(Trajectory::TimeToPoseMap::const_iterator p = t.m_poses.begin();
		p != t.m_poses.end(); ++p) {
		os << " " << p->first;
		if (++i > NPRINT) break;
	}
	os << "]";
	return (os);
}
