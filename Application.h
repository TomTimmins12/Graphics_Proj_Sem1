#pragma once
//
// Includes
//
#include "Structures.h" // Includes Includes.h and Directx namespace declaration
// Not to be put in includes as other classies will use includes.h
#include "Vector3D.h"
#include "Camera.h"
#include "CubeClass.h"
#include "DDSTextureLoader.h"
#include "OBJLoader.h"
#include "Matrix.h"
#include "Terrainclass.h"
#include "GameObj.h"

struct ConstantBuffer
{
	XMMATRIX mWorld;	//4
	XMMATRIX mView;		//4
	XMMATRIX mProjection;		//4

	SurfaceInfo gSurface; //4
	DirectionalLight gDirLight;		//4
	PointLight gPointLight;		//4
	SpotLight gSpotLight;		//4

	XMFLOAT3 EyePosW;		//4
	float HasTexture;
};
// Assert that the constant buffer remains 16-byte aligned. Prevents errors based aroubng the cb in the shaders
static_assert((sizeof(ConstantBuffer) % 16) == 0, "Constant Buffer size must be 16-byte aligned");


class Application // Messy need to make more classes to split the members and functions in to
{
private:
	HINSTANCE               _hInst;
	HWND                    _hWnd;
	D3D_DRIVER_TYPE         _driverType;
	D3D_FEATURE_LEVEL       _featureLevel;
	ID3D11Device*           _pd3dDevice;
	ID3D11DeviceContext*    _pImmediateContext;
	IDXGISwapChain*         _pSwapChain;
	ID3D11RenderTargetView* _pRenderTargetView;
	ID3D11VertexShader*     _pVertexShader;
	ID3D11PixelShader*      _pPixelShader;
	ID3D11InputLayout*      _pVertexLayout;

	// Cube Buffers
	ID3D11Buffer*           _pVertexBufferCube;
	ID3D11Buffer*           _pIndexBufferCube;
	// Pyramid buffers
	ID3D11Buffer*			_pVertexBufferPyramid;   
	ID3D11Buffer*			_pIndexBufferPyramid;
	// Plane Buffers
	ID3D11Buffer*			_pVertexBufferPlane;
	ID3D11Buffer*			_pIndexBufferPlane;

	ID3D11Buffer*           _pConstantBuffer;

	ID3D11RasterizerState* _wireFrame;
	bool _wireFrameState;

	ID3D11DepthStencilView* _depthStencilView = nullptr;
	ID3D11Texture2D* _depthStencilBuffer = nullptr;

	UINT _renderHeight = 1080;
	UINT _renderWidth = 1920;

	ID3D11DepthStencilState* DSLessEqual;
	ID3D11RasterizerState* RSCullNone;

	ID3D11RasterizerState* CCWcullMode;
	ID3D11RasterizerState* CWcullMode;

	Camera* m_camera; 
	Camera* m_topDownCamera;
	float _cameraOrbitRadius = 7.0f;
	float _cameraOrbitRadiusMin = 2.0f;
	float _cameraOrbitRadiusMax = 50.0f;
	float _cameraOrbitAngleXZ = -90.0f;
	float _cameraSpeed = 2.0f;


	// Values used to set the byte widths so that the correct amount of triangles are drawn
	// These should be moved to classes when approprioate
	int CubeMatrixCount = 0;
	int CubeIndCount = 0;
	int PyramidMatrixCount = 5;
	int PyramidIndCount = 18;
	int PlaneMatrixCount = 4;
	int PlaneIndCount = 6;

	float gtime;	//To be made equal to t in the update function to pass in to CB for transformations


	
	// Lighting variables to pass in to constant buffer.
	// Possible to make a light class to make this neater.
	// Put lighting and material structs in the object class.
	DirectionalLight m_dirLight;
	PointLight m_pointLight;
	SpotLight m_spotLight;

	SurfaceInfo m_surface;

	MeshData objMeshData;

	vector<GameObj*> _gameObjects;

	Appearance* _appearance;
	Transform* _transform;

	//XMFLOAT4 diffuseLight;
	//XMFLOAT4 ambientLight;
	//XMFLOAT4 specularLight;

	float specularPower;
	XMFLOAT3 eyePos;

	//Texturing
	ID3D11Texture2D* _pcubeTex;
	ID3D11ShaderResourceView* _pTextureRV = nullptr;
	ID3D11SamplerState* _pSamplerLinear = nullptr;

	// Code for class is incompatible with the rest 
	//TerrainClass* m_terrain;
	

private:
	HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
	HRESULT InitDevice();
	void Cleanup();
	HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
	HRESULT InitShadersAndInputLayout();
	HRESULT InitVertexBuffer();
	HRESULT InitIndexBuffer();


	UINT _WindowHeight;
	UINT _WindowWidth;

public:
	Application();
	~Application();

	HRESULT Initialise(HINSTANCE hInstance, int nCmdShow);

	void Update();
	void Draw();

	bool HandleKeyboard(MSG msg);

	// Similar function to be used for other shapes
	// Functional for any object so long as loop is correct in initialisation
	//perhaps a better way than using the function in a loop, passing in an object to calculate normals
	// Uses Newalls Method to calculate the normals and the Vector3D class for calculations, call a class to do the maths as opposed to bloating this function
	SimpleVertex GenerateCubeNormals(SimpleVertex v1, SimpleVertex v2); // Should be done in the object class
};