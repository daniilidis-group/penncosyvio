// -*-C++-*--------------------------------------------------------------------
// GRASP Lab UPenn
// Bernd Pfrommer 2016
//
#include "Pose.h"
#include "Quaternion.h"
#include <string.h>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <math.h>
#include <cassert>

using namespace std;

//
// A(m x k)  = B(m x n) * C(n x k)
// 
static void matmult(double *a, const double *b, const double *c,
					int m, int n, int k) {
	// Not proud of the triple loop ... insert your favorite
	// math lib here.
	for (int i = 0; i < m; i++) {
		for (int l = 0; l < k; l++) {
			a[i * k + l] = 0; //a[i][l] = 0;
			for (int j = 0; j < n; j++) {
				//a[i][l] += b[i][j]*c[j][l];
				a[i * k + l ] += b[i * n + j] * c[j * k + l];
			}
		}
	}
}

Pose::Pose(double t) : m_time(t) {
	double ident[4][4] =  {{1, 0, 0, 0},
						   {0, 1, 0, 0},
						   {0, 0, 1, 0},
						   {0, 0, 0, 1}};
	memcpy(m_pose, ident, min(sizeof(ident),sizeof(m_pose)));
}


std::string Pose::getTransAsString() const {
	ostringstream strs;
	strs << getX() << " " << getY() << " " << getZ();
	return (strs.str());
}

Pose Pose::inverse() const {
	Pose pi(m_time);
	pi.m_pose[0][0] = m_pose[0][0];
	pi.m_pose[0][1] = m_pose[1][0];
	pi.m_pose[0][2] = m_pose[2][0];
	pi.m_pose[1][0] = m_pose[0][1];
	pi.m_pose[1][1] = m_pose[1][1];
	pi.m_pose[1][2] = m_pose[2][1];
	pi.m_pose[2][0] = m_pose[0][2];
	pi.m_pose[2][1] = m_pose[1][2];
	pi.m_pose[2][2] = m_pose[2][2];

	pi.m_pose[3][0] = 0;
	pi.m_pose[3][1] = 0;
	pi.m_pose[3][2] = 0;
	pi.m_pose[3][3] = 1;

	double trans[3] = {-m_pose[0][3], -m_pose[1][3], -m_pose[2][3]};
	double ri[9];
	pi.getRotMatrix(ri);
	double ti[3];
	matmult(ti, ri, trans, 3, 3, 1);
	pi.setTrans(ti);
	return (pi);
}

void Pose::getRotMatrix(double *r) const {
	r[ 0] = m_pose[0][0];	r[ 1] = m_pose[0][1];	r[ 2] = m_pose[0][2];
	r[ 3] = m_pose[1][0];	r[ 4] = m_pose[1][1];	r[ 5] = m_pose[1][2];
	r[ 6] = m_pose[2][0];	r[ 7] = m_pose[2][1];	r[ 8] = m_pose[2][2];
}

void Pose::getTrans(double *t) const {
	t[ 0] = m_pose[0][3];	t[ 1] = m_pose[1][3];	t[ 2] = m_pose[2][3];
}

double Pose::getAngleMagnitude() const {
	// acos (0.5*(trace(R)-1))
	double trace = m_pose[0][0] + m_pose[1][1] + m_pose[2][2];
	double angle = acos(max(min(0.5*(trace - 1.0), 1.0), -1.0));
	return (angle);
}

void Pose::setRotMatrix(const double *r) {
	m_pose[0][0] = r[ 0];	m_pose[0][1] = r[ 1];	m_pose[0][2] = r[ 2];
	m_pose[1][0] = r[ 3];	m_pose[1][1] = r[ 4];	m_pose[1][2] = r[ 5];
	m_pose[2][0] = r[ 6];	m_pose[2][1] = r[ 7];	m_pose[2][2] = r[ 8];
}

void Pose::setTrans(const double *t) {
	m_pose[0][3] = t[0];	m_pose[1][3] = t[ 1];	m_pose[2][3] = t[ 2];
}

Pose Pose::compose(const Pose &p) const {
	Pose pc(this->m_time);
	matmult((double *)pc.m_pose, (const double *)this->m_pose,
			(const double *)p.m_pose,
			4, 4, 4);
	return (pc);
}

void Pose::getQuaternion(double *q) const {
	const double (&r)[4][4] = m_pose;
	double tr = r[0][0] + r[1][1] + r[2][2];
    if (tr > 0) {
        const double S = sqrt(tr + 1.0) * 2.0;
		const double ooS = 1.0/S;
        q[0] = 0.25 * S;
		q[1] = (r[2][1] - r[1][2]) * ooS;
		q[2] = (r[0][2] - r[2][0]) * ooS;
		q[3] = (r[1][0] - r[0][1]) * ooS;
	} else {
        if ((r[0][0] > r[1][1]) && (r[0][0]> r[2][2])) {
			const double S = sqrt(1.0 + r[0][0]-r[1][1]-r[2][2]) * 2.0;
			const double ooS = 1.0/S;
            q[0] = (r[2][1]-r[1][2]) * ooS;
			q[1] = 0.25 * S;
			q[2] = (r[0][1]+r[1][0]) * ooS;
			q[3] = (r[0][2]+r[2][0]) * ooS;
		} else {
            if (r[1][1] > r[2][2]) {
				const double S = sqrt(1.0 + r[1][1]-r[0][0]-r[2][2]) * 2.0;
				const double ooS = 1.0/S;
                q[0] = (r[0][2]-r[2][0]) * ooS;
				q[1] = (r[0][1]+r[1][0]) * ooS;
				q[2] = 0.25 * S;
				q[3] = (r[1][2]+r[2][1]) * ooS;
			} else {
                const double S = sqrt(1.0 + r[2][2]-r[0][0]-r[1][1]) * 2.0;
				const double ooS = 1.0/S;
                q[0] = (r[1][0]-r[0][1]) * ooS;
				q[1] = (r[0][2]+r[2][0]) * ooS;
				q[2] = (r[1][2]+r[2][1]) * ooS;
				q[3] = 0.25 * S;
			}
		}
	}
}

void Pose::setQuaternion(const double *q) {
    double qw = q[0];
	double qx = q[1];
	double qy = q[2];
	double qz = q[3];
	double (&r)[4][4] = m_pose;
	r[0][0] = 1 - 2*qy*qy - 2*qz*qz;
	r[0][1]	= 2*qx*qy - 2*qz*qw;
	r[0][2]	= 2*qx*qz + 2 *qy*qw;
	r[1][0]	= 2 * qx*qy + 2*qz*qw;
	r[1][1]	= 1- 2*qx*qx - 2*qz*qz;
	r[1][2]	= 2*qy*qz - 2*qx*qw;
	r[2][0]	= 2 * qx *qz - 2*qy*qw;
	r[2][1]	= 2*qy*qz + 2 * qx*qw;
	r[2][2]	= 1 - 2*qx*qx - 2*qy*qy;
}

/*
 * reads pose from stream in the following format:
 * 
 * H_11 H_12 H_13 H_14  H_21 H_22 H_23 H_24 H_31 H_32 H_33 H_34
 *
 * where H is the 3x4 twist with H(1:3,1:3) being the rotation
 * and H(:,4) being the translation
 */

bool Pose::readFromStream(ifstream &str) {
	double *mat = (double *)(m_pose);
	int j(0);
	for (j = 0; j < 12 && str >> mat[j] ; j++);
	mat[12] = 0; mat[13] = 0; mat[14] = 0; mat[15] = 1.0;
	return (j== 12);
}

bool Pose::writeToStream(ofstream &str) const {
	str << setw(12) << setprecision(6) << m_time;
	double *mat = (double *)(m_pose);
	for (int row = 0; row < 3; row++) {
		for (int col = 0; col < 3; col++) {
			str << " " << setw(14) << setprecision(9) << mat[row* 4 + col];
		}
		str << " " << setw(12) << setprecision(5) << mat[row * 4 + 3];
	}
	return (true);
}


void Pose::addToTrans(double *trans, double w) const {
	for (int i = 0; i < 3; i++) {
		trans[i] +=  w * m_pose[i][3];
	}
}

Pose Pose::interpolate(const Pose &p2, double h) const {
	// take the mean of positions
	Pose p(*this);
	p.m_pose[0][3] = m_pose[0][3] * (1-h) + h * p2.m_pose[0][3];
	p.m_pose[1][3] = m_pose[1][3] * (1-h) + h * p2.m_pose[1][3];
	p.m_pose[2][3] = m_pose[2][3] * (1-h) + h * p2.m_pose[2][3];
	// do SLERP interpolation for the rotational component
	Quaternion q2(p2);
	Quaternion q1(*this);
	Quaternion interp = Quaternion::s_interpolate(q1, q2, h);
	p.setQuaternion(interp.getData());
	return (p);
}

void Pose::scale(double sc, const double *center) {
	// take the mean of positions
	m_pose[0][3] = (m_pose[0][3] - center[0]) * sc + center[0];
	m_pose[1][3] = (m_pose[1][3] - center[1]) * sc + center[1];
	m_pose[2][3] = (m_pose[2][3] - center[2]) * sc + center[2];
}

void Pose::scaleRotation(double sc) {
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			m_pose[i][j] *= sc;
		}
	}
}

void Pose::positionDifference(const Pose &p2, double *diff) const {
	diff[0] = m_pose[0][3] - p2.m_pose[0][3];
	diff[1] = m_pose[1][3] - p2.m_pose[1][3];
	diff[2] = m_pose[2][3] - p2.m_pose[2][3];
}

double Pose::positionDistance(const Pose &p2) const {
	double dist = 0;
	for (int i = 0; i < 3; i++) {
		double dp =  m_pose[i][3] - p2.m_pose[i][3];
		dist += dp*dp;
	}
	return (sqrt(dist));
}

std::ostream &operator<<(std::ostream &os, const Pose &p) {
	os << "[";
	for (int i = 0; i < 3; i++) {
		if (i > 0) {
			os << " ";
		}
		for (int j = 0; j < 4; j++) {
			os << " " << setw(15) << setprecision(9) << p.m_pose[i][j];
		}
		if (i == 2) {
			os << "]";
		}
		os << endl;
	}
	return (os);
}

