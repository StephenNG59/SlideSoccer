#ifndef GAME_H
#define GAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include <stdlib.h>
#include <vector>

#include <MyClass/camera.h>
#include <MyClass/Shader/shader.h>
#include <MyClass/model.h>
#include <MyClass/Object/Object3D.h>
#include <MyClass/Object/Object3Dcube.h>
#include <MyClass/Object/Object3Dcylinder.h>
#include <MyClass/Object/Object3Dsphere.h>
#include <MyClass/Object/Collision.h>
#include <MyClass/Object/ParticleGenerator.h>
#include <MyClass/Skybox/Skybox.h>


enum GameState {
    GAME_ACTIVE = 0,
    GAME_MENU = 1,
    GAME_PLAYING = 2,
    GAME_WIN = 3
};

class Game
{
    
    public:
        // Game state
        GameState State;
        bool Keys[1024];
        unsigned int Width, Height;
        // Constructor/Deconstructor
        Game(unsigned int Width, unsigned int Height);
        ~Game();
        // Initialization
        void Init();
        // Gameloop
		void ProcessInput(float dt);
		void Update(float dt);
        void Render();
        // Camera
        Camera *GameCamera;
		// Players
		std::vector<Object3Dcylinder*> gamePlayers;
        // Shader
        Shader *gameShader;


    private:
		Shader *particleShader;
        // Balls
        std::vector<Object3Dsphere*> gameBalls;
        // Walls and Ground
        std::vector<Object3Dcube*> gameWalls;
		// Particle 
		ParticleGenerator* particleGenerator;
		// Model
		Model *model;

};

#endif