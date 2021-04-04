#pragma once
#include <directxmath.h>
#include <d3d11_1.h>
#include <string>

using namespace DirectX;
using namespace std;

class Transform
{
public:
	Transform();
	~Transform();

	void Update(float t);

	// Getters for position/rotation/scale
	XMFLOAT3 GetPosition() const { return _position; }

	XMFLOAT3 GetScale() const { return _scale; }

	XMFLOAT3 GetRotation() const { return _rotation; }

	// Setters for position/rotation/scale
	void SetPosition(XMFLOAT3 position) { _position = position; }
	void SetPosition(float x, float y, float z) { _position.x = x; _position.y = y; _position.z = z; }

	void SetScale(XMFLOAT3 scale) { _scale = scale; }
	void SetScale(float x, float y, float z) { _scale.x = x; _scale.y = y; _scale.z = z; }

	void SetRotation(XMFLOAT3 rotation) { _rotation = rotation; }
	void SetRotation(float x, float y, float z) { _rotation.x = x; _rotation.y = y; _rotation.z = z; }

	XMMATRIX GetWorldMatrix() const { return XMLoadFloat4x4(&_world); }

private:
	XMFLOAT3 _position;
	XMFLOAT3 _rotation;
	XMFLOAT3 _scale;

	XMFLOAT4X4 _world;
};