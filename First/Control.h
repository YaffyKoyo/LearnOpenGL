#pragma once

#include <glm/glm.hpp>


void computeMatricesFromInputs();
glm::mat4 getViewMatrix();
glm::mat4 getProjectionMatrix();
glm::mat4 getModelMatrixBall();
glm::mat4 getModelMatrixCube();


