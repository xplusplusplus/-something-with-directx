#pragma once
#include <d3d11.h>

class SwapChain;
class DeviceContext;
class VertexBuffer;
class VertexShader;


class GraphicsEngine
{
public:
    GraphicsEngine();
    bool init();
    bool release();
    ~GraphicsEngine();

    SwapChain* createSwapChain();
    DeviceContext* getDevieContext();
    VertexBuffer* createVertexBuffer();
    VertexShader* createVertexShader(const void* shader_byte_code, size_t byte_code_size);

    bool compileVertexShader(const wchar_t* file_name, const char* entry_point_name, void** shader_byte_code, size_t* byte_code_size);
    void releaseCompiledShader();

    bool createShaders();
    bool setShaders();
    

    static GraphicsEngine* get();

private:
    DeviceContext* m_device_context;

    ID3D11Device* m_device;
    D3D_FEATURE_LEVEL m_feature_level;

    IDXGIDevice* m_dxgi_device;
    IDXGIAdapter* m_dxgi_adapter;
    IDXGIFactory* m_dxgi_factory;


    ID3DBlob* m_blob = nullptr;

    ID3DBlob* m_vsblob = nullptr;
    ID3DBlob* m_psblob = nullptr;
    ID3D11VertexShader* m_vs = nullptr;
    ID3D11PixelShader* m_ps = nullptr;

    friend class SwapChain;
    friend class VertexBuffer;
    friend class VertexShader;
    friend class DeviceContext;

};