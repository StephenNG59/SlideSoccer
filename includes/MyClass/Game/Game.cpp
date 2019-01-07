#include "pch.h"
#include "Game.h"

Object3Dcube ground(glm::vec3(GROUND_WIDTH, GROUND_HEIGHT, GROUND_DEPTH));
Object3Dcube wall_e(glm::vec3(WALL_THICK, WALL_HEIGHT, GROUND_DEPTH));
Object3Dcube wall_w(glm::vec3(WALL_THICK, WALL_HEIGHT, GROUND_DEPTH));
Object3Dcube wall_n(glm::vec3(GROUND_WIDTH, WALL_HEIGHT, WALL_THICK));
Object3Dcube wall_s(glm::vec3(GROUND_WIDTH, WALL_HEIGHT, WALL_THICK));
Object3Dcube wall_center(glm::vec3(20, WALL_HEIGHT + 10, 2));
//Object3Dcube cube(glm::vec3(1.0f, 1.0f, 1.0f));
//Object3Dsphere ball(0.5f, 32, 20);
//Object3Dsphere ball3(0.4f, 32, 20);

// Screen
extern unsigned int screenWidth, screenHeight;

// Time
float currentTime = 0;
float timeFromLastCollide = 0;

// Light
float lightRadius = 5.0f;

// Sensitivity
extern float keySensitivity;

// Sound
ISoundEngine *SoundEngine = createIrrKlangDevice();


Game::Game(unsigned int screenWidth, unsigned int screenHeight) 
    : GameState(GAME_MAINMENU), KeysPressed(), ViewportW(screenWidth), ViewportH(screenHeight)
{

}

Game::~Game()
{
	delete(GameBalls[0]);
	for (int i = 0; i < gameKickers.size(); i++)
		delete(gameKickers[i]);
	delete(model);
	// TODO: delete all stuff

}


void Game::Init()
{

	// Players
	// -------
	GamePlayers[0] = new Player(0, "Name1");
	GamePlayers[1] = new Player(1, "Name2");

    // build and compile our shader program
	// ------------------------------------
    GameShader = new Shader("shaders/game/gameVS.glsl", "shaders/game/gameFS.glsl", "shaders/game/gameGS.glsl");
	particleShader = new Shader("shaders/particle/particleVS.glsl", "shaders/particle/particleFS.glsl");
	particleInstanceShader = new Shader("shaders/particle/particleInstanceVS.glsl", "shaders/particle/particleInstanceFS.glsl");
	DepthShader = new Shader("shaders/depth/depthVS.glsl", "shaders/depth/depthFS.glsl");
	TextShader = new Shader("shaders/text/textVS.glsl", "shaders/text/textFS.glsl");

	// ------------------------------------
    GameCamera = new Camera(CAMERA_POS_1, CAMERA_CENTER_1);
    GameCamera->SetPerspective(glm::radians(45.0f), (float)ViewportW / (float)ViewportH, CAMERA_ZNEAR, CAMERA_ZFAR);

	stbi_set_flip_vertically_on_load(true);

	// -- Objects --
	createObjects();

	// -- Lights --
	initLights();

	initParticle();

	// model
	//model = new Model("resources/objects/nanosuit/nanosuit.obj");
	//model = new Model("resources/objects/ball/1212.obj");
	//model = new Model("resources/objects/grass/grass.obj");
	model = new Model("resources/objects/scene/slidesoccer_scene.obj");

	// Skybox
	initSkybox();


	// Shadow
	this->initShadow();

	// Text manager
	GameTextManager = new TextManager(*TextShader);

	// Sound
	SoundEngine->play2D("resources/audio/BGClip.mp3", true);

}

float cameraCDtime = 0.1f;
float cameraFromLastMove = 0;
void Game::Update(float dt)
{
	currentTime += dt;

	updateCameras(dt);

	updateObjects(dt);
	
	updateLights(currentTime);

	updateParticles(dt);

	updateStatus();

}

void Game::RenderAll()
{
	if (GameState == GameStateType::GAME_MAINMENU)
		RenderInMainMenu();
	else if (GameState == GameStateType::GAME_PLAYING)
		RenderWithDoubleCamera();
	else if (GameState == GameStateType::GAME_COOLDOWN)
		RenderWithDoubleCamera();

	//GameTextManager->Render(*TextShader);
	
}

void Game::RenderWithDoubleCamera()
{

	// Left side
	// ---------
	ViewportW = 0.5 * screenWidth;
	ViewportH = screenHeight;
	glm::vec3 pos = gameKickers[GamePlayers[0]->CurrentControl]->GetPosition();
	glm::vec3 eye(pos.x - (CAMERA_LEAN_OFFSET2), CAMERA_POS_3_Y, pos.z * 1.25);
	GameCamera->SetPosition(eye, pos, CAMERA_UPVECNORM_Y);
	RenderWithShadow();
	GameTextManager->RenderText(*TextShader, std::to_string(GamePlayers[0]->GetScore()), 0.25 * screenWidth, 0.9 * screenHeight, 1.5f, PARTICLE_COLOR_RED);


	// Right side
	// ----------
	ViewportX = 0.5 * screenWidth;
	ViewportY = 0;
	pos = gameKickers[GamePlayers[1]->CurrentControl]->GetPosition();
	eye = glm::vec3(pos.x + CAMERA_LEAN_OFFSET1, CAMERA_POS_2_Y, pos.z);
	GameCamera->SetPosition(eye, pos, -CAMERA_UPVECNORM_X);
	RenderWithShadow();
	ViewportX = 0;
	ViewportY = 0;
	GameTextManager->RenderText(*TextShader, std::to_string(GamePlayers[1]->GetScore()), 0.25 * screenWidth, 0.9 * screenHeight, 1.5f, PARTICLE_COLOR_BLUE);
}

void Game::RenderScene(Shader *renderShader)
{

	for (std::vector<Object3Dsphere*>::iterator it = GameBalls.begin(); it < GameBalls.end(); it++)
	{
		(*it)->Draw(*GameCamera, *renderShader);
	}
    for (std::vector<Object3Dcube*>::iterator it = gameWalls.begin(); it < gameWalls.end(); it++)
    {
		//(*it)->Draw(*GameCamera, *renderShader);
		(*it)->DrawWithoutCamera(*renderShader);
    }
	for (std::vector<Object3Dcylinder*>::iterator it = gameKickers.begin(); it < gameKickers.end(); it++)
	{
		//(*it)->Draw(*GameCamera, *renderShader);
		(*it)->DrawWithoutCamera(*renderShader);
	}
	//ground.Draw(*GameCamera, *renderShader);

	particleGenerator_tail_0->Draw();
	particleGenerator_tail_1->Draw();
	particleGenerator_collide->Draw();


	GameShader->setFloat("material.shininess", 32);
	//model->Draw(*GameCamera, *GameShader, glm::scale(ground.GetModelMatrix(), glm::vec3(8.0f)));
	model->DrawWithoutCamera(*GameShader, glm::scale(ground.GetModelMatrix(), glm::vec3(9.5f)));

	GameSkybox->Draw(*GameCamera);

	// #NOTE this should be at last because it has transparent texture
	//if (particleGeneratorInstance_tail_0->IsActive)
		particleGeneratorInstance_tail_0->Draw(GameCamera);
	//if (particleGeneratorInstance_tail_1->IsActive)
		particleGeneratorInstance_tail_1->Draw(GameCamera);
	//if (particleGeneratorInstance_explosion_0->IsActive)
		particleGeneratorInstance_explosion_0->Draw(GameCamera);

}

void Game::RenderWithShadow()
{
	// 1. Render depth of scene to texture (from light's perspective)
	// - Get light projection/view matrix.
	glm::mat4 lightProjection, lightView;
	glm::mat4 lightSpaceMatrix;
	glm::vec3 lightPos = glm::vec3(10, 50, 0);
	GLfloat near_plane = 1.0f, far_plane = vecMod(lightPos) * 2;
	lightProjection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, near_plane, far_plane);
	lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
	lightSpaceMatrix = lightProjection * lightView;
	// - now render scene from light's point of view
	DepthShader->use();
	DepthShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		RenderScene(DepthShader);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// 2. Render scene as normal 
	glViewport(ViewportX, ViewportY, ViewportW, ViewportH);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// comment this to support two viewports
	GameCamera->SetPerspective(GameCamera->Fov, (float)ViewportW / (float)ViewportH, CAMERA_ZNEAR, CAMERA_ZFAR);
	GameShader->use();
	// Set light uniforms
	GameShader->setVec3("shadowLightPos", lightPos);
	GameShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);

	int shadowMapID = SHADOW_MAP_ID;
	GameShader->setInt("shadowMap", shadowMapID);
	glActiveTexture(GL_TEXTURE0 + shadowMapID);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	RenderScene(GameShader);

}

void Game::RenderInMainMenu()
{
	extern float currentFrame;

	ViewportX = ViewportY = 0;
	ViewportH = screenHeight;
	ViewportW = screenWidth;
	RenderWithShadow();

	GameTextManager->RenderText(*TextShader, "Slide Soccer", 0.5 * screenWidth, 0.5 * screenHeight, 1.5f, glm::vec3(0.5, 0.4f, 0.6f));
	GameTextManager->RenderText(*TextShader, "press -Enter- to begin", 0.5 * screenWidth, 0.4 * screenHeight, 0.7f, glm::vec3(0.3, 0.2f, 0.4f));

}

extern float gameCoolDown;
void Game::ProcessInput(float dt)
{

	if (this->KeysPressed[GLFW_KEY_ESCAPE])
	{
		if (GameState == GameStateType::GAME_PLAYING || GameState == GameStateType::GAME_HELP || GameState == GameStateType::GAME_COOLDOWN)
		{
			GameState = GameStateType::GAME_MAINMENU;
			GamePlayers[0]->ResetScore();
			GamePlayers[1]->ResetScore();
			GameTextManager->UpdateAspect(screenWidth, screenHeight);
			GameCamera->SetTrackingTarget(CameraTrackingTarget::Ball);
			GameCamera->SmoothlyMoveTo(CAMERA_POS_1, CAMERA_CENTER_1, CAMERA_UPVECNORM_Y, CAMERA_SMOOTHMOVING_TIME);
		}
	}
	if (this->KeysPressed[GLFW_KEY_ENTER])
	{
		if (GameState == GameStateType::GAME_MAINMENU)
		{
			GameState = GameStateType::GAME_PLAYING;
			GamePlayers[0]->ResetScore();
			GamePlayers[1]->ResetScore();
			ResetPosition();

			GameTextManager->UpdateAspect(0.5 * screenWidth, screenHeight);
			// TODO: reset position here.
			GameCamera->SetTrackingTarget(CameraTrackingTarget::Player1);
			GameCamera->SmoothlyMoveTo(CAMERA_POS_2, CAMERA_CENTER_2, CAMERA_UPVECNORM_X, CAMERA_SMOOTHMOVING_TIME);

		}
	}

	if (GameState == GameStateType::GAME_COOLDOWN)
	{
		gameCoolDown -= dt;
		if (gameCoolDown <= 0)
		{
			// TODO: reset objects positions here.
			ResetPosition();

			GameState = GameStateType::GAME_PLAYING;
			GameBalls[0]->SetBallStatus(BallStatus::BallIsFree);
			gameCoolDown = GAME_COOLDOWN_TIME;
		}
	}

	if (GameState != GameStateType::GAME_COOLDOWN)
	{

		// Left player
		if (this->KeysCurrent[GLFW_KEY_W])
		{
			this->gameKickers[GamePlayers[0]->CurrentControl]->AddVelocity(glm::vec3(ACCELERATION_BASIC, 0, 0) * dt);
		}
		if (this->KeysCurrent[GLFW_KEY_S])
		{
			this->gameKickers[GamePlayers[0]->CurrentControl]->AddVelocity(glm::vec3(-ACCELERATION_BASIC, 0, 0) * dt);
		}
		if (this->KeysCurrent[GLFW_KEY_A])
		{
			this->gameKickers[GamePlayers[0]->CurrentControl]->AddVelocity(glm::vec3(0, 0, -ACCELERATION_BASIC) * dt);
		}
		if (this->KeysCurrent[GLFW_KEY_D])
		{
			this->gameKickers[GamePlayers[0]->CurrentControl]->AddVelocity(glm::vec3(0, 0, ACCELERATION_BASIC) * dt);
		}

		if (this->KeysPressed[GLFW_KEY_Q])
		{
			ShiftControlLeft(0);
		}
		if (this->KeysPressed[GLFW_KEY_E])
		{
			ShiftControlRight(0);
		}

		// Right player
		if (this->KeysCurrent[GLFW_KEY_UP])
		{
			this->gameKickers[GamePlayers[1]->CurrentControl]->AddVelocity(glm::vec3(-ACCELERATION_BASIC, 0, 0) * dt);
		}
		if (this->KeysCurrent[GLFW_KEY_DOWN])
		{
			this->gameKickers[GamePlayers[1]->CurrentControl]->AddVelocity(glm::vec3(ACCELERATION_BASIC, 0, 0) * dt);
		}
		if (this->KeysCurrent[GLFW_KEY_LEFT])
		{
			this->gameKickers[GamePlayers[1]->CurrentControl]->AddVelocity(glm::vec3(0, 0, ACCELERATION_BASIC) * dt);
		}
		if (this->KeysCurrent[GLFW_KEY_RIGHT])
		{
			this->gameKickers[GamePlayers[1]->CurrentControl]->AddVelocity(glm::vec3(0, 0, -ACCELERATION_BASIC) * dt);
		}

		if (this->KeysPressed[GLFW_KEY_RIGHT_SHIFT])
		{
			ShiftControlLeft(1);
		}
		if (this->KeysPressed[GLFW_KEY_PAGE_DOWN])
		{
			ShiftControlRight(1);
		}


		if (this->KeysCurrent[GLFW_KEY_J])
		{
			this->gameKickers[0]->AddOmega(glm::vec3(0, 0.2f, 0));
		}
		if (this->KeysCurrent[GLFW_KEY_K])
		{
			this->gameKickers[0]->AddOmega(glm::vec3(0, -0.2f, 0));
		}
	}

	if (this->KeysCurrent[GLFW_KEY_EQUAL])
	{
		GameCamera->GoForward(keySensitivity);
	}
	if (this->KeysCurrent[GLFW_KEY_MINUS])
	{
		GameCamera->GoForward(-keySensitivity);
	}

	if (this->KeysPressed[GLFW_KEY_F1])
	{
		GameCamera->Status = CameraStatus::CameraIsFree;
		GameCamera->SmoothlyMoveTo(CAMERA_POS_1, CAMERA_CENTER_1, CAMERA_UPVECNORM_Y, CAMERA_SMOOTHMOVING_TIME);
	}
	if (this->KeysPressed[GLFW_KEY_F2])
	{
		GameCamera->SmoothlyMoveTo(CAMERA_POS_2, CAMERA_CENTER_2, -CAMERA_UPVECNORM_Z, CAMERA_SMOOTHMOVING_TIME);
	}
	if (this->KeysPressed[GLFW_KEY_F3])
	{
		GameCamera->SmoothlyMoveTo(CAMERA_POS_2, CAMERA_CENTER_2, CAMERA_UPVECNORM_X, CAMERA_SMOOTHMOVING_TIME);
	}
	if (this->KeysPressed[GLFW_KEY_F4])
	{
		GameCamera->SmoothlyMoveTo(CAMERA_POS_2, CAMERA_CENTER_2, -CAMERA_UPVECNORM_X, CAMERA_SMOOTHMOVING_TIME);
	}
	if (this->KeysPressed[GLFW_KEY_GRAVE_ACCENT])	/// '`'
	{
		GameCamera->SetTrackingTarget(CameraTrackingTarget::NoTracking);
	}
	if (this->KeysPressed[GLFW_KEY_0])
	{
		GameCamera->SetTrackingTarget(CameraTrackingTarget::Ball);
	}
	if (this->KeysPressed[GLFW_KEY_1])
	{
		GameCamera->SetTrackingTarget(CameraTrackingTarget::Player1);
	}
	if (this->KeysPressed[GLFW_KEY_2])
	{
		GameCamera->SetTrackingTarget(CameraTrackingTarget::Player2);
	}
	if (this->KeysReleased[GLFW_KEY_F5])
	{
		for (Object3Dcylinder * P : gameKickers)
		{
			P->SetStatic();
		}
	}
	if (this->KeysPressed[GLFW_KEY_B])
	{
		GameBalls[0]->StartExplosion(3, glm::vec3(0, -5.0, 0));
	}

	for (int i = 0; i < 1024; i++)
	{
		KeysPressed[i] = false;
		KeysReleased[i] = false;
	}

}


// -- Initialization Functions --
// ------------------------------

void Game::createObjects()
{
	glm::vec3 groundPos = glm::vec3(0, -5.0f, 0.0f);
	// ground
	ground.SetPosition(groundPos);		// this is useful when drawing pitch
	//ground.SetMass(0);
	//ground.SetFriction(0.8f);
	// wall-e
	wall_e.SetPosition(groundPos + glm::vec3(0.5f * GROUND_WIDTH, 0.5f * WALL_HEIGHT + 0.5f * GROUND_HEIGHT, 0));
	wall_e.IsGoal1 = true;
	// wall-w
	wall_w.SetPosition(groundPos + glm::vec3(-0.5f * GROUND_WIDTH, 0.5f * WALL_HEIGHT + 0.5f * GROUND_HEIGHT, 0));
	wall_w.IsGoal2 = true;
	// wall-n
	wall_n.SetPosition(groundPos + glm::vec3(0, 0.5f * WALL_HEIGHT + 0.5f * GROUND_HEIGHT, -0.5f * GROUND_DEPTH));
	// wall-s
	wall_s.SetPosition(groundPos + glm::vec3(0, 0.5f * WALL_HEIGHT + 0.5f * GROUND_HEIGHT, 0.5f * GROUND_DEPTH));
	// wall-center
	wall_center.SetPosition(groundPos + glm::vec3(0, 0.5f * WALL_HEIGHT + 0.5f * GROUND_HEIGHT, 0));
	wall_center.SetOmega(glm::vec3(0, 0.2, 0));

	GameBalls.push_back(new Object3Dsphere(1.3f, 20, 16));
	GameBalls[0]->IsBall = true;
	GameBalls[0]->AddTexture("resources/textures/awesomeface.png", ObjectTextureType::Emission);
	GameBalls[0]->SetPosition(glm::vec3(0, 0, 0));
	GameBalls[0]->SetERestitution(0.5f);
	GameBalls[0]->SetGravity(glm::vec3(0, 0, 0));

	for (int i = 0; i < 6; i++)
	{
		gameKickers.push_back(new Object3Dcylinder(1.5f, 1.5f, 0.2f, 20));
		//glm::vec3 pos = glm::vec3(i * 3 - 12, -4.8, i * 2 - 9);
		glm::vec3 pos = glm::vec3(i * 3 - 12, 0, i * 2 - 9);
		gameKickers[i]->SetPosition(pos);
		gameKickers[i]->AddTexture("resources/textures/awesomeface.png", ObjectTextureType::Emission);
		gameKickers[i]->SetFriction(1.0f);
		//gameKickers[i]->SetVelocity(glm::vec3(rand() % 50 - 25, 0, rand() % 40 - 20) / 10.0f);
		gameKickers[i]->SetOmega(glm::vec3(0, rand() % 20, 0));
		gameKickers[i]->SetGravity(glm::vec3(0, 0, 0));
		gameKickers[i]->SetERestitution(1.0f);
		gameKickers[i]->SetLinearFriction(FRICTION_LINEAR);
		gameKickers[i]->SetConstantFriction(FRICTION_CONSTANT);
	}
	gameKickers[0]->SetVelocity(glm::vec3(1.5f, 0, 2.5f));
	gameKickers[0]->SetOmega(glm::vec3(0, 20.0f, 0));
	//gameKickers[0]->AddModel("resources/objects/ball/1212.obj");
	//gameKickers[0]->AddModel("resources/objects/nanosuit/nanosuit.obj");
	//gameKickers[0]->AddModel("resources/objects/ball/pumpkin_01.obj");
	GameBalls[0]->AddModel("resources/objects/ball/pumpkin_01.obj", glm::vec3(0.03f));

	gameWalls.push_back(&wall_e);
	gameWalls.push_back(&wall_w);
	gameWalls.push_back(&wall_n);
	gameWalls.push_back(&wall_s);
	//gameWalls.push_back(&wall_center);
	for (Object3Dcube * gameWall : gameWalls)
	{
		gameWall->SetMass(0);
		//gameWall->SetERestitution(1.2f);		// dangerous
	}


}

void Game::initLights()
{
	// point light 0
	GameShader->use();		// don't forget to do this !!!!!!!!
	GameShader->setBool("pointLights[0].isExist", true);
	GameShader->setFloat("pointLights[0].constant", 1.0f);
	GameShader->setFloat("pointLights[0].linear", 0.09);
	GameShader->setFloat("pointLights[0].quadratic", 0.032);
	// direction light 0
	GameShader->setBool("dirLights[0].isExist", true);
	GameShader->setVec3("dirLights[0].direction", -1.0, -1.0, 0);
	GameShader->setVec3("dirLights[0].ambient", 0.05, 0.05, 0.1);
	GameShader->setVec3("dirLights[0].diffuse", 0.3, 0.3, 0.35);
	GameShader->setVec3("dirLights[0].specular", 1.0, 1.0, 1.0);
}

void Game::initSkybox()
{
	// left right top bottom back front
	std::vector<std::string> facesPath1 = {
		"resources/textures/skybox/1/right.jpg",
		"resources/textures/skybox/1/left.jpg",
		"resources/textures/skybox/1/top.jpg",
		"resources/textures/skybox/1/bottom.jpg",
		"resources/textures/skybox/1/front.jpg",
		"resources/textures/skybox/1/back.jpg"
	};
	std::vector<std::string> facesPath2 = {
		"resources/textures/skybox/2/back.tga",		// left
		"resources/textures/skybox/2/front.tga",	// right
		"resources/textures/skybox/2/top.tga",		// top
		"resources/textures/skybox/2/bottom.tga",	// bottom
		"resources/textures/skybox/2/left.tga",		// back
		"resources/textures/skybox/2/right.tga",	// front
	};
	std::vector<std::string> facesPath3 = {
		"resources/textures/skybox/3/left.tga",
		"resources/textures/skybox/3/right.tga",
		"resources/textures/skybox/3/top.tga",
		"resources/textures/skybox/3/bottom.tga",
		"resources/textures/skybox/3/back.tga",
		"resources/textures/skybox/3/front.tga",
	};
	std::vector<std::string> facesPath5 = {
		"resources/textures/skybox/5/L.jpg",
		"resources/textures/skybox/5/r.jpg",
		"resources/textures/skybox/5/u.jpg",
		"resources/textures/skybox/5/d.jpg",
		"resources/textures/skybox/5/b.jpg",
		"resources/textures/skybox/5/F.jpg",
	};

	std::vector<std::string> shadersPath = {
		"shaders/skybox/skyboxVS.glsl", "shaders/skybox/skyboxFS.glsl"
	};
	GameSkybox = new Skybox(facesPath2, shadersPath);
}

// Generate a depth texture, stored in depthMap
void Game::initShadow()
{
	// Configure depth map FBO
	glGenFramebuffers(1, &depthMapFBO);
	// Create depth texture
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	// Use our generated depth texture (depthMap) as the depth buffer of the frame buffer (depthMapFBO)
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	// Tell OpenGL not to use any color data in frame buffer to render
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	// Unbind frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void Game::initParticle()
{
	// particle generator
	particleGenerator_tail_0 = new ParticleGenerator(particleShader, GameCamera, 500, PARTICLE_COLOR_RED);
	particleGenerator_tail_1 = new ParticleGenerator(particleShader, GameCamera, 500, PARTICLE_COLOR_BLUE);
	particleGenerator_collide = new ParticleGenerator(particleShader, GameCamera, 200, PARTICLE_COLOR_GREEN);
	particleGeneratorInstance_tail_0 = new ParticleGeneratorInstance(particleInstanceShader, "resources/textures/particle/ParticleAtlas.png", 8, 8);
	particleGeneratorInstance_tail_1 = new ParticleGeneratorInstance(particleInstanceShader, "resources/textures/particle/ParticleAtlas-BreakingGlass.png", 4, 8);
	particleGeneratorInstance_explosion_0 = new ParticleGeneratorInstance(particleInstanceShader, "resources/textures/particle/ParticleAtlas-Explosion.png", 4, 4);
	particleGeneratorInstance_explosion_0->SetGravity(glm::vec3(0, 0, 0));
}

// -- Update Functions --
// ----------------------

void Game::updateCameras(float dt)
{
	if (GameState == GameStateType::GAME_MAINMENU)
	{
		//GameCamera->RotateRightByDegree(dt * 10);
		if (GameCamera->Status == CameraStatus::CameraIsFree)
			GameCamera->RotateCounterClockByDegree(dt * 30, glm::vec3(0, 1, 0));
	}

	// Camera tracks target
	if (GameCamera->Status == CameraStatus::IsTracking)
	{

		CameraTrackingTarget target = GameCamera->GetTarget();

		glm::vec3 pos;
		if (target == CameraTrackingTarget::NoTracking)
		{
			GameCamera->Status = CameraStatus::CameraIsFree;
		}
		if (target == CameraTrackingTarget::Ball)
		{
			pos = GameBalls[0]->GetPosition();
		}
		else
		{
			pos = gameKickers[target - 1]->GetPosition();
		}

		GameCamera->TranslateTo(pos);
	}

	GameCamera->Update(dt);
}

void Game::updateObjects(float dt)
{
	//CollideSph2Ground(gameBalls, &ground);
	CollideSph2Cube(GameBalls, gameWalls, true, true);

	CollisionInfo cInfo = CollideSph2Sph(gameKickers, true);
	if (/*timeFromLastCollide >= PARTICLE_COLLIDE_COOLDOWN && */cInfo.relation == RelationType::Ambiguous)
	{
		particleGenerator_collide->SpawnParticle(cInfo, PARTICLE_COLLIDE_NUMBER);
		//timeFromLastCollide = 0;
	}

	cInfo = CollideSph2Wall(gameKickers, gameWalls, true);
	if (/*timeFromLastCollide >= PARTICLE_COLLIDE_COOLDOWN && */cInfo.relation == RelationType::Ambiguous)
	{
		particleGenerator_collide->SpawnParticle(cInfo, PARTICLE_COLLIDE_NUMBER);
		//timeFromLastCollide = 0;
	}

	//timeFromLastCollide += dt;

	CollideSph2Sph(gameKickers, GameBalls, true);


	/*for (std::vector<Object3Dsphere*>::iterator it = gameBalls.begin(); it < gameBalls.end(); it++)
	{
		(*it)->UpdatePhysics(dt);
	};*/

	if (GameBalls[0] != NULL)
	{
		GameBalls[0]->UpdatePhysics(dt);
		
	}
	for (std::vector<Object3Dcube*>::iterator it = gameWalls.begin(); it < gameWalls.end(); it++)
	{
		(*it)->UpdatePhysics(dt);
	}
	for (std::vector<Object3Dcylinder*>::iterator it = gameKickers.begin(); it < gameKickers.end(); it++)
	{
		(*it)->UpdatePhysics(dt);
	}
	for (std::vector<Object3Dcube*>::iterator it = gameWalls.begin() + 4; it < gameWalls.end(); it++)
	{
		(*it)->UpdatePhysics(dt);
	}
}

void Game::updateLights(float currentTime)
{
	// Light update
	GameShader->use();
	lightsPos[0] = glm::vec3(sin(currentTime) * lightRadius, 3.0f, cos(currentTime) * lightRadius);
	GameShader->setVec3("viewPos", GameCamera->GetPosition());
	GameShader->setVec3("pointLights[0].position", lightsPos[0]);
	// light properties
	glm::vec3 lightColor;
	lightColor.r = sin(currentTime * 2.0f);
	lightColor.g = sin(currentTime * 1.2f);
	lightColor.b = sin(currentTime * 0.7f);
	glm::vec3 ambientColor = lightColor * glm::vec3(0.1);	// low influence
	glm::vec3 diffuseColor = lightColor * glm::vec3(0.5);	// middle influence
	GameShader->setVec3("pointLights[0].ambient", ambientColor);
	GameShader->setVec3("pointLights[0].diffuse", diffuseColor);
	GameShader->setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
}

void Game::updateStatus()
{
	BallInfo bInfo = GameBalls[0]->GetBallInfo();
	if (GameState == GameStateType::GAME_PLAYING && bInfo.Status != BallStatus::BallIsFree)
	{
		if (bInfo.Status == BallStatus::Score1)
		{
			GamePlayers[0]->AddScore(1);
			SoundEngine->play3D("resources/audio/explosion1.wav", irrklang::vec3df(0, 0, 0), false);
		}
		if (bInfo.Status == BallStatus::Score2)
		{
			GamePlayers[1]->AddScore(1);
		}
		GameBalls[0]->SetBallStatus(BallStatus::WaitForReset);
		GameState = GameStateType::GAME_COOLDOWN;
	}

}

/// playerIndex = 0/1
void Game::ShiftControlLeft(int playerIndex)
{

	int curCrl = GamePlayers[playerIndex]->CurrentControl;
	int next1 = (curCrl + 1) % 3 + playerIndex * 3;
	int	next2 = (curCrl + 2) % 3 + playerIndex * 3;
	
	float z0 = gameKickers[curCrl]->GetPosition().z;
	float z1 = gameKickers[next1]->GetPosition().z;
	float z2 = gameKickers[next2]->GetPosition().z;

	if (z1 >= z0)
	{
		if (z2 <= z0)
			GamePlayers[playerIndex]->SetControl(playerIndex == 0 ? next2 : next1);
		else if (z2 >= z1)
			GamePlayers[playerIndex]->SetControl(playerIndex == 0 ? next2 : next1);
		else
			GamePlayers[playerIndex]->SetControl(playerIndex == 1 ? next2 : next1);
	}
	else /*if (z1 < z0)*/
	{
		if (z2 >= z0)
			GamePlayers[playerIndex]->SetControl(playerIndex == 1 ? next2 : next1);
		else if (z2 <= z1)
			GamePlayers[playerIndex]->SetControl(playerIndex == 1 ? next2 : next1);
		else
			GamePlayers[playerIndex]->SetControl(playerIndex == 0 ? next2 : next1);
	}

}

void Game::ShiftControlRight(int playerIndex)
{
	int curCrl = GamePlayers[playerIndex]->CurrentControl;
	int next1 = (curCrl + 1) % 3 + playerIndex * 3;
	int	next2 = (curCrl + 2) % 3 + playerIndex * 3;

	float z0 = gameKickers[curCrl]->GetPosition().z;
	float z1 = gameKickers[next1]->GetPosition().z;
	float z2 = gameKickers[next2]->GetPosition().z;

	if (z1 >= z0)
	{
		if (z2 <= z0)
			GamePlayers[playerIndex]->SetControl(playerIndex == 1 ? next2 : next1);
		else if (z2 >= z1)
			GamePlayers[playerIndex]->SetControl(playerIndex == 1 ? next2 : next1);
		else
			GamePlayers[playerIndex]->SetControl(playerIndex == 0 ? next2 : next1);
	}
	else /*if (z1 < z0)*/
	{
		if (z2 >= z0)
			GamePlayers[playerIndex]->SetControl(playerIndex == 0 ? next2 : next1);
		else if (z2 <= z1)
			GamePlayers[playerIndex]->SetControl(playerIndex == 0 ? next2 : next1);
		else
			GamePlayers[playerIndex]->SetControl(playerIndex == 1 ? next2 : next1);
	}
}

void Game::ResetPosition()
{
	GameBalls[0]->SetPosition(glm::vec3(0, 0, 0));
	GameBalls[0]->SetVelocity(glm::vec3(0));
	GameBalls[0]->SetOmega(glm::vec3(0, 5, 0));
	GameBalls[0]->SetBallStatus(BallIsFree);

	for (int i = 0; i < 6; i++)
	{
		gameKickers[i]->SetPosition(KICKER_POSITION[i]);
		gameKickers[i]->SetStatic();
	}
}

void Game::updateParticles(float dt)
{

	particleGenerator_tail_0->Update(dt, *gameKickers[GamePlayers[0]->CurrentControl], 2);
	particleGenerator_tail_1->Update(dt, *gameKickers[GamePlayers[1]->CurrentControl], 2);
	particleGenerator_collide->Update(dt);

	particleGeneratorInstance_explosion_0->IsActive = GameState == GAME_COOLDOWN;
	particleGeneratorInstance_tail_0->IsActive = GameState == GAME_MAINMENU;
	particleGeneratorInstance_tail_1->IsActive = GameState == GAME_MAINMENU;

	//if (particleGeneratorInstance_tail_1->IsActive)
		particleGeneratorInstance_tail_1->Update(
			dt,
			glm::vec3(0, -5, 0),
			glm::vec3(10 * sin(currentTime), 5, -10 * cos(currentTime)),
			20,
			0.2,
			GameCamera->GetPosition());
	//if (particleGeneratorInstance_tail_0->IsActive)
		particleGeneratorInstance_tail_0->Update(
			dt, 
			glm::vec3(GROUND_WIDTH * 0.5f, -5, -GROUND_DEPTH * 0.5f), 
			glm::vec3(-2, 15, 2), 
			30, 
			0.1,
			GameCamera->GetPosition());
	//if (particleGeneratorInstance_explosion_0->IsActive)
		particleGeneratorInstance_explosion_0->Update(
			dt,
			glm::vec3(0, 20, 0),
			glm::vec3(0, 0, 0),
			5,
			0,
			GameCamera->GetPosition());
}