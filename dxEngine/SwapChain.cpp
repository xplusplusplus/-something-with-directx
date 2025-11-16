#include "SwapChain.h"
#include "GraphicsEngine.h"

SwapChain::SwapChain()
{
}

bool SwapChain::init(HWND hwnd, UINT width, UINT height)
{
	ID3D11Device* device = GraphicsEngine::get()->m_device;

	DXGI_SWAP_CHAIN_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.BufferDesc.Width = width;
	desc.BufferDesc.Height = height;
	desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.BufferCount = 1;
	desc.BufferDesc.RefreshRate.Numerator = 60;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.OutputWindow = hwnd;
	desc.SampleDesc.Quality = 0;
	desc.SampleDesc.Count = 1;
	desc.Windowed = TRUE;


	HRESULT hr= GraphicsEngine::get()->m_dxgi_factory->CreateSwapChain(device,&desc, &m_swapChain);

	if (FAILED(hr))
		return false;

	ID3D11Texture2D* buffer=NULL;
	hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&buffer);

	if (FAILED(hr))
		return false;

	hr=device->CreateRenderTargetView(buffer, NULL, &m_rtv);
	buffer->Release();

	if (FAILED(hr))
		return false;

	return true;
}

bool SwapChain::present(bool vsync)
{
	m_swapChain->Present(vsync, 0);
	
	return true;
}

bool SwapChain::release()
{
	m_swapChain->Release();
	delete this;
	return true;
}

SwapChain::~SwapChain()
{
}
