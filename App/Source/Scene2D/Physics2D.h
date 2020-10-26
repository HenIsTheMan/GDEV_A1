#pragma once
#include <includes/glm.hpp>
#include <includes/gtc/matrix_transform.hpp>
#include <includes/gtc/type_ptr.hpp>

class CPhysics2D final{
public:
	enum class STATUS{
		IDLE,
		JUMP,
		FALL,
		NUM
	};
	CPhysics2D();
	void Update();
	void AddElapsedTime(const float fElapseTime);
	float CalculateDist(glm::vec2 source, glm::vec2 destination);

	///Setters
	void SetInitialVelocity(const glm::vec2 v2InitialVelocity);
	void SetFinalVelocity(const glm::vec2 v2FinalVelocity);
	void SetAcceleration(const glm::vec2 v2Acceleration);
	void SetDisplacement(const glm::vec2 v2Displacement);
	void SetTime(const float fTime);
	void SetStatus(const STATUS sStatus);

	///Getters
	glm::vec2 GetInitialVelocity() const;
	glm::vec2 GetFinalVelocity() const;
	glm::vec2 GetAcceleration() const;
	glm::vec2 GetDisplacement() const;
	float GetTime() const;
	STATUS GetStatus() const;
private:
	void Reset();
	glm::vec2 v2InitialVelocity;
	glm::vec2 v2FinalVelocity;
	glm::vec2 v2Acceleration;
	glm::vec2 v2Displacement;
	float fTime;
	const glm::vec2 gravAccel = glm::vec2(0.f, -3.f);
	STATUS sCurrentStatus;
};