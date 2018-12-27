#include "stdafx.h"
#include "Collision.h"

extern float deltaTime;

void printVec3(std::string name, glm::vec3 v)
{
	std::cout << name << ": ";
	std::cout << v.x << " # " << v.y << " # " << v.z << std::endl;
}
void printVec3(glm::vec3 v)
{
	std::cout << "vec3: " << v.x << " # " << v.y << " # " << v.z << std::endl;
}


CollisionFreeStuckType checkFStype(float m1, float m2)
{
	if (m1 == 0 && m2 == 0)
	{
		return CollisionFreeStuckType::BothStuck;
	}
	else if (m1 == 0)
	{
		return CollisionFreeStuckType::Stuck1st;
	}
	else if (m2 == 0)
	{
		return CollisionFreeStuckType::Stuck2nd;
	}
	else
	{
		return CollisionFreeStuckType::BothFree;
	}
}

// --------------------------------------------------------------------------------------

CollisionInfo CollideSph2Sph(Object3Dsphere * sph1, Object3Dsphere * sph2, bool autoDeal)
{

	CollisionInfo cInfo;
	CollisionFreeStuckType fsType;

	// ---------------
	// get information

	float m1 = sph1->GetMass(), m2 = sph2->GetMass();
	fsType = checkFStype(m1, m2);

	float r1_abs = sph1->GetRadius(), r2_abs = sph2->GetRadius();

	glm::vec3 p1 = sph1->GetPosition(), p2 = sph2->GetPosition();
	glm::vec3 v1 = sph1->GetVelocity(), v2 = sph2->GetVelocity();


	// --------------
	// detect collide

	glm::vec3 vec1to2 = p2 - p1;
	glm::vec3 xStar = glm::normalize(vec1to2);
												
	float dis = vecMod(vec1to2);														// #NOTE - length()ÊÇÖ¸ÏòÁ¿Î¬¶È!!!!(vec3.length() == 3)

	if (dis >= r1_abs + r2_abs)		
	{
		// not collide
		cInfo.relation = RelationType::Stranger;
		return cInfo;
	}
	
	float v1_xstar = glm::dot(v1, xStar), v2_xstar = glm::dot(v2, xStar);			// v1-->v2 as positive x direction
	

	// #NOTE conditions like a ball stay on the desk
	if (dis < r1_abs + r2_abs/* && abs(v1_xstar - v2_xstar) < 0.01f*/)
	{
		glm::vec3 delta_p = (r1_abs + r2_abs - dis) * xStar;
		if (fsType == CollisionFreeStuckType::BothFree)
		{
			sph1->ChangePosition(-delta_p / 2.0f);
			sph2->ChangePosition(delta_p / 2.0f);
		}
		else if (fsType == CollisionFreeStuckType::Stuck1st)
		{
			sph2->ChangePosition(delta_p);
		}
		else if (fsType == CollisionFreeStuckType::Stuck2nd)
		{
			sph1->ChangePosition(-delta_p);
		}
	}


	if (v1_xstar < v2_xstar)							
	{
		// not collide but overlap
		cInfo.relation = RelationType::Breaking;
		return cInfo;
	}

	cInfo.relation = RelationType::Ambiguous;


	// if collide and the speed smaller than specific value, make it stop
	if (vecMod(v1) < 0.1f && vecMod(v2) < 0.1f)
	{
		cInfo.v1After = glm::vec3(0);
		cInfo.v2After = glm::vec3(0);
		cInfo.angularImpulse1 = glm::vec3(0);
		cInfo.angularImpulse2 = glm::vec3(0);
		if (autoDeal)
			DealCollision(sph1, sph2, cInfo);
		return cInfo;
	}

	// -----------------------------------------------------------
	// calculate velocity change caused by center-center collision

	glm::vec3 v1_subxstar = v1 - v1_xstar * xStar, v2_subxstar = v2 - v2_xstar * xStar;		// the remaining velocity would not change in the collision

	float e = sph1->GetERestitution() * sph2->GetERestitution();					// coefficient of restitution

	// velocity on the direction of center line    
	// #NOTE still need to consider the angular velocity!!!
	float v1_xstar_prime, v2_xstar_prime, I_xstar_abs;
	float vxDelta1, vxDelta2;	
	glm::vec3 I21_xstar = glm::vec3(0), I12_xstar = glm::vec3(0);
	
	if (fsType == CollisionFreeStuckType::BothFree)
	{
		v1_xstar_prime = (m1 * v1_xstar + m2 * v2_xstar - m2 * e * (v1_xstar - v2_xstar)) / (m1 + m2);
		v2_xstar_prime = (m1 * v1_xstar + m2 * v2_xstar + m1 * e * (v1_xstar - v2_xstar)) / (m1 + m2);
		I21_xstar = m1 * (v1_xstar_prime - v1_xstar) * xStar;
		I12_xstar = -I21_xstar;
		// vxDelta1 = v1_xstar_prime - v1_xstar;
		I_xstar_abs = vecMod(I21_xstar);
	}
	else if (fsType == CollisionFreeStuckType::Stuck1st)
	{
		v1_xstar_prime = v1_xstar;		
		v2_xstar_prime = (1 + e) * v1_xstar - e * v2_xstar;
		I12_xstar = m2 * (v2_xstar_prime - v2_xstar) * xStar;
		I_xstar_abs = vecMod(I12_xstar);
		// vxDelta2 = v2_xstar_prime - v2_xstar;
		// I_xstar_abs = abs(vxDelta2 * m2);
	}
	else if (fsType == CollisionFreeStuckType::Stuck2nd)
	{
		v1_xstar_prime = (1 + e) * v2_xstar - e * v1_xstar;
		v2_xstar_prime = v2_xstar;
		I21_xstar = m1 * (v1_xstar_prime - v1_xstar) * xStar;
		I_xstar_abs = vecMod(I21_xstar);
		// vxDelta1 = v1_xstar_prime - v1_xstar;
		// I_xstar_abs = abs(vxDelta1 * m1);
	}
	else if (fsType == CollisionFreeStuckType::BothStuck)
	{
		v1_xstar_prime = v1_xstar;
		v2_xstar_prime = v2_xstar;
		return cInfo;
	}


	// ----------------------
	// calculate omega change

	float f1 = sph1->GetFriction(), f2 = sph2->GetFriction();
	glm::vec3 r1 = xStar * r1_abs, r2 = -xStar * r2_abs;
	glm::vec3 w1LC = sph1->GetOmega(), w2LC = sph2->GetOmega();
	// #NOTE remember to translate LC coord to WC coord!!!
	glm::vec3 w1 = glm::vec3(sph1->GetModelMatrix() * glm::vec4(w1LC, 0.0f)), w2 = glm::vec3(sph2->GetModelMatrix() * glm::vec4(w2LC, 0.0f));


	// calculate direction of relative velocity on the collide point
	glm::vec3 v1touch = (w1 == glm::vec3(0.0f)) ? (v1) : (v1 + glm::cross(w1, r1));
	glm::vec3 v2touch = (w2 == glm::vec3(0.0f)) ? (v2) : (v2 + glm::cross(w2, r2));
	glm::vec3 v12touch = v1touch - v2touch, v21touch = -v12touch;
	glm::vec3 v12touch_yzstar = v12touch - glm::dot(v12touch, xStar) * xStar, v21touch_yzstar = -v12touch_yzstar;

	// glm::vec3 vTouch12Norm = glm::normalize(v1touch - v2touch), vTouch21Norm = -vTouch12Norm;

	// momentum change caused by yzstar part of the impulse

	glm::vec3 delta_L1(0), delta_L2(0);
	glm::vec3 v1_after = v1_xstar_prime * xStar + v1_subxstar;
	glm::vec3 v2_after = v2_xstar_prime * xStar + v2_subxstar;


	if (v12touch_yzstar == glm::vec3(0) || I_xstar_abs == 0)
	{
		cInfo.v1After = v1_after;
		cInfo.v2After = v2_after;
		cInfo.angularImpulse1 = cInfo.angularImpulse2 = glm::vec3(0);

		if (autoDeal)
		{
			DealCollision(sph1, sph2, cInfo);
		}

		return cInfo;
	}

	// --(1)--
	float I1 = sph1->GetInertia(), I2 = sph2->GetInertia();
	float m_equivalent;

	if (fsType == CollisionFreeStuckType::BothFree)
	{
		m_equivalent = 1 / (((r1_abs * r1_abs) / (I1) + 1 / m1) + ((r2_abs * r2_abs) / (I2 * I2) + 1 / m2));
	}
	else if (fsType == CollisionFreeStuckType::Stuck1st)
	{
		m_equivalent = 1 / ((r2_abs * r2_abs) / (I2) + 1 / m2);
	}
	else if (fsType == CollisionFreeStuckType::Stuck2nd)
	{
		m_equivalent = 1 / ((r1_abs * r1_abs) / (I1) + 1 / m1);
	}

	float I_yzstar_abs = std::min(0.5f * (f1 + f2) * I_xstar_abs, vecMod(v12touch_yzstar) * m_equivalent);

	// --(2)--
	glm::vec3 v12_yzstar_norm = glm::normalize(v12touch_yzstar), v21_yzstar_norm = -v12_yzstar_norm;
	// --(3)--
	delta_L1 = glm::cross(r1, I_yzstar_abs * v21_yzstar_norm);
	delta_L2 = glm::cross(r2, I_yzstar_abs * v12_yzstar_norm + I12_xstar);
	cInfo.angularImpulse1 = delta_L1;
	cInfo.angularImpulse2 = delta_L2;


	glm::vec3 delta_v1(0), delta_v2(0);
	if (fsType == CollisionFreeStuckType::BothFree)
	{
		delta_v1 = I_yzstar_abs * v21_yzstar_norm / m1;
		delta_v2 = I_yzstar_abs * v12_yzstar_norm / m2;
	}
	else if (fsType == CollisionFreeStuckType::Stuck1st)
	{
		delta_v2 = I_yzstar_abs * v12_yzstar_norm / m2;
	}
	else if (fsType == CollisionFreeStuckType::Stuck2nd)
	{
		delta_v1 = I_yzstar_abs * v21_yzstar_norm / m1;
	}

	v1_after += delta_v1;
	v2_after += delta_v2;

	cInfo.v1After = v1_after;
	cInfo.v2After = v2_after;


	// --------------
	// deal collision

	if (autoDeal)
	{
		DealCollision(sph1, sph2, cInfo);
	}

	return cInfo;
}

// --------------------------------------------------------------------------------------

CollisionInfo CollideSph2Cube(Object3Dsphere * sphere, Object3Dcube * cube, bool autoDeal)
{

	CollisionInfo cInfo;
	CollisionFreeStuckType fsType;

	float m1 = sphere->GetMass(), m2 = cube->GetMass();
	fsType = checkFStype(m1, m2);


	// ----------------------------------------------------------------
	// get current width, height, depth direction of the (rotated) cube

	glm::vec3 cubeX = glm::vec3(cube->GetRotationMatrix() * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));		// width direction
	glm::vec3 cubeY = glm::vec3(cube->GetRotationMatrix() * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));		// height direction
	glm::vec3 cubeZ = glm::vec3(cube->GetRotationMatrix() * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f));		// depth direction
	
	// normalize them to be the x, y, z unit vector

	glm::vec3 cubeXNorm = glm::normalize(cubeX);
	glm::vec3 cubeYNorm = glm::normalize(cubeY);
	glm::vec3 cubeZNorm = glm::normalize(cubeZ);


	// -------------
	// get cube size

	float width = cube->GetWidth(), height = cube->GetHeight(), depth = cube->GetDepth();
	float halfW = width / 2.0f, halfH = height / 2.0f, halfD = depth / 2.0f;
	glm::vec3 cubeHalfExtents(halfW, halfH, halfD);

	glm::vec3 vecCube2SphWC = sphere->GetPosition() - cube->GetPosition();					// world coordinate
	float cube2SphXLC = glm::dot(vecCube2SphWC, cubeXNorm);
	float cube2SphYLC = glm::dot(vecCube2SphWC, cubeYNorm);
	float cube2SphZLC = glm::dot(vecCube2SphWC, cubeZNorm);
	if (abs(cube2SphXLC) < halfW && abs(cube2SphYLC) < halfH && abs(cube2SphZLC) < halfD)
	{
		// the center of the circle is inside the cube
		cInfo.relation = RelationType::FallInLove;
		return cInfo;
	}


	glm::vec3 vecCube2SphLC = glm::vec3(cube2SphXLC, cube2SphYLC, cube2SphZLC);				// local coordinate
	glm::vec3 closestLC = glm::clamp(vecCube2SphLC, -cubeHalfExtents, cubeHalfExtents);		// closest point on cube in local coord
	glm::vec3 closestWC = glm::vec3(cube->GetModelMatrix() * glm::vec4(closestLC, 1.0f));	// closest point on cube in world coord
	glm::vec3 cube2ClosestWC = closestWC - cube->GetPosition();								// cube's center to closest point


	// ---------------
	// get sphere data

	glm::vec3 pSph = sphere->GetPosition();
	float rSph = sphere->GetRadius();


	// --------------
	// detect collide

	glm::vec3 closest2Sph = pSph - closestWC; 
	float dis = vecMod(closest2Sph);
	if (dis > rSph)
	{
		// not collide
		sphere->IsTouchingDesk = false;
		cInfo.relation = RelationType::Stranger;
		return cInfo;
	}
	// if (dis < rSph && is moving apart relatively) { relation = Breaking; } 
	cInfo.relation = RelationType::Ambiguous;


	glm::vec3 v1 = sphere->GetVelocity(), v2 = cube->GetVelocity();
	//printf("v1: %.2f, v2: %.2f\n", vecMod(v1), vecMod(v2));
	//// if collide and the speed smaller than specific value, make it stop
	//if (vecMod(v1) < 1.0f && vecMod(v2) < 1.0f)
	//{
	//	cInfo.v1After = glm::vec3(0);
	//	cInfo.v2After = glm::vec3(0);
	//	cInfo.angularImpulse1 = glm::vec3(0);
	//	cInfo.angularImpulse2 = glm::vec3(0);
	//	if (autoDeal)
	//		DealCollision(sphere, cube, cInfo);
	//	return cInfo;
	//}

	// determine which face is colliding
	
	CubeFace collideFace;
	glm::vec3 xStar;																		// #NOTE xStar: sphere center to the collide point vector in WC
	if (closestLC.x == halfW) {
		collideFace = CubeFace::XPos;
		xStar = -cubeXNorm;
	}
	else if (closestLC.x == -halfW) {
		collideFace = CubeFace::XNeg;
		xStar = cubeXNorm;
	}
	else if (closestLC.y == halfH) {
		collideFace = CubeFace::YPos;
		xStar = -cubeYNorm;
	}
	else if (closestLC.y == -halfH) {
		collideFace = CubeFace::YNeg;
		xStar = cubeYNorm;
	}
	else if (closestLC.z == halfD) {
		collideFace = CubeFace::ZPos;
		xStar = -cubeZNorm;
	}
	else if (closestLC.z == -halfD) {
		collideFace = CubeFace::ZNeg;
		xStar = cubeZNorm;
	}


	// -----------------
	// start calculation

	// -- step1: velocity and impulse --

	float e = sphere->GetERestitution() * cube->GetERestitution();

	float v1_xstar = glm::dot(v1, xStar), v2_xstar = glm::dot(v2, xStar);

	glm::vec3 force_friction(0);
	// #NOTE conditions like a ball stay on the desk
	if (/*abs(v1_xstar - v2_xstar) < 0.1f &&*/ dis < rSph && abs(v1_xstar - v2_xstar) < 0.1f)
	{
		glm::vec3 delta_p = (rSph - dis) * xStar;
		if (fsType == CollisionFreeStuckType::BothFree)
		{
			sphere->ChangePosition(-delta_p / 2.0f);
			cube->ChangePosition(delta_p / 2.0f);
		}
		else if (fsType == CollisionFreeStuckType::Stuck1st)
		{
			cube->ChangePosition(delta_p);
		}
		else if (fsType == CollisionFreeStuckType::Stuck2nd)
		{
			sphere->ChangePosition(-delta_p);
		}
	}


	if (v1_xstar < v2_xstar)
	{
		// #NOTE - if we do not detect this, before the first collision finishes, the second one will be performed
		//		 - which will make the two objects look like stuck with each other
		cInfo.relation = RelationType::Breaking;
		return cInfo;
	}
	glm::vec3 v1_subxstar = v1 - v1_xstar * xStar, v2_subxstar = v2 - v2_xstar * xStar;



	float v1_xstar_prime, v2_xstar_prime, I_xstar_abs = 0;
	glm::vec3 I21_xstar = glm::vec3(0), I12_xstar = glm::vec3(0);
	if (fsType == CollisionFreeStuckType::BothFree)
	{
		v1_xstar_prime = (m1 * v1_xstar + m2 * v2_xstar - e * m2 * (v1_xstar - v2_xstar)) / (m1 + m2);
		v2_xstar_prime = (m1 * v1_xstar + m2 * v2_xstar + e * m1 * (v1_xstar - v2_xstar)) / (m1 + m2);
		I21_xstar = m1 * (v1_xstar_prime - v1_xstar) * xStar;
		I12_xstar = -I21_xstar;
		I_xstar_abs = vecMod(I21_xstar);
	}
	else if (fsType == CollisionFreeStuckType::Stuck1st)
	{
		v1_xstar_prime = v1_xstar;
		v2_xstar_prime = (1 + e) * v1_xstar - e * v2_xstar;
		I12_xstar = m2 * (v2_xstar_prime - v2_xstar) * xStar;
		I_xstar_abs = vecMod(I12_xstar);
	}
	else if (fsType == CollisionFreeStuckType::Stuck2nd)
	{
		v1_xstar_prime = (1 + e) * v2_xstar - e * v1_xstar;
		v2_xstar_prime = v2_xstar;
		I21_xstar = m1 * (v1_xstar_prime - v1_xstar) * xStar;
		I_xstar_abs = vecMod(I21_xstar);
	}
	else if (fsType == CollisionFreeStuckType::BothStuck)
	{
		v1_xstar_prime = v1_xstar;
		v2_xstar_prime = v2_xstar;
		return cInfo;
	}


	// -- step2: angular velocity --

	glm::vec3 w1LC = sphere->GetOmega(), w2LC = cube->GetOmega();
	// #NOTE remember to translate LC coord to WC coord!!!
	glm::vec3 w1 = glm::vec3(sphere->GetModelMatrix() * glm::vec4(w1LC, 0.0f)), w2 = glm::vec3(cube->GetModelMatrix() * glm::vec4(w2LC, 0.0f));
	float f1 = sphere->GetFriction(), f2 = cube->GetFriction();
	glm::vec3 r1 = sphere->GetRadius() * xStar, r2 = cube2ClosestWC;					// #NOTE ÕâÀïÒ»¿ªÊ¼ÓÃglm::vec3 r1 = r1 * xStar; »áÓÐbug

	glm::vec3 v1touch = (w1 == glm::vec3(0.0f)) ? (v1) : (v1 + glm::cross(w1, r1));
	glm::vec3 v2touch = (w2 == glm::vec3(0.0f)) ? (v2) : (v2 + glm::cross(w2, r2));
	glm::vec3 v12touch = v1touch - v2touch, v21touch = -v12touch;


	glm::vec3 v12touch_yzstar = v12touch - glm::dot(v12touch, xStar) * xStar, v21touch_yzstar = -v12touch_yzstar;

	// momentum change caused by yzstar part of the impulse

	glm::vec3 v1_after = v1_xstar_prime * xStar + v1_subxstar;
	glm::vec3 v2_after = v2_xstar_prime * xStar + v2_subxstar;
	

	if (v12touch_yzstar == glm::vec3(0)/* || I_xstar_abs == 0*/)			// #INNORMAL when I_xstar_abs == 0, maybe a static friction
	{
		glm::vec3 angularV = glm::cross(w1, r1);

		cInfo.v1After = v1_after;
		cInfo.v2After = v2_after;
		cInfo.angularImpulse1 = cInfo.angularImpulse2 = glm::vec3(0);

		if (autoDeal)
		{
			DealCollision(sphere, cube, cInfo);
		}

		return cInfo;
	}


	// --(1)--
	float I1 = sphere->GetInertia(), I2 = cube->GetInertia();
	float r2_abs = vecMod(cube2ClosestWC);
	float m_equivalent;
	bool is_static_friction = (dis <= rSph && abs(v1_xstar - v2_xstar) < 0.1f);
	float force_N_abs = 0;

	if (fsType == CollisionFreeStuckType::BothFree)
	{
		m_equivalent = 1 / (((rSph * rSph) / (I1) + 1 / m1) + ((r2_abs * r2_abs) / (I2 * I2) + 1 / m2));
	}
	else if (fsType == CollisionFreeStuckType::Stuck1st)
	{
		m_equivalent = 1 / ((r2_abs * r2_abs) / (I2) + 1 / m2);
	}
	else if (fsType == CollisionFreeStuckType::Stuck2nd)
	{
		m_equivalent = 1 / ((rSph * rSph) / (I1) + 1 / m1);
	}

	float I_yzstar_abs = std::min(0.5f * (f1 + f2) * I_xstar_abs, vecMod(v12touch_yzstar) * m_equivalent);		// #INNORMAL when rotate on the desk, the left one would be very small!!
	// #NOTE static friction
	if (is_static_friction)
	{
		sphere->IsTouchingDesk = true;
		//printf("istouchingdesk\n");

		if (fsType == CollisionFreeStuckType::BothFree)
		{
			force_N_abs = 0;

		}
		else if (fsType == CollisionFreeStuckType::Stuck1st)
		{
			force_N_abs = std::max(0.0f, glm::dot(cube->GetForce(), -xStar));
			//force_N_abs = vecMod(cube->GetForce());
		}
		else if (fsType == CollisionFreeStuckType::Stuck2nd)
		{
			force_N_abs = std::max(0.0f, glm::dot(sphere->GetForce(), xStar));
			//force_N_abs = vecMod(sphere->GetForce());
		}
		
		I_yzstar_abs = std::min(force_N_abs * deltaTime, vecMod(v12touch_yzstar) * m_equivalent);
		//printf("I_yzstar_abs: %.4f\n", I_yzstar_abs);
	}


	// --(2)--
	glm::vec3 v12_yzstar_norm = ((vecMod(v12touch_yzstar) < 0.0001f ) ? glm::vec3(0) : glm::normalize(v12touch_yzstar)), v21_yzstar_norm = -v12_yzstar_norm;
	// --(3)--
	glm::vec3 delta_L1(0), delta_L2(0);

	//if (I_yzstar_abs != 0) printf("I_yzstar_abs = %.4f\n", I_yzstar_abs);
	glm::vec3 delta_L1WC = I_yzstar_abs == 0 ? glm::vec3(0) : glm::cross(r1, I_yzstar_abs * v21_yzstar_norm);
	printVec3("delta_L1WC#1", delta_L1WC);
	delta_L1 == glm::vec3(glm::inverse(sphere->GetModelMatrix()) * glm::vec4(delta_L1WC, 0.0f));
	printVec3("delta_L1#1", delta_L1);
	// #INNORMAL when rotate on the desk, it's too small

	glm::vec3 delta_L2WC = glm::cross(r2, I_yzstar_abs * v12_yzstar_norm + I12_xstar);
	delta_L2 == glm::vec3(glm::inverse(cube->GetModelMatrix()) * glm::vec4(delta_L2WC, 0.0f));

	cInfo.angularImpulse1 = delta_L1;
	cInfo.angularImpulse2 = delta_L2;
	// #NOTE - for cube we cannot directly calculate the delta omega since the inertia moment is not static
	//		 - for sphere we can, but we don't



	glm::vec3 delta_v1(0), delta_v2(0);
	if (fsType == CollisionFreeStuckType::BothFree)
	{
		delta_v1 = I_yzstar_abs * v21_yzstar_norm / m1;
		delta_v2 = I_yzstar_abs * v12_yzstar_norm / m2;
	}
	else if (fsType == CollisionFreeStuckType::Stuck1st)
	{
		delta_v2 = I_yzstar_abs * v12_yzstar_norm / m2;
	}
	else if (fsType == CollisionFreeStuckType::Stuck2nd)
	{
		delta_v1 = I_yzstar_abs * v21_yzstar_norm / m1;
	}


	v1_after += delta_v1;
	v2_after += delta_v2;


	cInfo.v1After = v1_after;
	cInfo.v2After = v2_after;

	if (autoDeal)
	{
		DealCollision(sphere, cube, cInfo);
	}

	return cInfo;

}

// --------------------------------------------------------------------------------------

void CollideSph2Cube(std::vector<Object3Dsphere*> &spheres, std::vector<Object3Dcube*> &cubes, bool autoDeal)
{
	std::vector<Object3Dsphere*>::iterator sph_it = spheres.begin();
	std::vector<Object3Dcube*>::iterator cube_it = cubes.begin();

	for (/*sph_it = spheres.begin()*/; sph_it < spheres.end(); sph_it++)
	{
		for (cube_it = cubes.begin(); cube_it < cubes.end(); cube_it++)
			CollideSph2Cube(*sph_it, *cube_it, true);
	}

}

// --------------------------------------------------------------------------------------

void CollideSph2Sph(std::vector<Object3Dsphere*> &spheres, bool autoDeal)
{
	std::vector<Object3Dsphere*>::iterator sph1_it = spheres.begin();
	std::vector<Object3Dsphere*>::iterator sph2_it;

	for (; sph1_it < spheres.end(); sph1_it++)
	{
		for (sph2_it = sph1_it + 1; sph2_it < spheres.end(); sph2_it++)
		{
			CollideSph2Sph(*sph1_it, *sph2_it, autoDeal);
		}
	}
}

// --------------------------------------------------------------------------------------

void DealCollision(Object3D * obj1, Object3D * obj2, CollisionInfo cInfo)
{
	if (cInfo.relation == RelationType::Ambiguous)		// the most beautiful one
	{
		obj1->SetVelocity(cInfo.v1After);
		obj2->SetVelocity(cInfo.v2After);

		obj1->AddAngularMomentum(cInfo.angularImpulse1);
		obj2->AddAngularMomentum(cInfo.angularImpulse2);
	}
}

void CollideSph2Ground(Object3Dsphere * sphere, Object3Dcube * ground)
{

	float m_sph = sphere->GetMass();
	if (m_sph == 0) return;

	float x_wall_pos = ground->GetPosition().x + ground->GetWidth() / 2.0f;
	float x_wall_neg = ground->GetPosition().x - ground->GetWidth() / 2.0f;
	float y_surface = ground->GetPosition().y + ground->GetHeight() / 2.0f;
	float z_wall_pos = ground->GetPosition().z + ground->GetDepth() / 2.0f;
	float z_wall_neg = ground->GetPosition().z - ground->GetDepth() / 2.0f;

	glm::vec3 p_sph = sphere->GetPosition();
	float r_sph = sphere->GetRadius();

	sphere->IsTouchingDesk = false;


	// ----------------------------------------------
	// if outside of the ground or in the air, return

	if (p_sph.x > x_wall_pos || p_sph.x < x_wall_neg || p_sph.z > z_wall_pos || p_sph.z < z_wall_neg || p_sph.y - r_sph > y_surface + 5)
	{
		printf("#1\n");
		return;
	}
	printf("#2\n");


	// ---------------------------------------------------
	// if inside and touching ground, bounce it or slow it

	if (p_sph.y - r_sph <= y_surface)
	{

		glm::vec3 v_sph = sphere->GetVelocity();
		float g_y = std::min(0.0f, sphere->GetGravity().y);				// #NOTE this is negative

		// if the falling speed is fast, bounce it

		//printf("v_sph_y = %.4f\ng_y = %.4f\ndeltaTime = %.4f\n", v_sph.y, g_y, deltaTime);
		//printf("|v_sph_y| - |g_y * deltaTime * 2.0f| = %.4f\n", abs(v_sph.y) - abs(g_y * deltaTime * 2.0f));
		if (abs(v_sph.y) > abs(g_y * deltaTime * 1.0f))
		{
			printf("#3\n");
			CollideSph2Cube(sphere, ground, true);
			return;
		}

		sphere->IsTouchingDesk = true;

		// else if very slow, make it stop

		if (vecMod(v_sph) < 0.01f)
		{
			printf("#4\n");
			sphere->SetStatic();
			sphere->SetPosition(glm::vec3(p_sph.x, y_surface + r_sph, p_sph.z));
			return;
		}

		// else if gravity_y = 0, no friction

		if (g_y == 0) return;

		// else slow it by a = f * g
		
		glm::vec3 v_sph_dir = glm::normalize(v_sph);

		float f_sph = sphere->GetFriction(), f_ground = ground->GetFriction(), f = 0.5f * (f_ground + f_sph);
		
		glm::vec3 delta_v_sph = g_y * f * deltaTime * v_sph_dir;		// #NOTE the sign has been got wrong.. wtf..

		// if slow enough, stop it
		if (vecMod(delta_v_sph) > vecMod(v_sph))			// #NOTE the sign has been got wrong.. wtf!!
		{
			printf("#5\n");
			sphere->SetStatic();
			sphere->SetPosition(glm::vec3(p_sph.x, y_surface + r_sph, p_sph.z));
			return;
		}

		// else slow it

		printf("#6\n");
		sphere->AddVelocity(glm::vec3(delta_v_sph.x, -v_sph.y, delta_v_sph.z));
		printVec3(sphere->GetVelocity());
		sphere->SetPosition(glm::vec3(p_sph.x, y_surface + r_sph, p_sph.z));


		return;

	}

}

void CollideSph2Ground(std::vector<Object3Dsphere*> &spheres, Object3Dcube * ground)
{
	std::vector<Object3Dsphere*>::iterator sph_it = spheres.begin();

	for (; sph_it < spheres.end(); sph_it++)
	{
		CollideSph2Ground(*sph_it, ground);
	}
}



CollisionInfo CollideSph2Sph(Object3Dcylinder * sph1, Object3Dcylinder * sph2, bool autoDeal)
{
	/*
	// check mass
	// ----------

	float m1 = cy1->GetMass(), m2 = cy2->GetMass();
	CollisionFreeStuckType fsType = checkFStype(m1, m2);

	// return 0: both mass are 0
	if (fsType == CollisionFreeStuckType::BothStuck) return;


	// collision detect
	// ----------------

	// position (xz)
	glm::vec3 p_cy1 = cy1->GetPosition(), p_cy2 = cy2->GetPosition();
	glm::vec2 p1 = glm::vec2(p_cy1.x, p_cy1.z), p2 = glm::vec2(p_cy2.x, p_cy2.z);

	// radius
	float r1 = cy1->GetRadius(), r2 = cy2->GetRadius();
	glm::vec2 vec1to2 = p2 - p1;
	float dis = vecMod(vec1to2);

	// return 1: no collide or distance = 0
	if (dis == 0 || dis > r1 + r2) return;


	// collision calculation
	// ---------------------

	// step 1: center-to-center linear velocity

	// velocity (xz)
	glm::vec3 v_cy1 = cy1->GetVelocity(), v_cy2 = cy2->GetVelocity();
	glm::vec2 v1 = glm::vec2(v_cy1.x, v_cy1.z), v2 = glm::vec2(v_cy2.x, v_cy2.z);

	// direction unit vector from 1 to 2
	glm::vec2 xStar = glm::normalize(vec1to2);

	// velocity on x* direction
	float v1_xstar = glm::dot(v1, xStar), v2_xstar = glm::dot(v2, xStar);			// v1-->v2 as positive x direction

	// velocity not on x* direrction (which would not change in cylin to cylin collision
	glm::vec2 v1_subxstar = v1 - v1_xstar * xStar, v2_subxstar = v2 - v2_xstar * xStar;

	// return 2: overlap but moving apart
	if (v1_xstar < v2_xstar) return;

	// restitution
	float e = cy1->GetERestitution() * cy2->GetERestitution();

	// velocity on x* after collision
	float v1_xstar_prime, v2_xstar_prime, I_xstar_abs;
	glm::vec2 I21_xstar = glm::vec2(0), I12_xstar = glm::vec2(0);
	if (fsType == CollisionFreeStuckType::BothFree)
	{
		v1_xstar_prime = (m1 * v1_xstar + m2 * v2_xstar - m2 * e * (v1_xstar - v2_xstar)) / (m1 + m2);
		v2_xstar_prime = (m1 * v1_xstar + m2 * v2_xstar + m1 * e * (v1_xstar - v2_xstar)) / (m1 + m2);
		I21_xstar = m1 * (v1_xstar_prime - v1_xstar) * xStar;
		I12_xstar = -I21_xstar;
		I_xstar_abs = vecMod(I21_xstar);
	}
	else if (fsType == CollisionFreeStuckType::Stuck1st)
	{
		v1_xstar_prime = v1_xstar;
		v2_xstar_prime = (1 + e) * v1_xstar - e * v2_xstar;
		I12_xstar = m2 * (v2_xstar_prime - v2_xstar) * xStar;
		I_xstar_abs = vecMod(I12_xstar);
	}
	else if (fsType == CollisionFreeStuckType::Stuck2nd)
	{
		v1_xstar_prime = (1 + e) * v2_xstar - e * v1_xstar;
		v2_xstar_prime = v2_xstar;
		I21_xstar = m1 * (v1_xstar_prime - v1_xstar) * xStar;
		I_xstar_abs = vecMod(I21_xstar);
	}

	// step 2: calculate omega change

	// friction
	float f1 = cy1->GetFriction(), f2 = cy2->GetFriction();

	// omega (y)
	float w1 = cy1->GetOmega().y, w2 = cy2->GetOmega().y;

	// radius vector
	glm::vec2 r1_vec = xStar * r1, r2_vec = -xStar * r2;

	// calculate direction of relative velocity on the collide point
	glm::vec2 v1touch = v1 + w1 * glm::vec2(r1_vec.y, -r1_vec.x);		// v_angular = glm::cross(w, r)
	glm::vec2 v2touch = v2 + w2 * glm::vec2(r2_vec.y, -r2_vec.x);		// 'r_vec.y' here actually represents the z component
	glm::vec2 v12touch = v1touch - v2touch, v21touch = -v12touch;
	glm::vec2 v12touch_yzstar = v12touch - glm::dot(v12touch, xStar) * xStar, v21touch_yzstar = -v12touch_yzstar;*/

	CollisionInfo cInfo;
	CollisionFreeStuckType fsType;

	// ---------------
	// get information

	float m1 = sph1->GetMass(), m2 = sph2->GetMass();
	fsType = checkFStype(m1, m2);

	float r1_abs = sph1->GetRadius(), r2_abs = sph2->GetRadius();

	glm::vec3 p1 = sph1->GetPosition(), p2 = sph2->GetPosition();
	glm::vec3 v1 = sph1->GetVelocity(), v2 = sph2->GetVelocity();


	// --------------
	// detect collide

	glm::vec3 vec1to2 = p2 - p1;
	glm::vec3 xStar = glm::normalize(vec1to2);

	float dis = vecMod(vec1to2);														// #NOTE - length()ÊÇÖ¸ÏòÁ¿Î¬¶È!!!!(vec3.length() == 3)

	if (dis >= r1_abs + r2_abs)
	{
		// not collide
		cInfo.relation = RelationType::Stranger;
		return cInfo;
	}

	float v1_xstar = glm::dot(v1, xStar), v2_xstar = glm::dot(v2, xStar);			// v1-->v2 as positive x direction


	// #NOTE conditions like a ball stay on the desk
	if (dis < r1_abs + r2_abs/* && abs(v1_xstar - v2_xstar) < 0.01f*/)
	{
		glm::vec3 delta_p = (r1_abs + r2_abs - dis) * xStar;
		if (fsType == CollisionFreeStuckType::BothFree)
		{
			sph1->ChangePosition(-delta_p / 2.0f);
			sph2->ChangePosition(delta_p / 2.0f);
		}
		else if (fsType == CollisionFreeStuckType::Stuck1st)
		{
			sph2->ChangePosition(delta_p);
		}
		else if (fsType == CollisionFreeStuckType::Stuck2nd)
		{
			sph1->ChangePosition(-delta_p);
		}
	}


	if (v1_xstar < v2_xstar)
	{
		// not collide but overlap
		cInfo.relation = RelationType::Breaking;
		return cInfo;
	}

	cInfo.relation = RelationType::Ambiguous;


	// if collide and the speed smaller than specific value, make it stop
	// #TODO use makeStatic() to make it stop
	if (vecMod(v1) < 0.1f && vecMod(v2) < 0.1f)
	{
		cInfo.v1After = glm::vec3(0);
		cInfo.v2After = glm::vec3(0);
		cInfo.angularImpulse1 = glm::vec3(0);
		cInfo.angularImpulse2 = glm::vec3(0);
		if (autoDeal)
			DealCollision(sph1, sph2, cInfo);
		return cInfo;
	}

	// -----------------------------------------------------------
	// calculate velocity change caused by center-center collision

	glm::vec3 v1_subxstar = v1 - v1_xstar * xStar, v2_subxstar = v2 - v2_xstar * xStar;		// the remaining velocity would not change in the collision

	float e = sph1->GetERestitution() * sph2->GetERestitution();					// coefficient of restitution

	// velocity on the direction of center line    
	// #NOTE still need to consider the angular velocity!!!
	float v1_xstar_prime, v2_xstar_prime, I_xstar_abs;
	float vxDelta1, vxDelta2;
	glm::vec3 I21_xstar = glm::vec3(0), I12_xstar = glm::vec3(0);

	if (fsType == CollisionFreeStuckType::BothFree)
	{
		v1_xstar_prime = (m1 * v1_xstar + m2 * v2_xstar - m2 * e * (v1_xstar - v2_xstar)) / (m1 + m2);
		v2_xstar_prime = (m1 * v1_xstar + m2 * v2_xstar + m1 * e * (v1_xstar - v2_xstar)) / (m1 + m2);
		I21_xstar = m1 * (v1_xstar_prime - v1_xstar) * xStar;
		I12_xstar = -I21_xstar;
		// vxDelta1 = v1_xstar_prime - v1_xstar;
		I_xstar_abs = vecMod(I21_xstar);
	}
	else if (fsType == CollisionFreeStuckType::Stuck1st)
	{
		v1_xstar_prime = v1_xstar;
		v2_xstar_prime = (1 + e) * v1_xstar - e * v2_xstar;
		I12_xstar = m2 * (v2_xstar_prime - v2_xstar) * xStar;
		I_xstar_abs = vecMod(I12_xstar);
		// vxDelta2 = v2_xstar_prime - v2_xstar;
		// I_xstar_abs = abs(vxDelta2 * m2);
	}
	else if (fsType == CollisionFreeStuckType::Stuck2nd)
	{
		v1_xstar_prime = (1 + e) * v2_xstar - e * v1_xstar;
		v2_xstar_prime = v2_xstar;
		I21_xstar = m1 * (v1_xstar_prime - v1_xstar) * xStar;
		I_xstar_abs = vecMod(I21_xstar);
		// vxDelta1 = v1_xstar_prime - v1_xstar;
		// I_xstar_abs = abs(vxDelta1 * m1);
	}
	else if (fsType == CollisionFreeStuckType::BothStuck)
	{
		v1_xstar_prime = v1_xstar;
		v2_xstar_prime = v2_xstar;
		return cInfo;
	}


	// ----------------------
	// calculate omega change

	float f1 = sph1->GetFriction(), f2 = sph2->GetFriction();
	glm::vec3 r1 = xStar * r1_abs, r2 = -xStar * r2_abs;
	glm::vec3 w1LC = sph1->GetOmega(), w2LC = sph2->GetOmega();
	// #NOTE remember to translate LC coord to WC coord!!!
	glm::vec3 w1 = glm::vec3(sph1->GetModelMatrix() * glm::vec4(w1LC, 0.0f)), w2 = glm::vec3(sph2->GetModelMatrix() * glm::vec4(w2LC, 0.0f));

	// calculate direction of relative velocity on the collide point
	glm::vec3 v1touch = (w1 == glm::vec3(0.0f)) ? (v1) : (v1 + glm::cross(w1, r1));
	glm::vec3 v2touch = (w2 == glm::vec3(0.0f)) ? (v2) : (v2 + glm::cross(w2, r2));
	glm::vec3 v12touch = v1touch - v2touch, v21touch = -v12touch;
	glm::vec3 v12touch_yzstar = v12touch - glm::dot(v12touch, xStar) * xStar, v21touch_yzstar = -v12touch_yzstar;

	// glm::vec3 vTouch12Norm = glm::normalize(v1touch - v2touch), vTouch21Norm = -vTouch12Norm;

	// momentum change caused by yzstar part of the impulse

	glm::vec3 delta_L1(0), delta_L2(0);
	glm::vec3 v1_after = v1_xstar_prime * xStar + v1_subxstar;
	glm::vec3 v2_after = v2_xstar_prime * xStar + v2_subxstar;


	if (v12touch_yzstar == glm::vec3(0) || I_xstar_abs == 0)
	{
		cInfo.v1After = v1_after;
		cInfo.v2After = v2_after;
		cInfo.v1After.y = cInfo.v2After.y = 0;
		cInfo.angularImpulse1 = cInfo.angularImpulse2 = glm::vec3(0);

		if (autoDeal)
		{
			DealCollision(sph1, sph2, cInfo);
		}

		return cInfo;
	}

	// --(1)--
	float I1 = sph1->GetInertia(), I2 = sph2->GetInertia();
	float m_equivalent;

	if (fsType == CollisionFreeStuckType::BothFree)
	{
		m_equivalent = 1 / (((r1_abs * r1_abs) / (I1)+1 / m1) + ((r2_abs * r2_abs) / (I2 * I2) + 1 / m2));
	}
	else if (fsType == CollisionFreeStuckType::Stuck1st)
	{
		m_equivalent = 1 / ((r2_abs * r2_abs) / (I2)+1 / m2);
	}
	else if (fsType == CollisionFreeStuckType::Stuck2nd)
	{
		m_equivalent = 1 / ((r1_abs * r1_abs) / (I1)+1 / m1);
	}

	float I_yzstar_abs = std::min(0.5f * (f1 + f2) * I_xstar_abs, vecMod(v12touch_yzstar) * m_equivalent);

	// --(2)--
	glm::vec3 v12_yzstar_norm = glm::normalize(v12touch_yzstar), v21_yzstar_norm = -v12_yzstar_norm;
	// --(3)--
	delta_L1 = glm::cross(r1, I_yzstar_abs * v21_yzstar_norm);
	delta_L2 = glm::cross(r2, I_yzstar_abs * v12_yzstar_norm + I12_xstar);
	cInfo.angularImpulse1.y = delta_L1.y;
	cInfo.angularImpulse2.y = delta_L2.y;


	glm::vec3 delta_v1(0), delta_v2(0);
	if (fsType == CollisionFreeStuckType::BothFree)
	{
		delta_v1 = I_yzstar_abs * v21_yzstar_norm / m1;
		delta_v2 = I_yzstar_abs * v12_yzstar_norm / m2;
	}
	else if (fsType == CollisionFreeStuckType::Stuck1st)
	{
		delta_v2 = I_yzstar_abs * v12_yzstar_norm / m2;
	}
	else if (fsType == CollisionFreeStuckType::Stuck2nd)
	{
		delta_v1 = I_yzstar_abs * v21_yzstar_norm / m1;
	}

	v1_after += delta_v1;
	v2_after += delta_v2;

	cInfo.v1After = v1_after;
	cInfo.v2After = v2_after;
	cInfo.v1After.y = cInfo.v2After.y = 0;


	// --------------
	// deal collision

	if (autoDeal)
	{
		DealCollision(sph1, sph2, cInfo);
	}

	return cInfo;

}

void CollideSph2Sph(std::vector<Object3Dcylinder*> &spheres, bool autoDeal)
{
	std::vector<Object3Dcylinder*>::iterator sph1_it = spheres.begin();
	std::vector<Object3Dcylinder*>::iterator sph2_it;

	for (; sph1_it < spheres.end(); sph1_it++)
	{
		for (sph2_it = sph1_it + 1; sph2_it < spheres.end(); sph2_it++)
		{
			CollideSph2Sph(*sph1_it, *sph2_it, autoDeal);
		}
	}
}

CollisionInfo CollideSph2Wall(Object3Dcylinder * sphere, Object3Dcube * wall, bool autoDeal)
{
	CollisionInfo cInfo;
	CollisionFreeStuckType fsType;

	float m1 = sphere->GetMass();
	fsType = CollisionFreeStuckType::Stuck2nd;


	// ----------------------------------------------------------------
	// get current width, height, depth direction of the (rotated) cube

	glm::vec3 cubeX = glm::vec3(wall->GetRotationMatrix() * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));		// width direction
	glm::vec3 cubeY = glm::vec3(wall->GetRotationMatrix() * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));		// height direction
	glm::vec3 cubeZ = glm::vec3(wall->GetRotationMatrix() * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f));		// depth direction

	// normalize them to be the x, y, z unit vector

	glm::vec3 cubeXNorm = glm::normalize(cubeX);
	glm::vec3 cubeYNorm = glm::normalize(cubeY);
	glm::vec3 cubeZNorm = glm::normalize(cubeZ);


	// -------------
	// get cube size

	float width = wall->GetWidth(), height = wall->GetHeight(), depth = wall->GetDepth();
	float halfW = width / 2.0f, halfH = height / 2.0f, halfD = depth / 2.0f;
	glm::vec3 cubeHalfExtents(halfW, halfH, halfD);

	glm::vec3 vecCube2SphWC = sphere->GetPosition() - wall->GetPosition();					// world coordinate
	float cube2SphXLC = glm::dot(vecCube2SphWC, cubeXNorm);
	float cube2SphYLC = glm::dot(vecCube2SphWC, cubeYNorm);
	float cube2SphZLC = glm::dot(vecCube2SphWC, cubeZNorm);
	if (abs(cube2SphXLC) < halfW && abs(cube2SphYLC) < halfH && abs(cube2SphZLC) < halfD)
	{
		// the center of the circle is inside the cube
		cInfo.relation = RelationType::FallInLove;
		return cInfo;
	}


	glm::vec3 vecCube2SphLC = glm::vec3(cube2SphXLC, cube2SphYLC, cube2SphZLC);				// local coordinate
	glm::vec3 closestLC = glm::clamp(vecCube2SphLC, -cubeHalfExtents, cubeHalfExtents);		// closest point on cube in local coord
	glm::vec3 closestWC = glm::vec3(wall->GetModelMatrix() * glm::vec4(closestLC, 1.0f));	// closest point on cube in world coord
	glm::vec3 cube2ClosestWC = closestWC - wall->GetPosition();								// cube's center to closest point


	// ---------------
	// get sphere data

	glm::vec3 pSph = sphere->GetPosition();
	float rSph = sphere->GetRadius();


	// --------------
	// detect collide

	glm::vec3 closest2Sph = pSph - closestWC;
	float dis = vecMod(closest2Sph);
	if (dis > rSph)
	{
		// not collide
		cInfo.relation = RelationType::Stranger;
		return cInfo;
	}
	// if (dis < rSph && is moving apart relatively) { relation = Breaking; } 
	cInfo.relation = RelationType::Ambiguous;


	glm::vec3 v = sphere->GetVelocity();
	
	// determine which face is colliding

	CubeFace collideFace;
	glm::vec3 xStar;						// #NOTE xStar: sphere center to the collide point vector in WC
	if (closestLC.x == halfW) {
		collideFace = CubeFace::XPos;
		xStar = -cubeXNorm;
	}
	else if (closestLC.x == -halfW) {
		collideFace = CubeFace::XNeg;
		xStar = cubeXNorm;
	}
	else if (closestLC.y == halfH) {
		collideFace = CubeFace::YPos;
		xStar = -cubeYNorm;
	}
	else if (closestLC.y == -halfH) {
		collideFace = CubeFace::YNeg;
		xStar = cubeYNorm;
	}
	else if (closestLC.z == halfD) {
		collideFace = CubeFace::ZPos;
		xStar = -cubeZNorm;
	}
	else if (closestLC.z == -halfD) {
		collideFace = CubeFace::ZNeg;
		xStar = cubeZNorm;
	}


	// -----------------
	// start calculation

	// -- step1: velocity and impulse --

	float e = sphere->GetERestitution() * wall->GetERestitution();

	float v_xstar = glm::dot(v, xStar);

	glm::vec3 force_friction(0);
	// #NOTE conditions like a ball stay on the desk
	if (/*abs(v1_xstar - v2_xstar) < 0.1f &&*/ dis < rSph && abs(v_xstar) < 0.1f)
	{
		glm::vec3 delta_p = (rSph - dis) * xStar;
		sphere->ChangePosition(-delta_p);
	}


	if (v_xstar < 0)
	{
		// #NOTE - if we do not detect this, before the first collision finishes, the second one will be performed
		//		 - which will make the two objects look like stuck with each other
		cInfo.relation = RelationType::Breaking;
		return cInfo;
	}
	glm::vec3 v1_subxstar = v - v_xstar * xStar;


	float v1_xstar_prime, I_xstar_abs = 0;
	glm::vec3 I21_xstar = glm::vec3(0);
	
	v1_xstar_prime =  -e * v_xstar;
	I21_xstar = m1 * (v1_xstar_prime - v_xstar) * xStar;
	I_xstar_abs = vecMod(I21_xstar);
	

	// -- step2: angular velocity --

	glm::vec3 w1LC = sphere->GetOmega();
	// #NOTE remember to translate LC coord to WC coord!!!
	glm::vec3 w1 = glm::vec3(sphere->GetModelMatrix() * glm::vec4(w1LC, 0.0f));
	float f1 = sphere->GetFriction(), f2 = wall->GetFriction();
	glm::vec3 r1 = sphere->GetRadius() * xStar, r2 = cube2ClosestWC;					// #NOTE ÕâÀïÒ»¿ªÊ¼ÓÃglm::vec3 r1 = r1 * xStar; »áÓÐbug

	glm::vec3 v1touch = (w1 == glm::vec3(0.0f)) ? (v) : (v + glm::cross(w1, r1));
	glm::vec3 v12touch = v1touch, v21touch = -v12touch;


	glm::vec3 v12touch_yzstar = v12touch - glm::dot(v12touch, xStar) * xStar, v21touch_yzstar = -v12touch_yzstar;

	// momentum change caused by yzstar part of the impulse

	glm::vec3 v1_after = v1_xstar_prime * xStar + v1_subxstar;


	if (v12touch_yzstar == glm::vec3(0)/* || I_xstar_abs == 0*/)			// #INNORMAL when I_xstar_abs == 0, maybe a static friction
	{
		glm::vec3 angularV = glm::cross(w1, r1);

		cInfo.v1After = v1_after;
		cInfo.v2After = glm::vec3(0);
		cInfo.angularImpulse1 = cInfo.angularImpulse2 = glm::vec3(0);

		if (autoDeal)
		{
			DealCollision(sphere, wall, cInfo);
		}

		return cInfo;
	}


	// --(1)--
	float I1 = sphere->GetInertia();
	float r2_abs = vecMod(cube2ClosestWC);
	float m_equivalent;
	float force_N_abs = 0;

	m_equivalent = 1 / ((rSph * rSph) / (I1)+1 / m1);

	float I_yzstar_abs = std::min(0.5f * (f1 + f2) * I_xstar_abs, vecMod(v12touch_yzstar) * m_equivalent);

	// --(2)--
	glm::vec3 v12_yzstar_norm = ((vecMod(v12touch_yzstar) < 0.0001f) ? glm::vec3(0) : glm::normalize(v12touch_yzstar)), v21_yzstar_norm = -v12_yzstar_norm;
	// --(3)--
	glm::vec3 delta_L1(0);

	//if (I_yzstar_abs != 0) printf("I_yzstar_abs = %.4f\n", I_yzstar_abs);
	glm::vec3 delta_L1WC = I_yzstar_abs == 0 ? glm::vec3(0) : glm::cross(r1, I_yzstar_abs * v21_yzstar_norm);
	delta_L1 == glm::vec3(glm::inverse(sphere->GetModelMatrix()) * glm::vec4(delta_L1WC, 0.0f));		// #INNORMAL why delta_L1 = 0,0,0 ?

	//printVec3("delta_L1WC", delta_L1WC);
	//printVec3("delta_L1", delta_L1);

	cInfo.angularImpulse1 = delta_L1WC;


	glm::vec3 delta_v1 = I_yzstar_abs * v21_yzstar_norm / m1;

	v1_after += delta_v1;

	cInfo.v1After = v1_after;
	cInfo.v2After = glm::vec3(0);

	if (autoDeal)
	{
		DealCollision(sphere, wall, cInfo);
	}

	return cInfo;

}

void CollideSph2Wall(std::vector<Object3Dcylinder*> &spheres, std::vector<Object3Dcube*> &walls, bool autoDeal)
{
	std::vector<Object3Dcylinder*>::iterator sph_it = spheres.begin();
	std::vector<Object3Dcube*>::iterator wall_it;

	for (; sph_it < spheres.end(); sph_it++)
	{
		for (wall_it = walls.begin(); wall_it < walls.end(); wall_it++)
		{
			CollideSph2Wall(*sph_it, *wall_it, autoDeal);
		}
	}
}

CollisionInfo CollideSph2Sph(Object3Dcylinder * sph1, Object3Dsphere * sph2, bool autoDeal)
{
	CollisionInfo cInfo;
	CollisionFreeStuckType fsType;

	// ---------------
	// get information

	float m1 = sph1->GetMass(), m2 = sph2->GetMass();
	fsType = checkFStype(m1, m2);

	float r1_abs = sph1->GetRadius(), r2_abs = sph2->GetRadius();

	glm::vec3 p1 = sph1->GetPosition(), p2 = sph2->GetPosition();
	glm::vec3 v1 = sph1->GetVelocity(), v2 = sph2->GetVelocity();


	// --------------
	// detect collide

	glm::vec3 vec1to2 = p2 - p1;
	glm::vec3 xStar = glm::normalize(vec1to2);

	float dis = vecMod(vec1to2);														// #NOTE - length()ÊÇÖ¸ÏòÁ¿Î¬¶È!!!!(vec3.length()== 3)

	if (dis >= r1_abs + r2_abs)
	{
		// not collide
		cInfo.relation = RelationType::Stranger;
		return cInfo;
	}

	float v1_xstar = glm::dot(v1, xStar), v2_xstar = glm::dot(v2, xStar);			// v1-->v2 as positive x direction


	// #NOTE conditions like a ball stay on the desk
	if (dis < r1_abs + r2_abs/* && abs(v1_xstar - v2_xstar) < 0.01f*/)
	{
		glm::vec3 delta_p = (r1_abs + r2_abs - dis) * xStar;
		if (fsType == CollisionFreeStuckType::BothFree)
		{
			sph1->ChangePosition(-delta_p / 2.0f);
			sph2->ChangePosition(delta_p / 2.0f);
		}
		else if (fsType == CollisionFreeStuckType::Stuck1st)
		{
			sph2->ChangePosition(delta_p);
		}
		else if (fsType == CollisionFreeStuckType::Stuck2nd)
		{
			sph1->ChangePosition(-delta_p);
		}
	}


	if (v1_xstar < v2_xstar)
	{
		// not collide but overlap
		cInfo.relation = RelationType::Breaking;
		return cInfo;
	}

	cInfo.relation = RelationType::Ambiguous;


	// if collide and the speed smaller than specific value, make it stop
	// #TODO use makeStatic() to make it stop
	if (vecMod(v1) < 0.1f && vecMod(v2) < 0.1f)
	{
		cInfo.v1After = glm::vec3(0);
		cInfo.v2After = glm::vec3(0);
		cInfo.angularImpulse1 = glm::vec3(0);
		cInfo.angularImpulse2 = glm::vec3(0);
		if (autoDeal)
			DealCollision(sph1, sph2, cInfo);
		return cInfo;
	}

	// -----------------------------------------------------------
	// calculate velocity change caused by center-center collision

	glm::vec3 v1_subxstar = v1 - v1_xstar * xStar, v2_subxstar = v2 - v2_xstar * xStar;		// the remaining velocity would not change inthe collision

	float e = sph1->GetERestitution() * sph2->GetERestitution();					// coefficient of restitution

	// velocity on the direction of center line    
	// #NOTE still need to consider the angular velocity!!!
	float v1_xstar_prime, v2_xstar_prime, I_xstar_abs;
	float vxDelta1, vxDelta2;
	glm::vec3 I21_xstar = glm::vec3(0), I12_xstar = glm::vec3(0);

	if (fsType == CollisionFreeStuckType::BothFree)
	{
		v1_xstar_prime = (m1 * v1_xstar + m2 * v2_xstar - m2 * e * (v1_xstar - v2_xstar)) / (m1 + m2);
		v2_xstar_prime = (m1 * v1_xstar + m2 * v2_xstar + m1 * e * (v1_xstar - v2_xstar)) / (m1 + m2);
		I21_xstar = m1 * (v1_xstar_prime - v1_xstar) * xStar;
		I12_xstar = -I21_xstar;
		// vxDelta1 = v1_xstar_prime - v1_xstar;
		I_xstar_abs = vecMod(I21_xstar);
	}
	else if (fsType == CollisionFreeStuckType::Stuck1st)
	{
		v1_xstar_prime = v1_xstar;
		v2_xstar_prime = (1 + e) * v1_xstar - e * v2_xstar;
		I12_xstar = m2 * (v2_xstar_prime - v2_xstar) * xStar;
		I_xstar_abs = vecMod(I12_xstar);
		// vxDelta2 = v2_xstar_prime - v2_xstar;
		// I_xstar_abs = abs(vxDelta2 * m2);
	}
	else if (fsType == CollisionFreeStuckType::Stuck2nd)
	{
		v1_xstar_prime = (1 + e) * v2_xstar - e * v1_xstar;
		v2_xstar_prime = v2_xstar;
		I21_xstar = m1 * (v1_xstar_prime - v1_xstar) * xStar;
		I_xstar_abs = vecMod(I21_xstar);
		// vxDelta1 = v1_xstar_prime - v1_xstar;
		// I_xstar_abs = abs(vxDelta1 * m1);
	}
	else if (fsType == CollisionFreeStuckType::BothStuck)
	{
		v1_xstar_prime = v1_xstar;
		v2_xstar_prime = v2_xstar;
		return cInfo;
	}


	// ----------------------
	// calculate omega change

	float f1 = sph1->GetFriction(), f2 = sph2->GetFriction();
	glm::vec3 r1 = xStar * r1_abs, r2 = -xStar * r2_abs;
	glm::vec3 w1LC = sph1->GetOmega(), w2LC = sph2->GetOmega();
	// #NOTE remember to translate LC coord to WC coord!!!
	glm::vec3 w1 = glm::vec3(sph1->GetModelMatrix() * glm::vec4(w1LC, 0.0f)), w2 = glm::vec3(sph2->GetModelMatrix() * glm::vec4(w2LC, 0.0f));

	// calculate direction of relative velocity on the collide point
	glm::vec3 v1touch = (w1 == glm::vec3(0.0f)) ? (v1) : (v1 + glm::cross(w1, r1));
	glm::vec3 v2touch = (w2 == glm::vec3(0.0f)) ? (v2) : (v2 + glm::cross(w2, r2));
	glm::vec3 v12touch = v1touch - v2touch, v21touch = -v12touch;
	glm::vec3 v12touch_yzstar = v12touch - glm::dot(v12touch, xStar) * xStar, v21touch_yzstar = -v12touch_yzstar;

	// glm::vec3 vTouch12Norm = glm::normalize(v1touch - v2touch), vTouch21Norm = -vTouch12Norm;

	// momentum change caused by yzstar part of the impulse

	glm::vec3 delta_L1(0), delta_L2(0);
	glm::vec3 v1_after = v1_xstar_prime * xStar + v1_subxstar;
	glm::vec3 v2_after = v2_xstar_prime * xStar + v2_subxstar;


	if (v12touch_yzstar == glm::vec3(0) || I_xstar_abs == 0)
	{
		cInfo.v1After = v1_after;
		cInfo.v1After.y = 0;
		cInfo.v2After = v2_after;
		cInfo.angularImpulse1 = cInfo.angularImpulse2 = glm::vec3(0);

		if (autoDeal)
		{
			DealCollision(sph1, sph2, cInfo);
		}

		return cInfo;
	}

	// --(1)--
	float I1 = sph1->GetInertia(), I2 = sph2->GetInertia();
	float m_equivalent;

	if (fsType == CollisionFreeStuckType::BothFree)
	{
		m_equivalent = 1 / (((r1_abs * r1_abs) / (I1)+1 / m1) + ((r2_abs * r2_abs) / (I2 * I2) + 1 / m2));
	}
	else if (fsType == CollisionFreeStuckType::Stuck1st)
	{
		m_equivalent = 1 / ((r2_abs * r2_abs) / (I2)+1 / m2);
	}
	else if (fsType == CollisionFreeStuckType::Stuck2nd)
	{
		m_equivalent = 1 / ((r1_abs * r1_abs) / (I1)+1 / m1);
	}

	float I_yzstar_abs = std::min(0.5f * (f1 + f2) * I_xstar_abs, vecMod(v12touch_yzstar) * m_equivalent);

	// --(2)--
	glm::vec3 v12_yzstar_norm = glm::normalize(v12touch_yzstar), v21_yzstar_norm = -v12_yzstar_norm;
	// --(3)--
	delta_L1 = glm::cross(r1, I_yzstar_abs * v21_yzstar_norm);
	delta_L2 = glm::cross(r2, I_yzstar_abs * v12_yzstar_norm + I12_xstar);
	cInfo.angularImpulse1.y = delta_L1.y;
	cInfo.angularImpulse2 = delta_L2;


	glm::vec3 delta_v1(0), delta_v2(0);
	if (fsType == CollisionFreeStuckType::BothFree)
	{
		delta_v1 = I_yzstar_abs * v21_yzstar_norm / m1;
		delta_v2 = I_yzstar_abs * v12_yzstar_norm / m2;
	}
	else if (fsType == CollisionFreeStuckType::Stuck1st)
	{
		delta_v2 = I_yzstar_abs * v12_yzstar_norm / m2;
	}
	else if (fsType == CollisionFreeStuckType::Stuck2nd)
	{
		delta_v1 = I_yzstar_abs * v21_yzstar_norm / m1;
	}

	v1_after += delta_v1;
	v2_after += delta_v2;
	v1_after.y = 0;

	cInfo.v1After = v1_after;
	cInfo.v2After = v2_after;


	// --------------
	// deal collision

	if (autoDeal)
	{
		DealCollision(sph1, sph2, cInfo);
	}

	return cInfo;
}

void CollideSph2Sph(std::vector<Object3Dcylinder*> &spheres1, std::vector<Object3Dsphere*> &spheres2, bool autoDeal)
{
	std::vector<Object3Dcylinder*>::iterator sph1_it = spheres1.begin();
	std::vector<Object3Dsphere*>::iterator sph2_it;

	for (; sph1_it < spheres1.end(); sph1_it++)
	{
		for (sph2_it = spheres2.begin(); sph2_it < spheres2.end(); sph2_it++)
		{
			CollideSph2Sph(*sph1_it, *sph2_it, autoDeal);
		}
	}
}