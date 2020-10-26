#include "Physics2D.h"
#include <iostream>
using namespace std;

CPhysics2D::CPhysics2D(): fTime(0.f), sCurrentStatus(STATUS::IDLE){
	v2Displacement = v2InitialVelocity = v2FinalVelocity = v2Acceleration = glm::vec2(0.f);
}

void CPhysics2D::Reset(){
	v2Displacement = v2InitialVelocity = v2FinalVelocity = glm::vec2(0.f); //No need to reset accel
	fTime = 0.f;
}

void CPhysics2D::SetInitialVelocity(const glm::vec2 v2InitialVelocity){
	this->v2InitialVelocity = v2InitialVelocity;
}

void CPhysics2D::SetFinalVelocity(const glm::vec2 v2FinalVelocity){
	this->v2FinalVelocity = v2FinalVelocity;
}

void CPhysics2D::SetAcceleration(const glm::vec2 v2Acceleration){
	this->v2Acceleration = v2Acceleration;
}

void CPhysics2D::SetDisplacement(const glm::vec2 v2Displacement){
	this->v2Displacement = v2Displacement;
}

void CPhysics2D::SetTime(const float fTime){
	this->fTime = fTime;
}

void CPhysics2D::SetStatus(const STATUS sStatus){
	if(sCurrentStatus != sStatus){
		sCurrentStatus = sStatus;
		Reset();
	}
}

glm::vec2 CPhysics2D::GetInitialVelocity() const{
	return v2InitialVelocity;
}

glm::vec2 CPhysics2D::GetFinalVelocity() const{
	return v2FinalVelocity;
}

glm::vec2 CPhysics2D::GetAcceleration() const{
	return v2Acceleration;
}

glm::vec2 CPhysics2D::GetDisplacement() const{
	return v2Displacement;
}

float CPhysics2D::GetTime() const{
	return fTime;
}

CPhysics2D::STATUS CPhysics2D::GetStatus() const{
	return sCurrentStatus;
}

void CPhysics2D::Update(){
	if(sCurrentStatus == STATUS::JUMP || sCurrentStatus == STATUS::FALL){
		v2FinalVelocity = v2InitialVelocity + gravAccel * fTime;
		v2FinalVelocity.y = v2FinalVelocity.y > 5.f ? 5.f : v2FinalVelocity.y; //Limit jump vel
		v2FinalVelocity.y = v2FinalVelocity.y < -2.f ? -2.f : v2FinalVelocity.y; //Limit fall vel
		v2Displacement = v2FinalVelocity * fTime - .5f * gravAccel * fTime * fTime;
		v2InitialVelocity = v2FinalVelocity;
	} else{
		v2FinalVelocity = v2InitialVelocity + v2Acceleration * fTime;
		v2Displacement = v2FinalVelocity * fTime - .5f * v2Acceleration * fTime * fTime;
		v2InitialVelocity = v2FinalVelocity;
	}
}

void CPhysics2D::AddElapsedTime(const float fElapseTime){
	fTime += fElapseTime;
}

float CPhysics2D::CalculateDist(glm::vec2 source, glm::vec2 destination){
	return glm::length(destination - source);
}