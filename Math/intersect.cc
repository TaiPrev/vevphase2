#include <cmath>
#include "intersect.h"
#include "constants.h"
#include "tools.h"

/* | algo           | difficulty | */
/* |----------------+------------| */
/* | BSPhereBSPhere |          1 | */
/* | BSPherePlane   |          2 | */
/* | BBoxBBox       |          3 | */
/* | BSphereBBox    |          5 | */
/* | BBoxPlane      |          5 | */


// @@ TODO: test if two BBoxes intersect.
//! Returns :
//    IINTERSECT intersect
//    IREJECT don't intersect

int  BBoxBBoxIntersect(const BBox *bba, const BBox *bbb ) {

	if (bba->m_min[0] > bbb->m_max[0] || bbb->m_min[0] > bba->m_max[0]) return IREJECT;
	if (bba->m_min[1] > bbb->m_max[1] || bbb->m_min[1] > bba->m_max[1]) return IREJECT;
	if (bba->m_min[2] > bbb->m_max[2] || bbb->m_min[2] > bba->m_max[2]) return IREJECT;

	return IINTERSECT;
}

// @@ TODO: test if a BBox and a plane intersect.
//! Returns :
//   +IREJECT outside
//   -IREJECT inside
//    IINTERSECT intersect

int  BBoxPlaneIntersect (const BBox *theBBox, Plane *thePlane) {

	Vector3 Vmin;
	Vector3 Vmax;

	int min_side;
	int max_side;

	for(size_t i = 0; i < 3 ; ++i) {
		if (thePlane->m_n[i] >= 0.0f) {
			Vmin[i] = theBBox->m_min[i];
			Vmax[i] = theBBox->m_max[i];
		} else {
			Vmin[i] = theBBox->m_max[i];
			Vmax[i] = theBBox->m_min[i];
		}
	}
	min_side = thePlane->whichSide(Vmin);
	if (min_side > 0) return IREJECT;  // Vmin on possitive side: bbox outside
	max_side = thePlane->whichSide(Vmax);
	if (max_side < 0) return -IREJECT;  // Vmax on negative side: bbox inside
	return IINTERSECT; // Intersecting
}


// @@ TODO: test if two BSpheres intersect.
//! Returns :
//    IREJECT don't intersect
//    IINTERSECT intersect

int BSphereBSphereIntersect(const BSphere *bsa, const BSphere *bsb ) {

	Vector3 v;
	v = bsa->m_centre - bsb->m_centre;
	float ls = v.dot(v);
	float rs = bsa->m_radius + bsb->m_radius;
	if (ls > (rs * rs)) return IREJECT; // Disjoint
	return IINTERSECT; // Intersect
}

// @@ TODO: test if a BSpheres intersects a plane.
//! Returns :
//   +IREJECT outside
//   -IREJECT inside
//    IINTERSECT intersect

int BSpherePlaneIntersect(const BSphere *bs, Plane *pl) {

	float dist = pl->signedDistance(bs->m_centre);
	if (fabs(dist) < bs->m_radius)
		return IINTERSECT; // Intersect
	if (dist < 0)
		return -IREJECT; // BSphere inside plane
	return +IREJECT; // BSphere outside plane
}

// @@ TODO: test if a BSpheres intersect a BBox.
//! Returns :
//    IREJECT don't intersect
//    IINTERSECT intersect

int BSphereBBoxIntersect(const BSphere *sphere, const BBox *box) {

	float d;
	float aux;
	float r;

	r = sphere->m_radius;
	d = 0;

	aux = sphere->m_centre[0] - box->m_min[0];
	if (aux < 0) {
		if (aux < -r) return IREJECT;
		d += aux*aux;
	} else {
		aux = sphere->m_centre[0] - box->m_max[0];
		if (aux > 0) {
			if (aux > r) return IREJECT;
			d += aux*aux;
		}
	}

	aux = (sphere->m_centre[1] - box->m_min[1]);
	if (aux < 0) {
		if (aux < -r) return IREJECT;
		d += aux*aux;
	} else {
		aux = sphere->m_centre[1] - box->m_max[1];
		if (aux > 0) {
			if (aux > r) return IREJECT;
			d += aux*aux;
		}
	}

	aux = sphere->m_centre[2] - box->m_min[2];
	if (aux < 0) {
		if (aux < -r) return IREJECT;
		d += aux*aux;
	} else {
		aux = sphere->m_centre[2] - box->m_max[2];
		if (aux > 0) {
			if (aux > r) return IREJECT;
			d += aux*aux;
		}
	}
	if (d > r * r) return IREJECT;
	return IINTERSECT;
}


int IntersectTriangleRay(const Vector3 & P0,
						 const Vector3 & P1,
						 const Vector3 & P2,
						 const Line *l,
						 Vector3 & uvw) {
	Vector3 e1(P1 - P0);
	Vector3 e2(P2 - P0);
	Vector3 p(crossVectors(l->m_d, e2));
	float a = e1.dot(p);
	if (fabs(a) < Constants::distance_epsilon) return IREJECT;
	float f = 1.0f / a;
	// s = l->o - P0
	Vector3 s(l->m_O - P0);
	float lu = f * s.dot(p);
	if (lu < 0.0 || lu > 1.0) return IREJECT;
	Vector3 q(crossVectors(s, e1));
	float lv = f * q.dot(l->m_d);
	if (lv < 0.0 || lv > 1.0) return IREJECT;
	uvw[0] = lu;
	uvw[1] = lv;
	uvw[2] = f * e2.dot(q);
	return IINTERSECT;
}

/* IREJECT 1 */
/* IINTERSECT 0 */

const char *intersect_string(int intersect) {

	static const char *iint = "IINTERSECT";
	static const char *prej = "IREJECT";
	static const char *mrej = "-IREJECT";
	static const char *error = "IERROR";

	const char *result = error;

	switch (intersect) {
	case IINTERSECT:
		result = iint;
		break;
	case +IREJECT:
		result = prej;
		break;
	case -IREJECT:
		result = mrej;
		break;
	}
	return result;
}
