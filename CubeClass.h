///////////////////////////////////////////////
//Filename: CubeClass.h
///////////////////////////////////////////////
#ifndef _CUBECLASS_H_
#define _CUBECLASS_H_
//////////////
// INCLUDES //
//////////////
#include "Includes.h"

///////////////////////////////////////////////
//Classname: CubeClass
///////////////////////////////////////////////
class CubeClass
{
private:
    struct SimpleVertex
    {
        XMFLOAT3 Pos;
        XMFLOAT3 Normal;
        XMFLOAT2 TexC;
    };

public:
    CubeClass();
    CubeClass(const CubeClass&);
    ~CubeClass();

    bool Initialize(ID3D11Device* device);
    void Render(ID3D11DeviceContext* deviceContext);

    int GetIndexCount();
private:
    bool InitializeBuffers(ID3D11Device*);
    void ShutdownBuffers();
    void RenderBuffers(ID3D11DeviceContext*);

private:
    int m_vertexCount, m_indexCount;
    ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;

};

#endif