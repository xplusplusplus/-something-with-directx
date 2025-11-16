#include "GraphicsEngine.h"
#include "SwapChain.h"
#include "DeviceContext.h"
#include "VertexBuffer.h"
#include "VertexShader.h"

#include "d3dcompiler.h"
#include <iostream>

GraphicsEngine::GraphicsEngine()
{
}

bool GraphicsEngine::init()
{
    D3D_DRIVER_TYPE driver_types[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE
    };
    UINT num_driver_types = ARRAYSIZE(driver_types);

    D3D_FEATURE_LEVEL feature_levels[] =
    {
        D3D_FEATURE_LEVEL_11_0
    };
    UINT num_feature_levels = ARRAYSIZE(feature_levels);
    HRESULT res = 0;

    ID3D11DeviceContext* m_context;
    for (UINT driver_type_index = 0; driver_type_index < num_driver_types;)
    {
        res = D3D11CreateDevice(NULL, driver_types[driver_type_index],
            NULL, NULL, feature_levels, num_feature_levels, D3D11_SDK_VERSION,
            &m_device, &m_feature_level, &m_context);
        if (SUCCEEDED(res))
            break;
        ++driver_type_index;
    }

    if (FAILED(res))
        return false;

    m_device_context = new DeviceContext(m_context);

    m_device->QueryInterface(__uuidof(IDXGIDevice), (void**)&m_dxgi_device);
    m_dxgi_device->GetParent(__uuidof(IDXGIAdapter), (void**)&m_dxgi_adapter);
    m_dxgi_adapter->GetParent(__uuidof(IDXGIFactory), (void**)&m_dxgi_factory);

    return true;
}

bool GraphicsEngine::release()
{
	m_dxgi_device->Release();
	m_dxgi_adapter->Release();
	m_dxgi_factory->Release(); 

	m_device_context->release();
	m_device->Release();

    if (m_vs) m_vs->Release();
    if (m_ps) m_ps->Release();
    if (m_vsblob) m_vsblob->Release();
    if (m_psblob) m_psblob->Release();

	return true;
}

GraphicsEngine::~GraphicsEngine()
{
}

SwapChain* GraphicsEngine::createSwapChain()
{
	return new SwapChain();
}

DeviceContext* GraphicsEngine::getDevieContext()
{
	return this->m_device_context;
}

VertexBuffer* GraphicsEngine::createVertexBuffer()
{
    return new VertexBuffer();
}

VertexShader* GraphicsEngine::createVertexShader(const void* shader_byte_code, size_t byte_code_size)
{
    VertexShader* vs = new VertexShader();

    if (!vs->init(shader_byte_code, byte_code_size)) 
    {
        vs->release();
        return nullptr;
    }
    return vs;
}

bool GraphicsEngine::compileVertexShader(const wchar_t* file_name, const char* entry_point_name, void** shader_byte_code, size_t* byte_code_size)
{
    ID3DBlob* error_blob = nullptr;

    if (!SUCCEEDED(::D3DCompileFromFile(file_name, nullptr, nullptr, entry_point_name, "vs_5_0", 0, 0, &m_blob, &error_blob)))
    {
        if (error_blob) error_blob->Release();
        return false;
    }


    *shader_byte_code = m_blob->GetBufferPointer();
    *byte_code_size = m_blob->GetBufferSize();
    return true;
}

void GraphicsEngine::releaseCompiledShader()
{
    if (m_blob)m_blob->Release();
}


GraphicsEngine* GraphicsEngine::get()
{
	static GraphicsEngine engine;
	return &engine;
}

bool GraphicsEngine::createShaders()
{
    ID3DBlob* vsBlob = nullptr;
    ID3DBlob* psBlob = nullptr;
    ID3DBlob* errorBlob = nullptr;

    // Vertex Shader
    HRESULT hr = D3DCompileFromFile(L"shader.hlsl", nullptr, nullptr, "vsmain", "vs_5_0", 0, 0, &vsBlob, &errorBlob);
    if (FAILED(hr)) {
        if (errorBlob) {
            OutputDebugStringA((char*)errorBlob->GetBufferPointer());
            errorBlob->Release();
        }
        return false;
    }
    m_device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &m_vs);

    // Pixel Shader
    hr = D3DCompileFromFile(L"shader.hlsl", nullptr, nullptr, "psmain", "ps_5_0", 0, 0, &psBlob, &errorBlob);
    if (FAILED(hr)) {
        if (errorBlob) {
            OutputDebugStringA((char*)errorBlob->GetBufferPointer());
            errorBlob->Release();
        }
        return false;
    }
    m_device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &m_ps);

    // VS blob’u Input Layout için sakla
    m_vsblob = vsBlob;
    m_psblob = psBlob;

    return true;
}

bool GraphicsEngine::setShaders()
{
    m_device_context->PSSetShader(m_ps);
    return true;
}



