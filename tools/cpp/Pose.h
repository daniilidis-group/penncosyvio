// -*-C++-*--------------------------------------------------------------------
// GRASP Lab UPenn
// Bernd Pfrommer 2016
//
#ifndef POSE_H
#define POSE_H

#include <string>
#include <fstream>
#include <vector>

class Pose {
public:
	Pose(double timestamp = 0);

	// getters
	void	getRotMatrix(double *r) const;
	double	getAngleMagnitude() const;
	void 	getQuaternion(double *q) const;
	double	getX() const {	return (m_pose[0][3]); }
	double	getY() const {	return (m_pose[1][3]); }
	double	getZ() const {	return (m_pose[2][3]); }
	void	getTrans(double *t) const;
	std::string getTransAsString() const;

	// setters
	void 	setTime(double t) { m_time = t; }
	void	setRotMatrix(const double *r);
	void	setTrans(const double *t);
	void 	setQuaternion(const double *q);

	// fancier operations
	Pose	compose(const Pose &p) const;
	Pose	inverse() const;
	void	addToTrans(double *trans, double w) const;
	Pose	interpolate(const Pose &p2, double h) const;
	void	scale(double sc, const double *center);
	void	scaleRotation(double sc);
	double	positionDistance(const Pose &p2) const;
	void	positionDifference(const Pose &p2, double *diff) const;

	// I/O related
	bool	readFromStream(std::ifstream &str);
	bool	writeToStream(std::ofstream &str) const;
private:
	double m_time;
	double m_pose[4][4];
	friend std::ostream &operator<<(std::ostream &os, const Pose &);
};

std::ostream &operator<<(std::ostream &os, const Pose &);

#endif
