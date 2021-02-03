#pragma once
//-----------------------------------------------------------------
// Filename: Camera.cpp
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Includes
//-----------------------------------------------------------------
#include "Camera.h"


// Class to hold and manipulate the values for the camera view.
Camera::Camera(XMFLOAT3 position, XMFLOAT3 at, XMFLOAT3 up, FLOAT windowWidth, FLOAT windowHeight,
	FLOAT nearDepth, FLOAT farDepth)
{
	m_positionX = position.x;
	m_positionX = position.y;
	m_positionX = position.z;

	m_eye = position;
	m_at = at;
	m_up = up;

	_windowWidth = windowWidth;
	_windowHeight = windowHeight;
	_nearDepth = nearDepth;
	_farDepth = farDepth;

	_view = XMFLOAT4X4();
	_projection = XMFLOAT4X4();
	
}


Camera::~Camera()
{

}


bool Camera::Update()
{
	return true;
}


void Camera::Reshape(FLOAT windowWidth, FLOAT windowHeight, FLOAT nearDepth, FLOAT farDepth)
{


}


//Movement functions
void Camera::MoveForward()
{
	m_eye.y -= CAM_SPEED;
}


void Camera::MoveBackward() 
{
	m_eye.y += CAM_SPEED;
}

void Camera::MoveUp()
{
	m_eye.z -= CAM_SPEED;
}

void Camera::MoveDown()
{
	m_eye.z += CAM_SPEED;
}


// Setters
void Camera::SetFrameTime(FLOAT time)
{
	m_frameTime = time;
	return;
}


void Camera::SetEye(XMFLOAT3 eye)
{
	m_eye = eye;
	return;
}


void Camera::SetLookAt(XMFLOAT3 at)
{
	m_at = at;
	return;
}


void Camera::SetUpValue(XMFLOAT3 up)
{
	m_up = up;
	return;
}


// Getters
XMFLOAT3 Camera::GetEye()
{
	return m_eye;
}


XMFLOAT3 Camera::GetLookAt()
{
	return m_at;
}


XMFLOAT3 Camera::GetUpValue()
{
	return m_up;
}


// Initialize the view matrix
XMFLOAT4X4 Camera::GetViewMatrix()
{
	XMVECTOR Eye = XMVectorSet(m_eye.x, m_eye.y, m_eye.z, 0.0f);
	XMVECTOR At = XMVectorSet(m_at.x, m_at.y, m_at.z, 0.0f);
	XMVECTOR Up = XMVectorSet(m_up.x, m_up.y, m_up.z, 0.0f);

	XMStoreFloat4x4(&_view, XMMatrixLookAtLH(Eye, At, Up));
	return _view;
}


// Initialize the projection matrix 
XMFLOAT4X4 Camera::GetProjectionMatrix()
{
	XMStoreFloat4x4(&_projection, XMMatrixPerspectiveFovLH(XM_PIDIV2, _windowWidth / (FLOAT)_windowHeight, _nearDepth, _farDepth));
	return _projection;
}


void Camera::Render()
{
	GetViewMatrix();



	//m_up.x = 0.0f;
	//m_up.y = 1.0f;
	//m_up.z = 0.0f;

	//m_positionX = m_PositionX;
	//m_positionY = m_PositionY;
	//m_positionZ = m_PositionZ;

	//lookAt.x = 0.0f;
	//lookAt.y = 0.0f;
	//lookAt.z = 1.0f;

	//pitch = m_RotationX * 0.0174532925f;
	//yaw = m_RotationY * 0.0174532925f;
	//roll = m_RotationZ * 0.0174532925f;

	//D3DXMatrixRotationYawPitchRoll(&rotationMatrix, yaw, pitch, roll);

	//D3DXVec3TransformCoord(&lookAt, &lookAt, &rotationMatrix);
	//D3DXVec3TransformCoord(&up, &up, &rotationMatrix);

	//lookAt = position + lookAt;

	//D3DXMatrixLookAtLH(&m_ViewMatrix, &position, &lookAt, &up);

	//return;
}