#ifndef CAMERA_H
#define CAMERA_H

#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

const GLfloat SENSITIVITY = 0.005f;
const GLfloat SPEED = 3.0f;
const GLfloat ZOOM = 30.0f;
const float pi = 3.14;

enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

class camera {
public: 
	camera(glm::vec3 cameraPos, GLfloat yaw, GLfloat pitch ) {
		
		cameraPOSITION = cameraPos;
		YAW = yaw;
		PITCH = pitch;
		sensitivity = SENSITIVITY;
		speed = SPEED;
		zoom = ZOOM;
		r = sqrt(cameraPos.x*cameraPos.x + cameraPos.y * cameraPos.y + cameraPos.z * cameraPos.z);
		ksi = asin(sqrt(cameraPos.x * cameraPos.x + cameraPos.y * cameraPos.y) / r);
		psi = acos(cameraPos.z / (r * sin(ksi)));
		//cameraDIRECTION = glm::vec3(0.0f, 0.0f, -1.0f);
		cameraUpdater();
	};

	void mouseMovement(GLfloat xoffset, GLfloat yoffset) {
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		cameraPOSITION.x = r * sin(psi + xoffset)* sin(ksi + yoffset);
		cameraPOSITION.z = r * cos(ksi + yoffset);
		cameraPOSITION.y = r * cos(psi + xoffset) * sin(ksi + yoffset);
		psi += xoffset;
		psi = fmod(psi,2 * pi);
		if (psi < 0) psi += 2 * pi;
		ksi += yoffset;
		ksi = fmod(ksi, 2 * pi);
		if (ksi < 0) ksi += 2 * pi;

		//YAW += xoffset;
		//PITCH += yoffset;
		//if (PITCH > 89.0f)
		//	PITCH = 89.0f;
		//if (PITCH < -89.0f)
		//	PITCH = -89.0f;

		cameraUpdater();
	}

	void ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime)
	{
		GLfloat velocity = speed * deltaTime;
		if (direction == FORWARD)
			cameraPOSITION += cameraDIRECTION * velocity;
		if (direction == BACKWARD)
			cameraPOSITION -= cameraDIRECTION * velocity;
		if (direction == LEFT)
			cameraPOSITION -= cameraRIGHT * velocity;
		if (direction == RIGHT)
			cameraPOSITION += cameraRIGHT * velocity;
		cameraUpdater();
	}
	GLfloat YAW;
	GLfloat PITCH;
	GLfloat sensitivity;
	GLfloat speed;
	GLfloat zoom;
	GLfloat psi;
	GLfloat ksi;
	GLfloat r;
	glm::vec3 cameraPOSITION;
	glm::vec3 cameraDIRECTION;
	glm::vec3 cameraRIGHT;
	glm::vec3 cameraUP;
	glm::mat4 view;
	const glm::vec3 up = glm::vec3(0.0f, 0.0f, 1.0f);
	
private:
	void cameraUpdater() {
		// Calculate the new Front vector
		cameraDIRECTION.x = 0.0f - cameraPOSITION.x;
		cameraDIRECTION.y = 0.0f - cameraPOSITION.y;
		cameraDIRECTION.z = 0.0f - cameraPOSITION.z;

		cameraDIRECTION = glm::normalize(cameraDIRECTION);
		// Also re-calculate the Right and Up vector
		cameraRIGHT = glm::normalize(glm::cross(up, cameraDIRECTION));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		cameraUP = glm::normalize(glm::cross(cameraRIGHT, cameraDIRECTION));
		view = glm::lookAt(cameraPOSITION, cameraPOSITION + cameraDIRECTION, cameraUP);
	};
};
#endif