#ifndef PLAYER_H
#define PLAYER_H

#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<stb/stb_image.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include"Model.h"
#include"Camera.h"

class Player 
{
public:
	Player(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f))
	{
		this->position = position;
		currentSpeed = 0.0f;
		currentTurnSpeed = 0.0f;
		model = Model("Models/wall.ply");
	}

	void move() {

	}

	void ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime)
	{
		GLfloat velocity = this->speed * deltaTime;
		if (direction == FORWARD) {
			this->currentSpeed = speed;
		}else if (direction == BACKWARD) {
			this->currentSpeed = -speed;
		}else {
			this->currentSpeed = 0;
		}
		if (direction == LEFT) {
			this->currentTurnSpeed = turnSpeed;
		}else if (direction == RIGHT) {
			this->currentTurnSpeed = - turnSpeed;
		}else {
			this->currentTurnSpeed = 0;
		}
	}

private:
	glm::vec3 position;
	float currentSpeed;	
	float currentTurnSpeed;
	float speed = 20.0f;
	float turnSpeed = 160.0f;
	Model model;
};

#endif