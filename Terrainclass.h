#pragma once
//////////////////////////////////////////
// Filename: Terrainclass.h
//////////////////////////////////////////

// INCLUDES
#include "Includes.h"

//////////////////////////////////////////
// Class Name: TerrainClass
//////////////////////////////////////////
class TerrainClass
{
private:

    struct SimpleVertex
    {
        XMFLOAT3 Pos;
        XMFLOAT4 Color;
    };

public:

    TerrainClass();
    TerrainClass(const TerrainClass& other);
    ~TerrainClass();

    bool Initialise(ID3D11Device* device);
    void Shutdown();
    void Render(ID3D11DeviceContext* deviceContext);

    int GetIndexCount();

private:
    bool InitializeBuffers(ID3D11Device* device);
    void ShutdownBuffers();
    void RenderBuffers(ID3D11DeviceContext* deviceContext);

private:
    int m_terrainWidth, m_terrainHeight;
    int m_vertexCount, m_indexCount;
    ID3D11Buffer* m_vertexBuffer, * m_indexBuffer;
};
