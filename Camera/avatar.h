// -*-C++-*-

#pragma once

#include <string>
#include "camera.h"
#include "bsphere.h"
#include "trfm3D.h"

class Avatar {

public:

	/**
	 * Set avatar on walk or fly mode. walkOrFly true -> walk, false -> fly
	 *
	 * returns previous state
	 */
	bool walkOrFly(bool walkOrFly);

	/**
	 * Advance the avatar if possible (no collisions). Walk or fly depending on status.
	 * return true if avatar moves. false if not
	 */

	bool advance(float step);

	/**
	 * Look left/right (depending on sign of angle)
	 *
	 */

	void leftRight(float angle);

	/**
	 * Look up/down (depending on sign of angle)
	 *
	 */
	void upDown(float angle);

	void print() const;

	friend class AvatarManager;

private:
	Avatar();
	~Avatar();
	Avatar(const std::string &name, Camera * cam, float radius);
	Avatar(const Avatar &);
	Avatar &operator=(const Avatar &);

	std::string m_name; //!< Name of the Avatar
	Camera   *m_cam;    //!< Associated camera (not owned)
	BSphere  *m_bsph;   //!< Bounding sphere
	bool      m_walk;   // true: walk (default); false: fly
};
