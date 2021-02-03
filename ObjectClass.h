///////////////////////////////////////////////
//Filename: ObjectClass.h
///////////////////////////////////////////////
#pragma once
//////////////
// INCLUDES //
//////////////
#include "Structures.h"

///////////////////////////////////////////////
//Classname: ObjectClass
///////////////////////////////////////////////
class ObjectClass
{
public:
    ObjectClass();
    ~ObjectClass();

    bool Initialise(ID3D11Device* device);
    void Shutdown();
    void Render(ID3D11DeviceContext* deviceContext);
    int GetIndexCount();

private:
    bool InitializeBuffers(ID3D11Device* device);
    void ShutdownBuffers();
    void RenderBuffers(ID3D11DeviceContext* deviceContext);

private:
    int m_vertexCount, m_indexCount;
    ID3D11Buffer* m_vertexBuffer, * m_indexBuffer;
    ID3D11Texture2D* _pObjTex;
    ID3D11ShaderResourceView* _pTextureRV = nullptr;
    ID3D11SamplerState* _pSamplerLinear = nullptr;
};

