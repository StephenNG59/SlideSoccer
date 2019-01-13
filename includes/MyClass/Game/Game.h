#ifndef GAME_H
#define GAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <irrklang\irrKlang.h>
using namespace irrklang;


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
	GAME_LOADING = -1,
    GAME_MAINMENU = 0,	/// Main menu
	GAME_HELP = 1,		/// Help menu
    GAME_PLAYING = 2,	/// Playing
    GAME_COOLDOWN = 3	/// Score and wait for reopen
};

enum MenuStateType {
	GAMEMENU_STARTGAME = 0,
	GAMEMENU_GAMEMODE = 1,
		GAMEMENU_GAMEMODE_NORMAL = 10,
		GAMEMENU_GAMEMODE_GHOST = 11,
		GAMEMENU_GAMEMODE_ICE = 12,
		GAMEMENU_GAMEMODE_GHOST_ICE = 13,
	GAMEMENU_SETTINGS = 2,
		GAMEMENU_SETTINGS_KICKERMODEL = 20,
		GAMEMENU_SETTINGS_EXPLODEBALL = 21,
		GAMEMENU_SETTINGS_FIREEFFECT = 22,
	GAMEMENU_HELP = 3,
		GAMEMENU_HELP_CONTENTS = 30,
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
		int MenuState = GAMEMENU_STARTGAME;
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


		void ShiftControlLeft(int param1);
		void ShiftControlRight(int param1);
		void ResetPosition();

		void initSound();

		// Sounds
		ISoundSource *bgm;
		ISoundSource *selectionSound;
		ISoundSource *ballExplosionSound;
		ISoundSource* collisionSound;

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
		Shader* particleInstanceShader;
		// Ball
		std::vector<Model*> gameBallModels;
		// Players
		std::vector<Object3Dcylinder*> gameKickers;
		std::vector<Model *> gameKickerModels;
        // Walls and Ground
        std::vector<Object3Dcube*> gameWalls;
		// Particle 
		ParticleGenerator* particleGenerator_tail_1;
		ParticleGenerator* particleGenerator_tail_2;
		ParticleGenerator* particleGenerator_tail_0;
		ParticleGenerator* particleGenerator_collide;
		ParticleGeneratorInstance* particleGeneratorInstance_tail_0;
		ParticleGeneratorInstance* particleGeneratorInstance_tail_1;
		ParticleGeneratorInstance* particleGeneratorInstance_explosion_0;
		ParticleGeneratorInstance* particleGeneratorInstance_explosion_1;
		ParticleGeneratorInstance* particleGeneratorInstance_ice;
		ParticleGeneratorInstance* pgi_explosion;
		void updateParticles(float dt);
		void initParticle();
		// Model
		Model *model;
		// Skybox
		void initSkybox();
		unsigned int cubemapTexture;
		// Shadow map
		unsigned int depthMap, depthMapFBO;
		void initShadow();
		// Status
		void updateStatus();
		void updateConfigure();
		void displayMenu();
		void updateModels(float dt);
		std::string GetKickerMenuText1();
};

#endif