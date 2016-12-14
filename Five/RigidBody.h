#pragma once
#include "Model.h"
#include "AABB.h"

class RigidBody
{
public:
	RigidBody(Model *object);
	~RigidBody();
	
	Model *object;
	/*Constant quantities*/
	GLfloat		mass;			/* mass M*/							
	glm::mat3	ibody,			/* I_body*/
				ibodyinv;		/* inverse of I_body*/
								
	/*State variables*/			
	glm::vec3	position;		/* x(t)*/
	GLfloat		rotation;		/* R(t)*/
	glm::vec3	linearMomentum, /* P(t)*/
				angularMomentum;/* L(t)*/
								
	/*Derived quantities*/		
	glm::mat3	Iinv;			/* (I^-1)(t)*/
	glm::vec3	velocity,		/* v(t)*/
				omega;			/* w(t)*/
								
	/*Computed quantities*/		
	glm::vec3	force,			/* F(t)*/
				torque;			/* tau(t)*/
	
	AABB *objectAABB;

	void MotionUpdate(float deltaTime);
	void StateUpdate();

};

