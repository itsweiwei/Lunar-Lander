#pragma once
#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

enum EntityType {PLAYER, GOAL, WALL, SPIKES, PLATFORM};

class Entity {
public:

	EntityType type;

	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 acceleration;

	float width;
	float height;

	bool hasCollided;
	bool successfulLanding;

	float speed;

	GLuint textureID;

	Entity();

	void Update(float deltaTime);
	void CheckCollision(Entity& entity);
	void Render(ShaderProgram* program);
};


