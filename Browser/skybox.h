// -*-C++-*-
#pragma once

#include <string>
#include "gObject.h"
#include "node.h"
#include "camera.h"

Node * CreateSkybox(const std::string &sbname,
					const std::string &dirname,
					const std::string &shaderName);
void DisplaySky(Node *sky, Camera *cam);

GObject *CreateSkyboxGeometry(const std::string & name,
							  Material *mat);
