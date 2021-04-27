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
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Entity.h"


SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

struct GameState {
	Entity player;
	Entity tile;
	Entity left_wall;
	Entity right_wall;
	Entity spikes;
};

GameState state;

GLuint LoadTexture(const char* filePath) {
	int w, h, n;
	unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);

	if (image == NULL) {
		std::cout << "Unable to load image. Make sure the path is correct\n";
		assert(false);
	}

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	stbi_image_free(image);
	return textureID;
}

void DrawText(ShaderProgram* program, GLuint fontTextureID, std::string text, float size, float spacing, glm::vec3 position) {
	float width = 1.0f / 16.0f;
	float height = 1.0f / 16.0f;

	std::vector<float> vertices;
	std::vector<float> texCoords;

	for (int i = 0; i < text.size(); ++i) {
		int index = (int)text[i];
		float u = (float)(index % 16) / 16.0f;
		float v = (float)(index / 16) / 16.0f;

		texCoords.insert(texCoords.end(), { u, v + height, u + width, v + height, u + width , v, u,
			v + height, u  + width, v, u, v});

		float offset = (size + spacing) * i;
		vertices.insert(vertices.end(), { offset + (-0.5f * size), (-0.5f * size), offset + (0.5f * size), (-0.5f * size),
			offset + (0.5f * size), (0.5f * size), offset + (-0.5f * size), (-0.5f * size), offset + (0.5f * size), (0.5f * size),
			offset + (-0.5f * size), (0.5f * size) });
	}
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, position);
	program->SetModelMatrix(modelMatrix);

	glBindTexture(GL_TEXTURE_2D, fontTextureID);

	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices.data());
	glEnableVertexAttribArray(program->positionAttribute);

	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords.data());
	glEnableVertexAttribArray(program->texCoordAttribute);

	glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 2.0f);

	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
}

void Initialize() {
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Lunar Lander", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
	glewInit();
#endif

	glViewport(0, 0, 640, 480);

	program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

	state.player.textureID = LoadTexture("spaceship.png");
	state.player.type = PLAYER;
	state.player.position = glm::vec3(0, 2, 0);
	state.player.acceleration = glm::vec3(0, -0.05f, 0);
	state.player.width = 1.0f;
	state.player.height = 1.0f;

	state.tile.textureID = LoadTexture("platform.png");
	state.tile.type = GOAL;
	state.tile.position = glm::vec3(0, -3.25f , 0);
	state.tile.height = 1.0f;
	state.tile.height = 1.0f;

	state.left_wall.textureID = LoadTexture("tile.jpg");
	state.left_wall.type = WALL;
	state.left_wall.position = glm::vec3(-4.5f, 0, 0);
	state.left_wall.height = 5.0f;
	state.left_wall.width = 1.0f;

	state.right_wall.textureID = LoadTexture("tile.jpg");
	state.right_wall.type = WALL;
	state.right_wall.position = glm::vec3(4.5f, 0, 0);
	state.right_wall.height = 5.0f;
	state.right_wall.width = 1.0f;

	viewMatrix = glm::mat4(1.0f);
	modelMatrix = glm::mat4(1.0f);
	projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

	program.SetProjectionMatrix(projectionMatrix);
	program.SetViewMatrix(viewMatrix);
	program.SetColor(1.0f, 1.0f, 1.0f, 1.0f);

	glUseProgram(program.programID);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
}

void ProcessInput() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
		case SDL_WINDOWEVENT_CLOSE:
			gameIsRunning = false;
			break;

		case SDL_KEYUP:
			switch (event.key.keysym.sym) {
			case SDLK_LEFT:
				state.player.acceleration.x = 0.0f;
				break;
			case SDLK_RIGHT:
				state.player.acceleration.x = 0.0f;
				break;
			}
			break;
		}

		const Uint8* keys = SDL_GetKeyboardState(NULL);
		if (keys[SDL_SCANCODE_LEFT] && !state.player.hasCollided) {
			state.player.acceleration.x = -0.75f;
		}
		else if (keys[SDL_SCANCODE_RIGHT] && !state.player.hasCollided) {
			state.player.acceleration.x = 0.75f;
		}
	}
}

#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;

void Update() {
	float ticks = (float)SDL_GetTicks() / 1000.0f;
	float deltaTime = ticks - lastTicks;
	lastTicks = ticks;

	deltaTime += accumulator;
	if (deltaTime < FIXED_TIMESTEP) {
		accumulator = deltaTime;
		return;
	}

	while (deltaTime >= FIXED_TIMESTEP) {
		state.player.Update(FIXED_TIMESTEP);
		state.player.CheckCollision(state.tile);
		state.player.CheckCollision(state.left_wall);
		deltaTime -= FIXED_TIMESTEP;
	}

	accumulator = deltaTime;
}


void Render() {
	glClear(GL_COLOR_BUFFER_BIT);

	state.player.Render(&program);
	state.tile.Render(&program);
	state.left_wall.Render(&program);
	state.right_wall.Render(&program);

	if (state.player.hasCollided) {
		if (state.player.successfulLanding) {
			DrawText(&program, LoadTexture("alpha.png"), "Mission Successful!", 0.5f, 0.0f, glm::vec3(-4.25f, 0, 0));
		}
		else {
			DrawText(&program, LoadTexture("alpha.png"), "Mission Failed...", 0.5f, 0.0f, glm::vec3(-4.0f, 0, 0));
		}
	}

	SDL_GL_SwapWindow(displayWindow);
}

void Shutdown() {
	SDL_Quit();
}

int main(int argc, char* argv[]) {
	Initialize();

	while (gameIsRunning) {
		ProcessInput();
		Update();
		Render();
	}

	Shutdown();
	return 0;
}