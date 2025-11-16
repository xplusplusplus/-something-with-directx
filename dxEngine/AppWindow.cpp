#include "AppWindow.h"
#include "GraphicsEngine.h"
#include <stdexcept>

struct vec3 {
    float x, y, z;
};

struct vec4
{
    float r, g, b, a;
};

struct Vertex
{
    vec3 pos;
};

AppWindow::~AppWindow()
{
    if (m_vs) m_vs->release();
    if (m_vb) m_vb->release();
    if (m_swap_chain) m_swap_chain->release();
}

void AppWindow::onCreate()
{
    try {
        Window::onCreate();
        
        // Graphics Engine başlatma
        if (!GraphicsEngine::get()->init())
        {
            throw std::runtime_error("Graphics Engine başlatılamadı");
        }

        // Swap Chain oluşturma
        m_swap_chain = GraphicsEngine::get()->createSwapChain();
        if (!m_swap_chain)
        {
            throw std::runtime_error("Swap Chain oluşturulamadı");
        }

        // Pencere boyutlarını al ve swap chain'i başlat
        RECT rc = this->getClientWindowRect();
        int width = rc.right - rc.left;
        int height = rc.bottom - rc.top;
        
        if (width <= 0 || height <= 0)
        {
            throw std::runtime_error("Geçersiz pencere boyutları");
        }

        if (!m_swap_chain->init(this->m_hwnd, width, height))
        {
            throw std::runtime_error("Swap Chain başlatılamadı");
        }

        // Vertex verileri
        Vertex list[] = {
            {-0.5f, -0.5f, 0.0f},
            {-0.5f,  0.5f, 0.0f},
            { 0.5f, -0.5f, 0.0f},
            { 0.5f,  0.5f, 0.0f}
        };

        // Vertex Buffer oluşturma
        m_vb = GraphicsEngine::get()->createVertexBuffer();
        if (!m_vb)
        {
            throw std::runtime_error("Vertex Buffer oluşturulamadı");
        }

        UINT size_list = ARRAYSIZE(list);
        void* shader_byte_code = nullptr;
        size_t size_shader = 0;

        // Shader'ları oluştur ve derle
        if (!GraphicsEngine::get()->createShaders())
        {
            throw std::runtime_error("Shader'lar oluşturulamadı");
        }

        if (!GraphicsEngine::get()->compileVertexShader(L"VertexShader.hlsl", "vsmain", &shader_byte_code, &size_shader))
        {
            throw std::runtime_error("Vertex Shader derlenemedi");
        }

        // Vertex Shader oluştur
        m_vs = GraphicsEngine::get()->createVertexShader(shader_byte_code, size_shader);
        if (!m_vs)
        {
            throw std::runtime_error("Vertex Shader oluşturulamadı");
        }

        // Vertex Buffer'a verileri yükle
        if (!m_vb->load(list, sizeof(Vertex), size_list, shader_byte_code, size_shader))
        {
            throw std::runtime_error("Vertex Buffer'a veri yüklenemedi");
        }

        GraphicsEngine::get()->releaseCompiledShader();
    }
    catch (const std::exception& e) {
        // Hata mesajını göster
        MessageBoxA(NULL, e.what(), "Hata", MB_OK | MB_ICONERROR);
        // Uygulamayı sonlandır
        PostQuitMessage(0);
    }
}

void AppWindow::onUpdate()
{
    try {
        if (!m_swap_chain || !m_vb || !m_vs)
        {
            throw std::runtime_error("Gerekli grafik kaynakları yüklenmemiş");
        }

        Window::onUpdate();
        
        // Ekranı temizle
        GraphicsEngine::get()->getDevieContext()->clearRenderTargetColor(m_swap_chain, 0, 0.3f, 0.4f, 1);

        // Görünüm alanını ayarla
        RECT rc = this->getClientWindowRect();
        int width = rc.right - rc.left;
        int height = rc.bottom - rc.top;
        
        if (width > 0 && height > 0)
        {
            GraphicsEngine::get()->getDevieContext()->setViewportSize(width, height);
        }

        // Shader'ları ayarla ve çizim yap
        GraphicsEngine::get()->setShaders();
        GraphicsEngine::get()->getDevieContext()->setVertexShader(m_vs);
        GraphicsEngine::get()->getDevieContext()->setVertexBuffer(m_vb);
        GraphicsEngine::get()->getDevieContext()->drawTriangleStrip(m_vb->getSizeVertexList(), 0);

        // Sonuçları ekrana yansıt
        m_swap_chain->present(true);
    }
    catch (const std::exception& e) {
        // Hata mesajını göster
        MessageBoxA(NULL, e.what(), "Çizim Hatası", MB_OK | MB_ICONERROR);
        // Uygulamayı sonlandır
        PostQuitMessage(0);
    }
}

void AppWindow::onDestroy()
{
    Window::onDestroy();
    
    // Kaynakları temizle
    if (m_vb) {
        m_vb->release();
        m_vb = nullptr;
    }
    if (m_swap_chain) {
        m_swap_chain->release();
        m_swap_chain = nullptr;
    }
    if (m_vs) {
        m_vs->release();
        m_vs = nullptr;
    }
    
    GraphicsEngine::get()->release();
}
