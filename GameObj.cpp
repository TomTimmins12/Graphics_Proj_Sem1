#include "GameObj.h"

GameObj::GameObj(string type, Appearance* appearance, Transform* transform) : _type(type)
{
	_appearance = appearance;
	_transform = transform;
}

GameObj::~GameObj()
{

}

void GameObj::Update(const float t)
{
	_transform->Update(t);
}

void GameObj::Draw(ID3D11DeviceContext* pImmediateContext)
{
	// NOTE: We are assuming that the constant buffers and all other draw setup has already taken place

	// Set vertex and index buffers
	Geometry geometry = _appearance->GetGeometry();
	pImmediateContext->IASetVertexBuffers(0, 1, &geometry.vertexBuffer, &geometry.vertexBufferStride, &geometry.vertexBufferOffset);
	pImmediateContext->IASetIndexBuffer(geometry.indexBuffer, DXGI_FORMAT_R16_UINT, 0);

	pImmediateContext->DrawIndexed(geometry.numberOfIndices, 0, 0);
}
