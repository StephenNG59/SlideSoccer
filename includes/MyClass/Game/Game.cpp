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
    gameShader = new Shader("shaders/test/colorsVS.glsl", "shaders/test/colorsFS.glsl");


	// ------------------------------------
    gameCamera = new Camera(glm::vec3(0.0f, 5.0f, 30.0f), glm::vec3(0, -5.0f, 0));
    gameCamera->SetPerspective(glm::radians(45.0f), (float)Width / (float)Height, 0.1f, 100.0f);

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

	gameBalls.push_back(new Object3Dsphere(0.4f, 20, 16));
	gameBalls[0]->AddTexture("resources/textures/awesomeface.png", ObjectTextureType::Emission);
	gameBalls[0]->SetPosition(glm::vec3(0, -4.8, 0));
	gameBalls[0]->SetERestitution(0.5f);
	gameBalls[0]->SetGravity(glm::vec3(0));

	for (int i = 0; i < 6; i++)
	{
		gamePlayers.push_back(new Object3Dcylinder(1.5f, 1.5f, 0.2f, 20));
		gamePlayers[i]->SetPosition(glm::vec3(i * 3 - 12, -4.8, i * 2 - 9));
		gamePlayers[i]->AddTexture("resources/textures/awesomeface.png", ObjectTextureType::Emission);
		gamePlayers[i]->SetFriction(1.0f);
		gamePlayers[i]->SetVelocity(glm::vec3(rand() % 50 - 25, 0, rand() % 40 - 20));
		gamePlayers[i]->SetOmega(glm::vec3(0, rand() % 20, 0));
		gamePlayers[i]->SetGravity(glm::vec3(0, 0, 0));
		gamePlayers[i]->SetERestitution(1.0f);
	}


	gameWalls.push_back(&wall_e);
	gameWalls.push_back(&wall_w);
	gameWalls.push_back(&wall_n);
	gameWalls.push_back(&wall_s);


    // point light 1
	gameShader->use();		// don't forget to do this !!!!!!!!
	gameShader->setBool("pointLights[0].isExist", true);
	gameShader->setFloat("pointLights[0].constant", 1.0f);
	gameShader->setFloat("pointLights[0].linear", 0.09);
	//gameShader->setFloat("pointLights[0].linear", 0.22);
	gameShader->setFloat("pointLights[0].quadratic", 0.032);
	//gameShader->setFloat("pointLights[0].quadratic", 0.0019);

}

void Game::Update(float dt)
{
    //CollideSph2Cube(gameBalls, gameTables, true);
	CollideSph2Ground(gameBalls, &ground);
	//CollideSph2Sph(gameBalls, true);
	CollideSph2Cube(gameBalls, gameWalls, true);
	CollideSph2Sph(gamePlayers, true);
	CollideSph2Wall(gamePlayers, gameWalls, true);
	CollideSph2Sph(gamePlayers, gameBalls, true);
	//CollideSph2Wall(gamePlayers[0], gameWalls[0], true);
	//CollideSph2Wall(gamePlayers[0], gameWalls[1], true);
	//CollideSph2Wall(gamePlayers[0], gameWalls[2], true);
	//CollideSph2Wall(gamePlayers[0], gameWalls[3], true);

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

	// Light update
	currentTime += dt;

	glm::vec3 lightPos = glm::vec3(sin(currentTime) * lightRadius, 3.0f, cos(currentTime) * lightRadius);
	gameShader->setVec3("viewPos", gameCamera->GetPosition());
	gameShader->setVec3("pointLights[0].position", lightPos); 

	// light properties
	glm::vec3 lightColor;
	lightColor.r = sin(currentTime * 2.0f);
	lightColor.g = sin(currentTime * 1.2f);
	lightColor.b = sin(currentTime * 0.7f);
	glm::vec3 ambientColor = lightColor * glm::vec3(0.1);	// low influence
	glm::vec3 diffuseColor = lightColor * glm::vec3(0.5);	// middle influence
	gameShader->setVec3("pointLights[0].ambient", ambientColor);
	gameShader->setVec3("pointLights[0].diffuse", diffuseColor);
	gameShader->setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);

}

void Game::Render()
{
	for (std::vector<Object3Dsphere*>::iterator it = gameBalls.begin(); it < gameBalls.end(); it++)
	{
		(*it)->Draw(*gameCamera, *gameShader);
	}
    for (std::vector<Object3Dcube*>::iterator it = gameWalls.begin(); it < gameWalls.end(); it++)
    {
        (*it)->Draw(*gameCamera, *gameShader);
    }
	for (std::vector<Object3Dcylinder*>::iterator it = gamePlayers.begin(); it < gamePlayers.end(); it++)
	{
		(*it)->Draw(*gameCamera, *gameShader);
	}
	ground.Draw(*gameCamera, *gameShader);
}