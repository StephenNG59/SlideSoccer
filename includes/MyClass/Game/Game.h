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
#include <MyClass/Text/TextManager.h>
#include <MyClass/Player/Player.h>


enum GameStateType {
    GAME_MAINMENU = 0,	/// Main menu
	GAME_HELP = 1,		/// Help menu
    GAME_PLAYING = 2,	/// Playing
    GAME_COOLDOWN = 3	/// Score and wait for reopen
};


class Game
{
    
    public:

        // Constructor/Deconstructor
        Game(unsigned int Width, unsigned int Height);
        ~Game();

		// Game players
		Player * GamePlayers[2];

        // Game state
        GameStateType GameState;
        bool KeysCurrent[1024], KeysPressed[1024], KeysReleased[1024];
		unsigned int ViewportX = 0, ViewportY = 0;
        unsigned int ViewportW, ViewportH;

        // Initialization
        void Init();

        // Gameloop
		void ProcessInput(float dt);
		void Update(float dt);
		void RenderAll();
		void RenderWithDoubleCamera();
		void RenderScene(Shader *renderShader);
		void RenderWithShadow();
		void RenderInMainMenu();

		// Camera
        Camera *GameCamera;

        // Shader
        Shader *GameShader;
		Shader *DepthShader;
		Shader *TextShader;

        // Balls
        std::vector<Object3Dsphere*> GameBalls;
		// Skybox
		Skybox *GameSkybox;

		// Text
		TextManager *GameTextManager;


    private:

		// Camera
		void updateCameras(float dt);
		// Objects
		void createObjects();
		void updateObjects(float dt);
		// Lights position
		void initLights();
		void updateLights(float currentTime);
		glm::vec3 lightsPos[5];
		Shader *particleShader;
		// Players
		std::vector<Object3Dcylinder*> gameKickers;
        // Walls and Ground
        std::vector<Object3Dcube*> gameWalls;
		// Particle 
		ParticleGenerator* particleGenerator_tail;
		ParticleGenerator* particleGenerator_collide;
		// Model
		Model *model;
		// Skybox
		void initSkybox();
		// Shadow map
		unsigned int depthMap, depthMapFBO;
		void initShadow();
		// Status
		void updateStatus();
};

#endif