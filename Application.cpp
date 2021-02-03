#include "Application.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch (message)
    {
        case WM_PAINT:
            hdc = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}


Application::Application()
{
    _hInst = nullptr;
	_hWnd = nullptr;
	_driverType = D3D_DRIVER_TYPE_NULL;
	_featureLevel = D3D_FEATURE_LEVEL_11_0;
	_pd3dDevice = nullptr;
	_pImmediateContext = nullptr;
	_pSwapChain = nullptr;
	_pRenderTargetView = nullptr;
	_pVertexShader = nullptr;
	_pPixelShader = nullptr;
	_pVertexLayout = nullptr;
	_pVertexBufferCube = nullptr;
	_pIndexBufferCube = nullptr;
	_pConstantBuffer = nullptr;

    _depthStencilView = nullptr;
    _depthStencilBuffer = nullptr;

    _wireFrame = nullptr;
    _wireFrameState = false;

    _pVertexBufferPyramid = nullptr;
    _pIndexBufferPyramid = nullptr;
    m_camera = nullptr;
    _pInputHandler = nullptr;
}

Application::~Application()
{
	Cleanup();
}


HRESULT Application::Initialise(HINSTANCE hInstance, int nCmdShow)
{
    if (FAILED(InitWindow(hInstance, nCmdShow)))
	{
        return E_FAIL;
	}

    RECT rc;
    GetClientRect(_hWnd, &rc);
    _WindowWidth = rc.right - rc.left;
    _WindowHeight = rc.bottom - rc.top;

    if (FAILED(InitDevice()))
    {
        Cleanup();

        return E_FAIL;
    }

	// Initialize the world matrix
	XMStoreFloat4x4(&_world, XMMatrixIdentity());

    // Initialize Input handler
    _pInputHandler = new Input(_hWnd);
    _pInputHandler->InitInput(hInstance);

    //Init Camera Values 
    XMFLOAT3 Eye = XMFLOAT3(0.0f, 10.0f, 0.0f);
    XMFLOAT3 At = XMFLOAT3(0.0f, -10.0f, 0.0f);
    XMFLOAT3 Up = XMFLOAT3(0.0f, 0.0f, -1.0f);

    m_camera = new Camera(Eye, At, Up, _WindowWidth, _WindowHeight, 0.01f, 100.0f);
    // Initialize _view as default camera
    _view = m_camera->GetViewMatrix();
    _projection = m_camera->GetProjectionMatrix();

    Eye = XMFLOAT3(0.0f, 0.0f, -10.0f);
    At = XMFLOAT3(0.0f, 0.0f, 0.0f);
    Up = XMFLOAT3(10.0f, 0.0f, 0.0f);
    
    m_topDownCamera = new Camera(Eye, At, Up, _WindowWidth, _WindowHeight, 0.01f, 100.0f);

	//XMStoreFloat4x4(&_view, XMMatrixLookAtLH(Eye, At, Up));

    // Initialize the projection matrix
	//XMStoreFloat4x4(&_projection, XMMatrixPerspectiveFovLH(XM_PIDIV2, _WindowWidth / (FLOAT) _WindowHeight, 0.01f, 100.0f));

	return S_OK;
}


HRESULT Application::InitShadersAndInputLayout()
{
	HRESULT hr;

    // Compile the vertex shader
    ID3DBlob* pVSBlob = nullptr;
    hr = CompileShaderFromFile(L"DX11 Framework.fx", "VS", "vs_4_0", &pVSBlob);

    if (FAILED(hr))
    {
        MessageBox(nullptr,
                   L"The FX file cannot be compiled. Please run this executable from the directory that contains the FX file. \nError in compiling Vertex shader.", L"Error", MB_OK);
        return hr;
    }

	// Create the vertex shader
	hr = _pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &_pVertexShader);

	if (FAILED(hr))
	{
		pVSBlob->Release();
        return hr;
	}

	// Compile the pixel shader
	ID3DBlob* pPSBlob = nullptr;
    hr = CompileShaderFromFile(L"DX11 Framework.fx", "PS", "ps_4_0", &pPSBlob);

    if (FAILED(hr))
    {
        MessageBox(nullptr,
                   L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.\nError compiling the pixel shader.", L"Error", MB_OK);
        return hr;
    }

	// Create the pixel shader
	hr = _pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &_pPixelShader);
	pPSBlob->Release();

    if (FAILED(hr))
        return hr;

    // Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        //{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT numElements = ARRAYSIZE(layout);

    // Create the input layout
	hr = _pd3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
                                        pVSBlob->GetBufferSize(), &_pVertexLayout);
	pVSBlob->Release();

	if (FAILED(hr))
        return hr;

    // Set the input layout
    _pImmediateContext->IASetInputLayout(_pVertexLayout);

	return hr;
}


HRESULT Application::InitVertexBuffer()
{
	HRESULT hr;

    // Create vertex buffer
    SimpleVertex CubeVertices[] =
    {
        //Front face -Z
        { XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },
        { XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },
        { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },
        { XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
        //Back face +Z
        { XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
        { XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },
        { XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },
        { XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },
        //Top Face +Y
        { XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
        { XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },
        { XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },
        { XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },
        //Bottom Face -y
        { XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },
        { XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },
        { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },
        { XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
        //Left face -x
        { XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },
        { XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },
        { XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },
        { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
        //Right face +x
        { XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
        { XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },
        { XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },
        { XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },
    };

    size_t vertSize = sizeof(CubeVertices) / sizeof(CubeVertices[0]);;
    CubeMatrixCount = vertSize;  // Dynamically set the size of the buffer rather than a constant in .h file
    for (size_t i = 0; i < vertSize; i++)
    {
        SimpleVertex v1 = CubeVertices[i];
        SimpleVertex v2 = CubeVertices[((i + 1) % vertSize)];
        CubeVertices[i] = GenerateCubeNormals(v1, v2);
    }
    


    //SimpleVertex PyramidVertices[] =
    //{
    //    // Square base of the pyramid
    //    { XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
    //    { XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
    //    { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
    //    { XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },

    //    // The tip of the pyramid
    //    { XMFLOAT3(0.0f, 0.0f, 2.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
    //};
    //
    //// This is not how the tutorial wanted the plane to be done, Look in to how the indices would work for a 4x4 plane
    //SimpleVertex PlaneVertices[] =
    //{
    //    { XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
    //    { XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
    //    { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
    //    { XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
    //};

    D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(CubeVertices);            // Set byteWidth for Cube
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));

    // Use cube vertices
    InitData.pSysMem = CubeVertices;

    // Create buffer holding Cube data
    hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pVertexBufferCube);

    //// Use pyramid vertices
    //InitData.pSysMem = PyramidVertices;
    //bd.ByteWidth = sizeof(PyramidVertices);           // Set byteWidth for pyramid

    //// Create buffer holding pyramid data
    //hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pVertexBufferPyramid);

    //// Use plane vertices
    //InitData.pSysMem = PlaneVertices;
    //bd.ByteWidth = sizeof(PlaneVertices);       // Set byteWidth for plane

    //// Create buffer holding plane data
    //hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pVertexBufferPlane);

    if (FAILED(hr))
        return hr;

	return S_OK;
}


HRESULT Application::InitIndexBuffer()
{
	HRESULT hr;
    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));

    // Create index buffer
    WORD CubeIndices[] =
    {
        0, 1, 2, //Side 1
        2, 1, 3,
        4, 0, 6, //Side 2
        6, 0, 2,
        7, 5, 6, //Side 3
        6, 5, 4,
        3, 1, 7, //Side 4
        7, 1, 5,
        4, 5, 0, //Side 5
        0, 5, 1,
        3, 7, 2, //Side 6
        2, 7, 6
    };
    size_t indCount = sizeof(CubeIndices) / sizeof(CubeIndices[0]);
    CubeIndCount = indCount;

    WORD PyramidIndices[] =
    {
        // Base Square
        0, 2, 1,
        1, 2, 3,
        // 4 Edges
        0, 1, 4,
        1, 3, 4,
        3, 2, 4,
        2, 0, 4,
    };

    WORD PlaneIndices[] =
    {
        0, 2, 1,
        1, 2, 3,
    };

    //vector<WORD> PlaneIndices = GeneratePlaneIndices(4, 4);

	//D3D11_BUFFER_DESC bd;     //Relocated at top of code
	//ZeroMemory(&bd, sizeof(bd));

    bd.Usage = D3D11_USAGE_DEFAULT;

    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));

    // Cube Indices
    InitData.pSysMem = CubeIndices;
    // Use Cube Indices
    bd.ByteWidth = sizeof(CubeIndices);         // Set bytewidth for cube
    hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pIndexBufferCube);

    // Pyramid Indices
    //initdata.psysmem = pyramidindices;
    // use pyramid indices
    //bd.bytewidth = sizeof(pyramidindices);      // set bytewidth for pyramid
    //hr = _pd3ddevice->createbuffer(&bd, &initdata, &_pindexbufferpyramid);

    // Plane Indices
    //InitData.pSysMem = PlaneIndices;
    // Use Plane Indices
    //bd.ByteWidth = sizeof(PlaneIndices);        // Set byte width for plane
    //hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pIndexBufferPlane);

    if (FAILED(hr))
        return hr;

	return S_OK;
}


HRESULT Application::InitWindow(HINSTANCE hInstance, int nCmdShow)
{
    // Register class
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_TUTORIAL1);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW );
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = L"TutorialWindowClass";
    wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_TUTORIAL1);
    if (!RegisterClassEx(&wcex))
        return E_FAIL;

    // Create window
    _hInst = hInstance;
    RECT rc = {0, 0, 640, 480};
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    _hWnd = CreateWindow(L"TutorialWindowClass", L"DX11 Framework", WS_OVERLAPPEDWINDOW,
                         CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
                         nullptr);
    if (!_hWnd)
		return E_FAIL;

    ShowWindow(_hWnd, nCmdShow);

    return S_OK;
}


HRESULT Application::CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows
    // the shaders to be optimized and to run exactly the way they will run in
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

    ID3DBlob* pErrorBlob;
    hr = D3DCompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel,
        dwShaderFlags, 0, ppBlobOut, &pErrorBlob);

    if (FAILED(hr))
    {
        if (pErrorBlob != nullptr)
            OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());

        if (pErrorBlob) pErrorBlob->Release();

        return hr;
    }

    if (pErrorBlob) pErrorBlob->Release();

    return S_OK;
}


HRESULT Application::InitDepthStencilView()
{
    HRESULT hr = S_OK;

    // Set up the dpeth/stencil buffer
    D3D11_TEXTURE2D_DESC depthStencilDesc;

    depthStencilDesc.Width = _WindowWidth;
    depthStencilDesc.Height = _WindowHeight;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.ArraySize = 1;
    depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilDesc.SampleDesc.Count = 1;
    depthStencilDesc.SampleDesc.Quality = 0;
    depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
    depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthStencilDesc.CPUAccessFlags = 0;
    depthStencilDesc.MiscFlags = 0;

    //D3D11_DEPTH_STENCIL_DESC dsDesc;

    //// Depth test parameters
    //dsDesc.DepthEnable = true;
    //dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    //dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

    //// Stencil test parameters
    //dsDesc.StencilEnable = true;
    //dsDesc.StencilReadMask = 0xFF;
    //dsDesc.StencilWriteMask = 0xFF;

    //// Stencil operations if pixel is front-facing
    //dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    //dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    //dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    //dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    //// Stencil operations if pixel is back-facing
    //dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    //dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    //dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    //dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    //// Create depth stencil state
    //ID3D11DepthStencilState* pDSState;
    //hr = _pd3dDevice->CreateDepthStencilState(&dsDesc, &pDSState);
    //if (FAILED(hr))
    //    return hr;

    //D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
    //descDSV.Format = depthStencilDesc.Format;
    //descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    //descDSV.Texture2D.MipSlice = 0;

    // Initialise depth/stencil buffers
    _pd3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, &_depthStencilBuffer);

    _pd3dDevice->CreateDepthStencilView(_depthStencilBuffer, nullptr, &_depthStencilView);

    _pImmediateContext->OMSetRenderTargets(1, &_pRenderTargetView, _depthStencilView);

    return S_OK;
}


HRESULT Application::InitDevice()
{
    HRESULT hr = S_OK;

    UINT createDeviceFlags = 0;

#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };

    UINT numDriverTypes = ARRAYSIZE(driverTypes);

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };

	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 1;
    sd.BufferDesc.Width = _WindowWidth;
    sd.BufferDesc.Height = _WindowHeight;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = _hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

    for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
    {
        _driverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDeviceAndSwapChain(nullptr, _driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
                                           D3D11_SDK_VERSION, &sd, &_pSwapChain, &_pd3dDevice, &_featureLevel, &_pImmediateContext);
        if (SUCCEEDED(hr))
            break;
    }

    if (FAILED(hr))
        return hr;

    // Create a render target view
    ID3D11Texture2D* pBackBuffer = nullptr;
    hr = _pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

    if (FAILED(hr))
        return hr;

    hr = _pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &_pRenderTargetView);
    pBackBuffer->Release();

    if (FAILED(hr))
        return hr;


    // Setup the viewport
    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)_WindowWidth;
    vp.Height = (FLOAT)_WindowHeight;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    _pImmediateContext->RSSetViewports(1, &vp);

	InitShadersAndInputLayout();

	InitVertexBuffer();

	InitIndexBuffer();

    InitDepthStencilView();

    // Set primitive topology
    _pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Create the constant buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
    hr = _pd3dDevice->CreateBuffer(&bd, nullptr, &_pConstantBuffer);



    // Fill out the wireframe rendering struct and initialise
    D3D11_RASTERIZER_DESC wfdesc;
    ZeroMemory(&wfdesc, sizeof(D3D11_RASTERIZER_DESC));
    wfdesc.FillMode = D3D11_FILL_WIREFRAME;
    wfdesc.CullMode = D3D11_CULL_NONE;
    hr = _pd3dDevice->CreateRasterizerState(&wfdesc, &_wireFrame);

    if (FAILED(hr))
        return hr;


    D3D11_TEXTURE2D_DESC desc;
    desc.Width = 256;
    desc.Height = 256;
    desc.MipLevels = desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    desc.MiscFlags = 0;
    _pd3dDevice->CreateTexture2D(&desc, nullptr, &_pcubeTex);
    _pd3dDevice->CreateShaderResourceView(_pcubeTex, nullptr, &_pTextureRV);
    CreateDDSTextureFromFile(_pd3dDevice, L"Crate_COLOR.dds", nullptr, &_pTextureRV); ///////////////////////////////
    // Set Texture to PS
    _pImmediateContext->PSSetShaderResources(0, 1, &_pTextureRV);


    // Create the sample state
    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory(&sampDesc, sizeof(sampDesc));
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    _pd3dDevice->CreateSamplerState(&sampDesc, &_pSamplerLinear);
    // Set samplers
    _pImmediateContext->PSSetSamplers(0, 1, &_pSamplerLinear);

    return S_OK;
}


void Application::Cleanup()
{
    if (_pImmediateContext) _pImmediateContext->ClearState();

    if (_pConstantBuffer) _pConstantBuffer->Release();
    if (_pVertexBufferCube) _pVertexBufferCube->Release();
    if (_pIndexBufferCube) _pIndexBufferCube->Release();
    if (_pVertexLayout) _pVertexLayout->Release();
    if (_pVertexShader) _pVertexShader->Release();
    if (_pPixelShader) _pPixelShader->Release();
    if (_pRenderTargetView) _pRenderTargetView->Release();
    if (_pSwapChain) _pSwapChain->Release();
    if (_pImmediateContext) _pImmediateContext->Release();
    if (_pd3dDevice) _pd3dDevice->Release();

    if (_depthStencilView) _depthStencilView->Release();
    if (_depthStencilBuffer) _depthStencilBuffer->Release();

    if (_wireFrame) _wireFrame->Release();

    if (_pVertexBufferPyramid) _pVertexBufferPyramid->Release();
    if (_pIndexBufferPyramid) _pIndexBufferPyramid->Release();

}


void Application::Update()
{
    // Update our time
    static float t = 0.0f;

    gtime = t;
    if (_driverType == D3D_DRIVER_TYPE_REFERENCE)
    {
        t += (float)XM_PI * 0.0125f;
    }
    else
    {
        static DWORD dwTimeStart = 0;
        DWORD dwTimeCur = GetTickCount64();

        if (dwTimeStart == 0)
            dwTimeStart = dwTimeCur;

        t = (dwTimeCur - dwTimeStart) / 1000.0f;
    }
    int vKey;
    if (GetAsyncKeyState('w'))
    {
        m_camera->MoveForward();
    }
    if (GetAsyncKeyState('s'))
    {
        m_camera->MoveBackward();
    }

    if (GetAsyncKeyState('q'))
    {
        //moveBackward(3);
    }
    if (GetAsyncKeyState('4'))
    {
        //moveBackward(4);
    }
    //switch(GetAsyncKeyState(vKey))
    //{
    //    case('5'):
    //    {
    //        std::cout << "Incorrect key pressed" << endl;
    //        break;
    //    }
    //    // Key A pressed
    //    case('a'):
    //    {

    //        break;
    //    }

    //    // Key D pressed
    //    case('d'):
    //    {
    //        break;
    //    }

    //    // Key W pressed
    //    case('w'):
    //    {
    //        m_camera->MoveForward();
    //        break;
    //    }

    //    // Key S pressed
    //    case('s'):
    //    {
    //        m_camera->MoveBackward();
    //        break;
    //    }

    //    // Key P pressed
    //    case('p'):
    //    {
    //        _view = m_topDownCamera->GetViewMatrix();
    //        _projection = m_topDownCamera->GetProjectionMatrix();
    //        _wireFrameState = true;
    //        break;
    //    }

    //    // Key O pressed
    //    case('o'):
    //    {
    //        _view = m_camera->GetViewMatrix();
    //        _projection = m_camera->GetProjectionMatrix();
    //        _wireFrameState = false;
    //        break;
    //    }

    //    // Key Q pressed
    //    case('q'):
    //    {
    //        m_camera->MoveUp();
    //        break;
    //    }

    //    //key E pressed
    //    case('e'):
    //    {
    //        m_camera->MoveDown();
    //        break;
    //    }
    //    default:
    //    {
    //        break;
    //    }
    //}

    if (m_camera->Update())
    {
        _view = m_camera->GetViewMatrix();
    }
    
    

    //
    // Animate the cubes
    //
	XMStoreFloat4x4(&_world, XMMatrixRotationZ(t*0.5));

    XMStoreFloat4x4(&_world2, XMMatrixRotationZ(t * 0.5) * XMMatrixTranslation(6.0f, 0.0f, 0.0f) * XMMatrixRotationZ(t));

    XMStoreFloat4x4(&_world3, XMMatrixScaling(0.5f, 0.5f, 0.5f) * XMMatrixRotationZ(t * 0.5) * XMMatrixTranslation(6.0f, 0.0f, 0.0f) * XMMatrixRotationZ(t) * XMMatrixTranslation(3.0f, 0.0f, 0.0f));
    
}


void Application::Draw()
{
    //
    // Clear the back buffer
    //
    float ClearColor[4] = {0.0f, 0.125f, 0.3f, 1.0f}; // red,green,blue,alpha
    _pImmediateContext->ClearRenderTargetView(_pRenderTargetView, ClearColor);

    //
    // Apply the depth/stencil buffer
    //
    _pImmediateContext->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);


    // Make evrything wireframe by having the state set to wireframe before anything is drawn
    // Use a bool to try change the state to the wireframes, errors inut side, research needed for console inputs
    // Make a keynord class to handle events properly, and add this funtionallity in the object class
    if (_wireFrameState) {
        _pImmediateContext->RSSetState(_wireFrame);
    }
    else {
        _pImmediateContext->RSSetState(nullptr);
    }

    //
    // Create the texture for the cube
    //
    //_cubeTex

	XMMATRIX world = XMLoadFloat4x4(&_world);
	XMMATRIX view = XMLoadFloat4x4(&_view);
	XMMATRIX projection = XMLoadFloat4x4(&_projection);
    
    // Better to have object store these in personal members as material will change from object to object
    // Hard coded values for now
    // Light direction from surface loaded in to struct (XYZ)
    m_dirLight.Direction = XMFLOAT3(0.0f, 0.5f, -1.0f);
    // Diffuse Light colour (RGBA)
    m_dirLight.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    // Ambient light colour (RGBA)
    m_dirLight.Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    // Specular light colour(RGBA)
    m_dirLight.Specular = XMFLOAT4(0.2f, 0.0f, 0.8f, 1.0f);

    // Diffuse material properties (RGBA)
    diffuseMaterial = XMFLOAT4(0.8f, 0.5f, 0.5f, 1.0f);
    // Ambient material properties (RGBA)
    ambientMaterial = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
    // Specular material properties (RGBA)
    specularMaterial = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);

    // Specular power (float)
    specularPower = 5.0f;
    // Eye pos
    eyePos = m_camera->GetEye();

    //
    // Update variables
    //
    // Directional light
    ConstantBuffer cb;
	cb.mWorld = XMMatrixTranspose(world);
	cb.mView = XMMatrixTranspose(view);
	cb.mProjection = XMMatrixTranspose(projection);
    cb.DiffuseMtrl = diffuseMaterial;
    cb.AmbientMtrl = ambientMaterial;
    cb.SpecularMtrl = specularMaterial;

    cb.gDirLight = m_dirLight;
    cb.gPointLight = m_pointLight;
    cb.gSpotLight = m_spotLight;
    //cb.SpecularPower = specularPower;
    cb.EyePosW = eyePos;
    cb.gTime = gtime;

    // Set Index and Vertex buffers in draw function
    UINT stride = sizeof(SimpleVertex);
    UINT offset = 0;
    // Set vertex buffer for first object
    _pImmediateContext->IASetVertexBuffers(0, 1, &_pVertexBufferCube, &stride, &offset);
    // Set index buffer for first object
    _pImmediateContext->IASetIndexBuffer(_pIndexBufferCube, DXGI_FORMAT_R16_UINT, 0);

    // Set constant buffer
	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
    
    // Draws the first object
	_pImmediateContext->VSSetConstantBuffers(0, 1, &_pConstantBuffer);
    _pImmediateContext->PSSetConstantBuffers(0, 1, &_pConstantBuffer);
    _pImmediateContext->VSSetShader(_pVertexShader, nullptr, 0);
	_pImmediateContext->PSSetShader(_pPixelShader, nullptr, 0);
	_pImmediateContext->DrawIndexed(CubeIndCount, 0, 0);

    // Draw the second object
    world = XMLoadFloat4x4(&_world2);
    cb.mWorld = XMMatrixTranspose(world);
    _pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
    _pImmediateContext->DrawIndexed(CubeIndCount, 0, 0);


    // Change the vetex and index buffers to the pyramid mesh
    //_pImmediateContext->IASetVertexBuffers(0, 1, &_pVertexBufferPyramid, &stride, &offset);
    //_pImmediateContext->IASetIndexBuffer(_pIndexBufferPyramid, DXGI_FORMAT_R16_UINT, 0);

    // Draw third object
    world = XMLoadFloat4x4(&_world3);
    cb.mWorld = XMMatrixTranspose(world);
    _pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);

    _pImmediateContext->DrawIndexed(CubeIndCount, 0, 0);
    //_pImmediateContext->DrawIndexed(PyramidIndCount, 0, 0);


    //
    // Present our back buffer to our front buffer
    //
    _pSwapChain->Present(0, 0);
}


//Uses the Vector3D class to generate the cross-product of two vectors
SimpleVertex Application::GenerateCubeNormals(SimpleVertex v1, SimpleVertex v2)
{
    Vector3D vec1 = Vector3D(v1.Pos.x, v1.Pos.y, v1.Pos.z);     //Create instance of Vector3D using XMFLOAT3s
    Vector3D vec2 = Vector3D(v2.Pos.x, v2.Pos.y, v2.Pos.z);
    Vector3D NormVec = vec1.cross_product(vec2);        //Use cross-product function
    //NormVec.Normalization();    //Normalize result

    SimpleVertex vec3 = { XMFLOAT3(v1.Pos.x, v1.Pos.y, v1.Pos.z), XMFLOAT3(NormVec.x, NormVec.y, NormVec.z), XMFLOAT2(v1.TexC.x, v1.TexC.y) };
    return vec3;
}