// -*-C++-*--------------------------------------------------------------------
// GRASP Lab UPenn
// Bernd Pfrommer 2016
//
#ifndef TRAJECTORY_H
#define TRAJECTORY_H

#include <map>
#include <iostream>
#include "Pose.h"
#include <vector>

class Trajectory {
public:
	/* Defined as shorthand */
	typedef std::map<double, Pose> TimeToPoseMap;
	/** adds pose to trajectory */
	bool	addPose(double ts, const Pose &p);
	/** returns the poses */
	const TimeToPoseMap &getPoses() const { return (m_poses); }
	/** returns number of poses in trajectory */
	size_t	size() const { return (m_poses.size()); }
	/** returns true if no poses are there */
	bool	empty() const { return (m_poses.empty()); }
	/** returns pose interpolated to time stamp t */
	Pose	interpolate(double t) const;
	/** computes position mean */
	std::vector<double> mean() const;
	/** total path length of trajectory */
	double	getPathLength() const;
	/** applies pose transformation to entire trajectory */
	Trajectory applyTransform(const Pose &transform) const;
	/** get first timestamp */
	double getFirstTime() const;
	/** get last timestamp */
	double getLastTime() const;
	/** scale positions by factor sc, centered around center */
	void scale(double sc, const double *center);

	// ------------- static methods
	static const int 	INVALID_TIME;

	static Trajectory	s_read(char const *fname);
	static void 		s_write(const Trajectory &t, char const *fname);
	static Trajectory	s_align(const Trajectory &ref,
								const Trajectory &test,
								Pose *p,
								double alignTime = INVALID_TIME,
								bool allowScale = false);
	/** keep only within [tstart, tend] */
	static Trajectory	s_truncate(const Trajectory &t, double tstart, double
								   tend);
	static Pose 		s_findPoseDifferenceAtTime(const Trajectory &ref,
												   const Trajectory &test,
												   double time);
	friend std::ostream &operator<<(std::ostream &os, const Trajectory &);

private:
	TimeToPoseMap	m_poses;
	static Pose s_getOptimalRotation(const Trajectory &ref,
									 const Trajectory &test);
	static void s_getMeans(const Trajectory &ref, const Trajectory &test,
						   double *meanRef, double *meanTest);

	/* helper function to get overlapping time regions */
	static double s_getTimeLimits(const Trajectory &ref,
									const Trajectory &test,
									TimeToPoseMap::const_iterator *startIt,
									TimeToPoseMap::const_iterator *endIt);
	static double s_getScale(const Trajectory &ref, const Trajectory &test);
	static Trajectory s_matchOrientation(const Trajectory &ref,
										 const Trajectory &test,
										 Pose *p,
										 double alignTime);
	static Trajectory s_matchMeans(const Trajectory &ref,
								   const Trajectory &test,
								   Pose *p);
};
std::ostream &operator<<(std::ostream &os, const Trajectory &);

#endif
