#include "Entity.h"

Entity::Entity()
{
	position = glm::vec3(0);
	hasCollided = false;
	successfulLanding = false;
	speed = 0;
	width = 0;
	height = 0;
}

void Entity::Update(float deltaTime)
{
	velocity += acceleration * deltaTime;
	position.y += velocity.y * deltaTime;
	position.x += velocity.x * deltaTime;
}

void Entity::CheckCollision(Entity& object) {
	float xdist = fabs(position.x - object.position.x);
	float ydist = fabs(position.y - object.position.y);

	if (xdist - ((width + object.width) / 2) < 0 && ydist - (height + object.height) / 2 < 0) {
		hasCollided = true;
		if (object.type == GOAL) {
			successfulLanding = true;
		}
		float ypenetration = fabs(ydist - (height / 2) - (object.height / 2));
		if (velocity.y > 0) {
			position.y -= ypenetration;
		}
		else if (velocity.y < 0) {
			position.y += ypenetration;
		}
		float xpenetration = fabs(xdist - (width / 2) - (object.width / 2));
		if(velocity.x > 0) {
			position.x -= xpenetration;
		}
		else if (velocity.x < 0) {
			position.x += xpenetration;
		}
		velocity = glm::vec3(0);
		acceleration = glm::vec3(0);
	}
}

void Entity::Render(ShaderProgram* program) {
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, position);
	program->SetModelMatrix(modelMatrix);

	if (type == PLAYER) {
		float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
		float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

		glBindTexture(GL_TEXTURE_2D, textureID);

		glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
		glEnableVertexAttribArray(program->positionAttribute);

		glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
		glEnableVertexAttribArray(program->texCoordAttribute);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisableVertexAttribArray(program->positionAttribute);
		glDisableVertexAttribArray(program->texCoordAttribute);
	}
	else if (type == WALL) {
		float vertices[] = { -0.5, -5.0, 0.5, -5.0, 0.5, 5.0, 0.5, 5.0, -0.5, 5.0, -0.5, -5.0 };
		float texCoords[] = { 0.0, 5.0, 1.0, 5.0, 1.0, 0.0, 0.0, 5.0, 1.0, 0.0, 0.0, 0.0 };

		glBindTexture(GL_TEXTURE_2D, textureID);

		glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
		glEnableVertexAttribArray(program->positionAttribute);

		glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
		glEnableVertexAttribArray(program->texCoordAttribute);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisableVertexAttribArray(program->positionAttribute);
		glDisableVertexAttribArray(program->texCoordAttribute);
	}
	else if (type == GOAL) {
		float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
		float texCoords[] = { 0.0, 1.0, 2.0, 1.0, 2.0, 0.0, 0.0, 1.0, 2.0, 0.0, 0.0, 0.0 };

		glBindTexture(GL_TEXTURE_2D, textureID);

		glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
		glEnableVertexAttribArray(program->positionAttribute);

		glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
		glEnableVertexAttribArray(program->texCoordAttribute);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisableVertexAttribArray(program->positionAttribute);
		glDisableVertexAttribArray(program->texCoordAttribute);
	}
}
