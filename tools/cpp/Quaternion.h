// -*-C++-*--------------------------------------------------------------------
// GRASP Lab UPenn
// Bernd Pfrommer 2016
//
#ifndef QUATERNION_H
#define QUATERNION_H

#include <string>
#include <fstream>

class Pose;

class Quaternion {
public:
	Quaternion();
	Quaternion(const double *);
	Quaternion(const Pose &pose);

	Quaternion regularize(const Quaternion &q) const;
	Quaternion inverse() const;
	Quaternion mult(const Quaternion &q) const;
	Quaternion pow(double pow) const;
	double	   dist(const Quaternion &q) const;
	Quaternion scale(double h) const;
	double	   norm() const;
	const double *getData() const { return (m_q); }

	void toPose(Pose *pose) const;

	static Quaternion s_interpolate(const Quaternion &q1,
									const Quaternion &q2, double h);

private:
	double m_q[4];	// order: w, x, y, z
	friend std::ostream &operator<<(std::ostream &os, const Quaternion &);
};

std::ostream &operator<<(std::ostream &os, const Quaternion &);

#endif
