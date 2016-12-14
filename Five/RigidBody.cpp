#include "stdafx.h"
#include "RigidBody.h"
#include "AABB.h"



RigidBody::RigidBody(Model *model)
{
	object = model;
	mass = 1;
	ibody = glm::mat3(1.0);
	ibodyinv = glm::inverse(ibody);
	position = glm::vec3(1,0,0);
	rotation = 0;
	Iinv = glm::mat3(1.0);

	velocity = glm::vec3(0, 0, 0);
	omega = glm::vec3(0, 0, 0);

	force = glm::vec3(0, 0, 0);
	torque = glm::vec3(0, 0, 0);

	objectAABB = new AABB(*object->getAllVertexPos());

}


RigidBody::~RigidBody()
{
}

void RigidBody::MotionUpdate(float deltaTime)
{

}

void RigidBody::StateUpdate()
{
	auto cubeMeshes = object->getMeshes();
	for (auto it = (*cubeMeshes).begin(); it != (*cubeMeshes).end(); ++it) {
		auto meshVetex = &it->vertices;
		for (auto itV = (*meshVetex).begin();itV!=(*meshVetex).end();++itV)
		{
			itV->Position += glm::vec3(10, 0, 0);
		}
	}		
}

