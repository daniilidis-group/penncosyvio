// -*-C++-*--------------------------------------------------------------------
// GRASP Lab UPenn
// Bernd Pfrommer 2016
//
#ifndef TRAJECTORYTESTER_H
#define TRAJECTORYTESTER_H

#include <map>
#include <iostream>
#include "Pose.h"
#include "Trajectory.h"

class TrajectoryTester {
public:
	struct Err {
		Err(): rotationError(0),err3d(0), err2d(0) {
			err[0] = 0; err[1] = 0; err[2] = 0;
		};
		double	rotationError;
		double	err3d;	// euclidian distance error in 3d
		double	err2d;	// error in x/y plane
		double	err[3];	// absolute error in each direction
		void print(std::ostream &os, double mult, const char *unit);
		friend std::ostream &operator<<(std::ostream &os, const Err &);
	};
	struct Result {
		Err		ate;	// absolute trajectory error
		Err		rpe;	// relative 
		void print(std::ostream &os);
		friend std::ostream &operator<<(std::ostream &os, const Result &);
	};
	/* Constructor for TrajectoryTester object */
	TrajectoryTester();
	/* Sets the time (in seconds) at which the reference and
	 * the test pose should be aligned. Automatic alignment will be
	 * done if no time is specified */
	void	setAlignTime(double t) { m_alignTime = t; }
	/* Sets the time horizon (in seconds) for relative position error */
	void	setRPETimeHorizon(double dt) { m_deltaT = dt; }
	/* Set this to true to create a plot file */
	void	setMakePlot(bool f) { m_makePlot = f; }
	/* Set the reference trajectory to which to compare */
	void	setReferenceTrajectory(const Trajectory &ref) { m_ref = ref; }

	/* Test pose sequence vs reference, and return rotational
	 * and translation errors
	 * @param test		the trajectory to test against
	 */
	Result test(const Trajectory &test);
	
	static const int INVALID_ALIGN_TIME;
private:
	TrajectoryTester(const TrajectoryTester &); // NOT IMPL
	TrajectoryTester& operator=(const TrajectoryTester &); // NOT IMPL
	double getRelativePoseDifference(double t1, const Trajectory &test,
									 Pose *diffPose) const;
	double		m_alignTime;
	double		m_deltaT;	// time horizon for RPE
	Trajectory	m_ref;		// reference trajectory
	bool		m_makePlot;
};

std::ostream &operator<<(std::ostream &os, const TrajectoryTester::Result &);
std::ostream &operator<<(std::ostream &os, const TrajectoryTester::Err &);

#endif
