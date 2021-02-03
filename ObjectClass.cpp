///////////////////////////////////////////////
//Filename: ObjectClass.cpp
///////////////////////////////////////////////
#include "ObjectClass.h"

ObjectClass::ObjectClass()
{
}

ObjectClass::~ObjectClass()
{
}

bool ObjectClass::Initialise(ID3D11Device* device)
{
	return false;
}

void ObjectClass::Shutdown()
{
}

void ObjectClass::Render(ID3D11DeviceContext* deviceContext)
{
}

int ObjectClass::GetIndexCount()
{
	return 0;
}

bool ObjectClass::InitializeBuffers(ID3D11Device* device)
{
	return false;
}

void ObjectClass::ShutdownBuffers()
{
}

void ObjectClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
}
