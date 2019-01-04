#include "pch.h"
#include "Game.h"

float groundWidth = 30.0f, groundHeight = 0.2f, groundDepth = 20.0f;
float wallThick = 1.0f, wallHeight = 5.0f;
Object3Dcube ground(glm::vec3(groundWidth, groundHeight, groundDepth));
Object3Dcube wall_e(glm::vec3(wallThick, wallHeight, groundDepth));
Object3Dcube wall_w(glm::vec3(wallThick, wallHeight, groundDepth));
Object3Dcube wall_n(glm::vec3(groundWidth, wallHeight, wallThick));
Object3Dcube wall_s(glm::vec3(groundWidth, wallHeight, wallThick));
Object3Dcube cube(glm::vec3(1.0f, 1.0f, 1.0f));
Object3Dsphere ball(0.5f, 32, 20);
Object3Dsphere ball3(0.4f, 32, 20);

// Time
float currentTime = 0;
float timeFromLastCollide = 0;

// Light
float lightRadius = 5.0f;

// Sensitivity
extern float keySensitivity;


Game::Game(unsigned int screenWidth, unsigned int screenHeight) 
    : State(GAME_MAINMENU), KeysPressed(), ViewportW(screenWidth), ViewportH(screenHeight)
{

}

Game::~Game()
{

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
	particleShader = new Shader("shaders/particle/vs.glsl", "shaders/particle/fs.glsl");
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

	// particle generator
	particleGenerator_tail = new ParticleGenerator(particleShader, GameCamera, 50);
	particleGenerator_collide = new ParticleGenerator(particleShader, GameCamera, 200);

	// model
	//model = new Model("resources/objects/nanosuit/nanosuit.obj");
	//model = new Model("resources/objects/ball/1212.obj");
	//model = new Model("resources/objects/grass/grass.obj");
	model = new Model("resources/objects/scene/slidesoccer scene.obj");

	// Skybox
	initSkybox();


	// Shadow
	this->initShadow();

	// Text manager
	GameTextManager = new TextManager(*TextShader);

}

float cameraCDtime = 0.1f;
float cameraFromLastMove = 0;
void Game::Update(float dt)
{
	//cameraFromLastMove += dt;
	//if (cameraFromLastMove >= cameraCDtime)
	//{
	//	glm::vec3 pos = gameKickers[0]->GetPosition();
	//	GameCamera->SmoothlyMoveTo(pos + glm::vec3(0, 25, 0), pos, CAMERA_UPVECNORM_X, CAMERA_SMOOTHMOVING_TIME);
	//	cameraFromLastMove = 0;
	//}


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
	
	updateObjects(dt);

	currentTime += dt;
	
	updateLights(currentTime);

	particleGenerator_tail->Update(dt, *gameKickers[0], 2);
	particleGenerator_tail->Update(dt, *gameKickers[1], 2);

	particleGenerator_collide->Update(dt);

	updateStatus();

}


void Game::Render(Shader *renderShader)
{
	for (std::vector<Object3Dsphere*>::iterator it = GameBalls.begin(); it < GameBalls.end(); it++)
	{
		(*it)->Draw(*GameCamera, *renderShader);
	}
    for (std::vector<Object3Dcube*>::iterator it = gameWalls.begin(); it < gameWalls.end(); it++)
    {
        (*it)->Draw(*GameCamera, *renderShader);
    }
	for (std::vector<Object3Dcylinder*>::iterator it = gameKickers.begin(); it < gameKickers.end(); it++)
	{
		(*it)->Draw(*GameCamera, *renderShader);
	}
	//ground.Draw(*GameCamera, *renderShader);

	particleGenerator_tail->Draw();
	particleGenerator_collide->Draw();

	GameShader->setFloat("material.shininess", 32);
	model->Draw(*GameCamera, *GameShader, glm::scale(ground.GetModelMatrix(), glm::vec3(3.0f)));

	GameSkybox->Draw(*GameCamera);

	GameTextManager->Render(*TextShader);
	
	GameTextManager->RenderText(*TextShader, std::to_string(GamePlayers[0]->GetScore()) + " : " + std::to_string(GamePlayers[1]->GetScore()), 60.0f, 60.0f, 0.5f, glm::vec3(0.5, 0.8f, 0.2f));

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
		Render(DepthShader);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// 2. Render scene as normal 
	glViewport(ViewportX, ViewportY, ViewportW, ViewportH);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// comment this to support two viewports
	GameShader->use();
	// Set light uniforms
	GameShader->setVec3("shadowLightPos", lightPos);
	GameShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);

	int shadowMapID = SHADOW_MAP_ID;
	GameShader->setInt("shadowMap", shadowMapID);
	glActiveTexture(GL_TEXTURE0 + shadowMapID);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	Render(GameShader);

}


void Game::ProcessInput(float dt)
{

	if (this->KeysCurrent[GLFW_KEY_UP])
	{
		this->gameKickers[0]->AddVelocity(glm::vec3(0, 0, -25.0f) * dt);
	}
	if (this->KeysCurrent[GLFW_KEY_DOWN])
	{
		this->gameKickers[0]->AddVelocity(glm::vec3(0, 0, 25.0f) * dt);
	}
	if (this->KeysCurrent[GLFW_KEY_LEFT])
	{
		this->gameKickers[0]->AddVelocity(glm::vec3(-25.0f, 0, 0) * dt);
	}
	if (this->KeysCurrent[GLFW_KEY_RIGHT])
	{
		this->gameKickers[0]->AddVelocity(glm::vec3(25.0f, 0, 0) * dt);
	}
	if (this->KeysCurrent[GLFW_KEY_J])
	{
		this->gameKickers[0]->AddOmega(glm::vec3(0, 0.2f, 0));
	}
	if (this->KeysCurrent[GLFW_KEY_K])
	{
		this->gameKickers[0]->AddOmega(glm::vec3(0, -0.2f, 0));
	}

	if (this->KeysCurrent[GLFW_KEY_W])
	{
		GameCamera->RotateDownByDegree(-keySensitivity);
	}
	if (this->KeysCurrent[GLFW_KEY_S])
	{
		GameCamera->RotateDownByDegree(keySensitivity);
	}
	if (this->KeysCurrent[GLFW_KEY_A])
	{
		GameCamera->RotateRightByDegree(-keySensitivity);
	}
	if (this->KeysCurrent[GLFW_KEY_D])
	{
		GameCamera->RotateRightByDegree(keySensitivity);
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


// ------------------------------
// -- Initialization Functions --

void Game::createObjects()
{
	// ground
	glm::vec3 groundPos = glm::vec3(0, -5.0f, 0.0f);
	ground.SetMass(0);
	ground.SetPosition(groundPos);
	ground.SetFriction(0.8f);
	// wall-e
	wall_e.SetPosition(groundPos + glm::vec3(0.5f * groundWidth, 0.5f * wallHeight + 0.5f * groundHeight, 0));
	wall_e.IsGoal1 = true;
	// wall-w
	wall_w.SetPosition(groundPos + glm::vec3(-0.5f * groundWidth, 0.5f * wallHeight + 0.5f * groundHeight, 0));
	// wall-n
	wall_n.SetPosition(groundPos + glm::vec3(0, 0.5f * wallHeight + 0.5f * groundHeight, -0.5f * groundDepth));
	// wall-s
	wall_s.SetPosition(groundPos + glm::vec3(0, 0.5f * wallHeight + 0.5f * groundHeight, 0.5f * groundDepth));

	GameBalls.push_back(new Object3Dsphere(3.0f, 20, 16));
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
		gameKickers[i]->SetLinearFriction(0.25f);
		gameKickers[i]->SetConstantFriction(5);
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


// ----------------------
// -- Update Functions --

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
	if (bInfo.Status != BallStatus::BallIsFree)
	{
		if (bInfo.Status == BallStatus::Score1)
		{
			GamePlayers[0]->AddScore(1);
		}
		if (bInfo.Status == BallStatus::Score2)
		{
			GamePlayers[1]->AddScore(1);
		}
		GameBalls[0]->SetBallStatus(BallStatus::WaitForReset);
	}
}