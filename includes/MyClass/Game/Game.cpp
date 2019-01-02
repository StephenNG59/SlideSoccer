#include "Game.h"

float groundWidth = 30.0f, groundHeight = 0.2f, groundDepth = 20.0f;
float wallThick = 1.0f, wallHeight = 2.0f;
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

// Light
float lightRadius = 5.0f;

// Sensitivity
extern float keySensitivity;


Game::Game(unsigned int Width, unsigned int Height) 
    : State(GAME_ACTIVE), Keys(), Width(Width), Height(Height)
{

}

Game::~Game()
{

}


void Game::Init()
{
    // build and compile our shader program
	// ------------------------------------
    GameShader = new Shader("shaders/test/colorsVS.glsl", "shaders/test/colorsFS.glsl");
	particleShader = new Shader("shaders/particle/vs.glsl", "shaders/particle/fs.glsl");


	// ------------------------------------
    GameCamera = new Camera(glm::vec3(0.0f, 5.0f, 30.0f), glm::vec3(0, -5.0f, 0));
    GameCamera->SetPerspective(glm::radians(45.0f), (float)Width / (float)Height, 0.1f, 100.0f);


	stbi_set_flip_vertically_on_load(true);


	// -- Objects --

	// ground
	glm::vec3 groundPos = glm::vec3(0, -5.0f, 0.0f);
	ground.SetMass(0);
	ground.SetPosition(groundPos);
	ground.SetFriction(0.8f);
	// wall-e
	wall_e.SetMass(0);
	wall_e.SetPosition(groundPos + glm::vec3(0.5f * groundWidth, 0.5f * wallHeight + 0.5f * groundHeight, 0));
	// wall-w
	wall_w.SetMass(0);
	wall_w.SetPosition(groundPos + glm::vec3(-0.5f * groundWidth, 0.5f * wallHeight + 0.5f * groundHeight, 0));
	// wall-n
	wall_n.SetMass(0);
	wall_n.SetPosition(groundPos + glm::vec3(0, 0.5f * wallHeight + 0.5f * groundHeight, -0.5f * groundDepth));
	// wall-s
	wall_s.SetMass(0);
	wall_s.SetPosition(groundPos + glm::vec3(0, 0.5f * wallHeight + 0.5f * groundHeight, 0.5f * groundDepth));

	gameBalls.push_back(new Object3Dsphere(0.8f, 20, 16));
	gameBalls[0]->AddTexture("resources/textures/awesomeface.png", ObjectTextureType::Emission);
	gameBalls[0]->SetPosition(glm::vec3(0, -4.8, 0));
	gameBalls[0]->SetERestitution(0.5f);
	gameBalls[0]->SetGravity(glm::vec3(0));

	for (int i = 0; i < 6; i++)
	{
		gamePlayers.push_back(new Object3Dcylinder(1.5f, 1.5f, 0.2f, 20));
		glm::vec3 pos = glm::vec3(i * 3 - 12, -4.8, i * 2 - 9);
		gamePlayers[i]->SetPosition(pos);
		gamePlayers[i]->AddTexture("resources/textures/awesomeface.png", ObjectTextureType::Emission);
		gamePlayers[i]->SetFriction(1.0f);
		//gamePlayers[i]->SetVelocity(glm::vec3(rand() % 50 - 25, 0, rand() % 40 - 20) / 10.0f);
		gamePlayers[i]->SetOmega(glm::vec3(0, rand() % 20, 0));
		gamePlayers[i]->SetGravity(glm::vec3(0, 0, 0));
		gamePlayers[i]->SetERestitution(1.0f);
		gamePlayers[i]->SetLinearFriction(0.25f);
		gamePlayers[i]->SetConstantFriction(5);
	}
	gamePlayers[0]->SetVelocity(glm::vec3(1.5f, 0, 2.5f));
	gamePlayers[0]->SetOmega(glm::vec3(0, 20.0f, 0));
	//gamePlayers[0]->AddModel("resources/objects/ball/1212.obj");
	//gamePlayers[0]->AddModel("resources/objects/nanosuit/nanosuit.obj");
	gamePlayers[0]->AddModel("resources/objects/ball/pumpkin_01.obj");


	gameWalls.push_back(&wall_e);
	gameWalls.push_back(&wall_w);
	gameWalls.push_back(&wall_n);
	gameWalls.push_back(&wall_s);


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

	// particle generator
	particleGenerator = new ParticleGenerator(particleShader, GameCamera, 500);

	// model
	//model = new Model("resources/objects/nanosuit/nanosuit.obj");
	//model = new Model("resources/objects/ball/1212.obj");
	//model = new Model("resources/objects/grass/grass.obj");

	// Skybox
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
	std::vector<std::string> shadersPath = {
		"shaders/skybox/skyboxVS.glsl", "shaders/skybox/skyboxFS.glsl"
	};
	GameSkybox = new Skybox(facesPath3, shadersPath);


}


void Game::Update(float dt)
{
	//CollideSph2Ground(gameBalls, &ground);
	CollideSph2Cube(gameBalls, gameWalls, true, true);
	CollideSph2Sph(gamePlayers, true);
	CollideSph2Wall(gamePlayers, gameWalls, true);
	CollideSph2Sph(gamePlayers, gameBalls, true);



    for (std::vector<Object3Dsphere*>::iterator it = gameBalls.begin(); it < gameBalls.end(); it++)
    {
        (*it)->UpdatePhysics(dt);
	};
    for (std::vector<Object3Dcube*>::iterator it = gameWalls.begin(); it < gameWalls.end(); it++)
    {
        (*it)->UpdatePhysics(dt);
    }
	for (std::vector<Object3Dcylinder*>::iterator it = gamePlayers.begin(); it < gamePlayers.end(); it++)
	{
		(*it)->UpdatePhysics(dt);
	}




	currentTime += dt;
	
	// Light update
	GameShader->use();
	glm::vec3 lightPos = glm::vec3(sin(currentTime) * lightRadius, 3.0f, cos(currentTime) * lightRadius);
	GameShader->setVec3("viewPos", GameCamera->GetPosition());
	GameShader->setVec3("pointLights[0].position", lightPos); 
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

	particleGenerator->Update(dt, *gamePlayers[0], 2);
}


void Game::Render()
{
	for (std::vector<Object3Dsphere*>::iterator it = gameBalls.begin(); it < gameBalls.end(); it++)
	{
		(*it)->Draw(*GameCamera, *GameShader);
	}
    for (std::vector<Object3Dcube*>::iterator it = gameWalls.begin(); it < gameWalls.end(); it++)
    {
        (*it)->Draw(*GameCamera, *GameShader);
    }
	for (std::vector<Object3Dcylinder*>::iterator it = gamePlayers.begin(); it < gamePlayers.end(); it++)
	{
		(*it)->Draw(*GameCamera, *GameShader);
	}
	ground.Draw(*GameCamera, *GameShader);

	particleGenerator->Draw();

	GameShader->setFloat("material.shininess", 32);
	//model->Draw(*GameCamera, *gameShader, gamePlayers[0]->GetModelMatrix());

	GameSkybox->Draw(*GameCamera);
}


void Game::ProcessInput(float dt)
{
	if (this->Keys[GLFW_KEY_UP])
	{
		this->gamePlayers[0]->AddVelocity(glm::vec3(0, 0, -25.0f) * dt);
	}
	if (this->Keys[GLFW_KEY_DOWN])
	{
		this->gamePlayers[0]->AddVelocity(glm::vec3(0, 0, 25.0f) * dt);
	}
	if (this->Keys[GLFW_KEY_LEFT])
	{
		this->gamePlayers[0]->AddVelocity(glm::vec3(-25.0f, 0, 0) * dt);
	}
	if (this->Keys[GLFW_KEY_RIGHT])
	{
		this->gamePlayers[0]->AddVelocity(glm::vec3(25.0f, 0, 0) * dt);
	}
	if (this->Keys[GLFW_KEY_J])
	{
		this->gamePlayers[0]->AddOmega(glm::vec3(0, 0.2f, 0));
	}
	if (this->Keys[GLFW_KEY_K])
	{
		this->gamePlayers[0]->AddOmega(glm::vec3(0, -0.2f, 0));
	}

	if (this->Keys[GLFW_KEY_W])
	{
		GameCamera->RotateDownByDegree(-keySensitivity);
	}
	if (this->Keys[GLFW_KEY_S])
	{
		GameCamera->RotateDownByDegree(keySensitivity);
	}
	if (this->Keys[GLFW_KEY_A])
	{
		GameCamera->RotateRightByDegree(-keySensitivity);
	}
	if (this->Keys[GLFW_KEY_D])
	{
		GameCamera->RotateRightByDegree(keySensitivity);
	}

	if (this->Keys[GLFW_KEY_EQUAL])
	{
		GameCamera->GoForward(keySensitivity);
	}
	if (this->Keys[GLFW_KEY_MINUS])
	{
		GameCamera->GoForward(-keySensitivity);
	}
}