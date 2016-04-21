#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
//using namespace std;
// GL Includes
#include <GL/glew.h> // Contains all the necessery OpenGL includes
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class AABB {
public:
	AABB(vector<glm::vec3> vertexPos) {
		GLfloat maxX, minX = maxX = vertexPos.front()[0];
		GLfloat maxY, minY = maxY = vertexPos.front()[1];
		GLfloat maxZ, minZ = maxZ = vertexPos.front()[2];
		for (auto it = vertexPos.begin(); it != vertexPos.end(); ++it)
		{
			maxX = std::max(maxX, (*it)[0]);
			maxY = std::max(maxY, (*it)[1]);
			maxZ = std::max(maxZ, (*it)[2]);

			minX = std::min(minX, (*it)[0]);
			minY = std::min(minY, (*it)[1]);
			minZ = std::min(minZ, (*it)[2]);
			
		}

		cubeFrameVertex[0] = glm::vec3(maxX, maxY, maxZ);
		cubeFrameVertex[1] = glm::vec3(maxX, maxY, minZ);
		cubeFrameVertex[2] = glm::vec3(maxX, minY, maxZ);
		cubeFrameVertex[3] = glm::vec3(maxX, minY, minZ);
		cubeFrameVertex[4] = glm::vec3(minX, maxY, maxZ);
		cubeFrameVertex[5] = glm::vec3(minX, maxY, minZ);
		cubeFrameVertex[6] = glm::vec3(minX, minY, maxZ);
		cubeFrameVertex[7] = glm::vec3(minX, minY, minZ);
		
		
		cubeLines.push_back(std::make_pair(cubeFrameVertex[0], cubeFrameVertex[1]));
		cubeLines.push_back(std::make_pair(cubeFrameVertex[1], cubeFrameVertex[3]));
		cubeLines.push_back(std::make_pair(cubeFrameVertex[3], cubeFrameVertex[2]));
		cubeLines.push_back(std::make_pair(cubeFrameVertex[2], cubeFrameVertex[0]));
		cubeLines.push_back(std::make_pair(cubeFrameVertex[4], cubeFrameVertex[5]));
		cubeLines.push_back(std::make_pair(cubeFrameVertex[5], cubeFrameVertex[7]));
		cubeLines.push_back(std::make_pair(cubeFrameVertex[7], cubeFrameVertex[6]));
		cubeLines.push_back(std::make_pair(cubeFrameVertex[6], cubeFrameVertex[4]));
		cubeLines.push_back(std::make_pair(cubeFrameVertex[0], cubeFrameVertex[4]));
		cubeLines.push_back(std::make_pair(cubeFrameVertex[1], cubeFrameVertex[5]));
		cubeLines.push_back(std::make_pair(cubeFrameVertex[3], cubeFrameVertex[7]));
		cubeLines.push_back(std::make_pair(cubeFrameVertex[2], cubeFrameVertex[6]));

	}

	void DrawAABB(glm::mat4 viewMatrix, glm::mat4 projectionMatrix) {

		glUseProgram(0);
		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf((const GLfloat*)&projectionMatrix[0]);
		glMatrixMode(GL_MODELVIEW);
		glm::mat4 MV = viewMatrix * glm::mat4(1);
		glLoadMatrixf((const GLfloat*)&MV[0]);

		glColor3f(1, 1, 0);
		glLineWidth(2);
		glBegin(GL_LINES);
		for (auto it = cubeLines.begin(); it !=cubeLines.end(); it++)
		{
			glm::vec3 first = (*it).first;
			glm::vec3 second = (*it).second;
			glVertex3f(first.x, first.y, first.z);
			glVertex3f(second.x, second.y, second.z);
		}
		glEnd();
	}
private:
	glm::vec3 cubeFrameVertex[8];
	vector<std::pair<glm::vec3,glm::vec3>> cubeLines;
};
