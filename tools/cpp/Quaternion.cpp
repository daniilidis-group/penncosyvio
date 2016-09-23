// -*-C++-*--------------------------------------------------------------------
// GRASP Lab UPenn
// Bernd Pfrommer 2016
//
#include "Quaternion.h"
#include "Pose.h"
#include <math.h>
#include <iostream>

using namespace std;


Quaternion::Quaternion() {
	m_q[0] = 1;
	m_q[1] = 0;
	m_q[2] = 0;
	m_q[3] = 0;
}

Quaternion::Quaternion(const double *q) {
	m_q[0] = q[0];
	m_q[1] = q[1];
	m_q[2] = q[2];
	m_q[3] = q[3];
}

Quaternion::Quaternion(const Pose &p) {
	p.getQuaternion(m_q);
}

void Quaternion::toPose(Pose *pose) const {
	pose->setQuaternion(m_q);
}

Quaternion Quaternion::regularize(const Quaternion &q) const {
	// return q or -q, whichever is closer to *this
	Quaternion qneg = q.scale(-1.0);
	return (dist(qneg) < dist(q) ? qneg : q);
}

double Quaternion::dist(const Quaternion &q) const {
	double dq[4] = {m_q[0] - q.m_q[0],
					m_q[1] - q.m_q[1],
					m_q[2] - q.m_q[2],
					m_q[3] - q.m_q[3]};
	return (sqrt(dq[0]*dq[0] + dq[1]*dq[1] + dq[2]*dq[2] + dq[3]*dq[3]));
}

Quaternion Quaternion::inverse() const {
	double norm = this->norm();
	double oon = (norm != 0) ? (1.0/norm) : 1.0;
	Quaternion q;
	q.m_q[0] =  m_q[0] *oon;
	q.m_q[1] = -m_q[1] *oon;
	q.m_q[2] = -m_q[2] *oon;
	q.m_q[3] = -m_q[3] *oon;
	return (q);
}

double Quaternion::norm() const {
	return (sqrt(m_q[0]*m_q[0] + m_q[1]*m_q[1] + m_q[2]*m_q[2] + m_q[3]*m_q[3]));
}

// multiplies q = this * r

Quaternion Quaternion::mult(const Quaternion &r) const {
	Quaternion q;
	q.m_q[0] = r.m_q[0]*m_q[0] - r.m_q[1]*m_q[1] - r.m_q[2]*m_q[2] - r.m_q[3]*m_q[3];
	q.m_q[1] = r.m_q[0]*m_q[1] + r.m_q[1]*m_q[0] - r.m_q[2]*m_q[3] + r.m_q[3]*m_q[2];
	q.m_q[2] = r.m_q[0]*m_q[2] + r.m_q[1]*m_q[3] + r.m_q[2]*m_q[0] - r.m_q[3]*m_q[1];
	q.m_q[3] = r.m_q[0]*m_q[3] - r.m_q[1]*m_q[2] + r.m_q[2]*m_q[1] + r.m_q[3]*m_q[0];
	return (q);
}

Quaternion Quaternion::scale(double h) const {
	Quaternion q;
	q.m_q[0] = m_q[0] * h;
	q.m_q[1] = m_q[1] * h;
	q.m_q[2] = m_q[2] * h;
	q.m_q[3] = m_q[3] * h;
	return (q);
}

Quaternion Quaternion::pow(double h) const {
	double theta = acos(m_q[0]);
	Quaternion q;
	if (fabs(theta) > 1e-8) {
		double sfac = sin(theta * h) / sin(theta);
		q.m_q[0] = cos(theta * h);
		q.m_q[1] = m_q[1] * sfac;
		q.m_q[2] = m_q[2] * sfac;
		q.m_q[3] = m_q[3] * sfac;
	}
	return (q);
}

Quaternion 
Quaternion::s_interpolate(const Quaternion &q1, const Quaternion &q2, double h) {
	Quaternion q2r = q1.regularize(q2);
	// SLERP interpolation between q1 and q2
	Quaternion qi = q1.mult(q1.inverse().mult(q2r).pow(h));
	return (qi);
}

std::ostream &operator<<(std::ostream &os, const Quaternion &q) {
	os << "[";
	for (int i = 0; i < 4; i++) {
		os << " " << q.m_q[i];
	}
	os << "]";
	return (os);
}
