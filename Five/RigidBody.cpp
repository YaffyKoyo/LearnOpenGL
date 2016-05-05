#include "stdafx.h"
#include "RigidBody.h"



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

}


RigidBody::~RigidBody()
{
}

void RigidBody::MotionUpdate(float deltaTime)
{

}

void RigidBody::StateUpdate()
{
	auto cubeVertices = object->getAllVertexPos();
	for (auto it = cubeVertices.begin(); it != cubeVertices.end(); ++it) {
		//*it *= omega;
		*it += position;
	}		
}

