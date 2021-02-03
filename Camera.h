////////////////////////////////////////////////////////////////////////////////
// Filename: Camera.h
////////////////////////////////////////////////////////////////////////////////
#pragma once

//////////////
// INCLUDES //
//////////////
#include "Includes.h"

#define CAM_SPEED 0.001;

////////////////////////////////////////////////////////////////////////////////
// Class name: Camera
////////////////////////////////////////////////////////////////////////////////
class Camera
{
private:
	float m_positionX, m_positionY, m_positionZ;
	float m_rotationX, m_rotationY, m_rotationZ;

	float m_frameTime;

	float m_forwardSpeed, m_backwardSpeed;
	float m_upwardSpeed, m_downwardSpeed;
	float m_leftTurnSpeed, m_rightTurnSpeed;
	float m_lookUpSpeed, m_lookDownSpeed;

	float m_pitch;	// = m_RotationX * 0.0174532925f;
	float m_yaw;	 //= m_RotationY * 0.0174532925f;
	float m_roll;	// = m_RotationZ * 0.0174532925f;


	// Priavte values will store the camera position and view volume
	XMFLOAT3 m_eye;
	XMFLOAT3 m_at;
	XMFLOAT3 m_up;

	FLOAT _windowWidth;
	FLOAT _windowHeight;
	FLOAT _nearDepth;
	FLOAT _farDepth;

	// Attributesto hold the view and projection matrices which 
	// Will be passed to the shader
	XMFLOAT4X4 _view;
	XMFLOAT4X4 _projection;

public:
	//Constructor and destructor for the camera
	Camera(XMFLOAT3 position, XMFLOAT3 at, XMFLOAT3 up, FLOAT windowWidth, FLOAT windowHeight,
		FLOAT nearDepth, FLOAT farDepth);
	~Camera();

	// update function to make the current view and projection 
	// matrices
	bool Update();
	// Reshape function to alter camera values should the window size change
	void Reshape(FLOAT windowWidth, FLOAT windowHeight, FLOAT nearDepth, FLOAT farDepth);

	void SetEye(XMFLOAT3);
	void SetLookAt(XMFLOAT3);
	void SetUpValue(XMFLOAT3);

	void SetFrameTime(FLOAT);

	// Return camera position values
	XMFLOAT3 GetEye();
	XMFLOAT3 GetLookAt();
	XMFLOAT3 GetUpValue();

	XMFLOAT4X4 GetViewMatrix();
	XMFLOAT4X4 GetProjectionMatrix();

	void Render();

	void MoveForward();
	void MoveBackward();
	void MoveUp();
	void MoveDown();
	void SlowDown();

	void StrafeLeft();
	void StrafeRight();

	void RollRight();
	void RollLeft();

	void PitchRight();
	void PitchLeft();

};

