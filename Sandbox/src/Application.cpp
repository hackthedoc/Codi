#include <Codi.h>
#include <Codi/Core/EntryPoint.h>

#include <glm/gtc/matrix_transform.hpp>

#include "Sandbox2D.h"

class ExampleLayer: public Codi::Layer {
public:
    ExampleLayer() : Layer("Example"), _squarePosition(0.0f), _cameraController(1280.0f / 720.0f) {
        _vertexArray = Codi::VertexArray::Create();
        float vertices[3 * 7] = {
            -0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
             0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
             0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
        };

        std::shared_ptr<Codi::VertexBuffer> vertexBuffer;
        vertexBuffer.reset(Codi::VertexBuffer::Create(vertices, sizeof(vertices)));
        Codi::BufferLayout layout = {
            { Codi::ShaderDataType::Float3, "a_Position" },
            { Codi::ShaderDataType::Float4, "a_Color" }
        };
        vertexBuffer->setLayout(layout);
        _vertexArray->addVertexBuffer(vertexBuffer);

        uint32_t indices[3] = { 0, 1, 2 };
        std::shared_ptr<Codi::IndexBuffer> indexBuffer;
        indexBuffer.reset(Codi::IndexBuffer::Create(indices, 3));
        _vertexArray->setIndexBuffer(indexBuffer);

        _squareVA = Codi::VertexArray::Create();
        float squareVertices[3 * 4] = {
            -0.75f, -0.75f, 0.0f,
             0.75f, -0.75f, 0.0f,
             0.75f,  0.75f, 0.0f,
            -0.75f,  0.75f, 0.0f
        };

        std::shared_ptr<Codi::VertexBuffer> squareVB;
        squareVB.reset(Codi::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
        squareVB->setLayout({
            { Codi::ShaderDataType::Float3, "a_Position" }
        });
        _squareVA->addVertexBuffer(squareVB);

        uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
        std::shared_ptr<Codi::IndexBuffer> squareIB;
        squareIB.reset(Codi::IndexBuffer::Create(squareIndices, 6));
        _squareVA->setIndexBuffer(squareIB);

        _shader = Codi::Shader::Create("assets/shaders/TriangleShader.glsl");
        _blueShader = Codi::Shader::Create("assets/shaders/BlueShader.glsl");
    }

    void onUpdate(Codi::DeltaTime deltatime) override {
        //////// UPDATING //////////
        
        if (Codi::Input::IsKeyPressed(CODI_KEY_ESCAPE))
            Codi::Application::Get().close();
        
        _cameraController.onUpdate(deltatime);

        ////////// RENDERING //////////

        Codi::RenderCommand::SetClearColor({ 0.0863f, 0.0902f, 0.1137f, 1.0f });
        Codi::RenderCommand::Clear();

        Codi::Renderer::BeginScene(_cameraController.getCamera());

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), _squarePosition);
        
        Codi::Renderer::Submit(_blueShader, _squareVA, transform);
        Codi::Renderer::Submit(_shader, _vertexArray);

        Codi::Renderer::EndScene();
    }

    void onEvent(Codi::Event& e) override {
        _cameraController.onEvent(e);
    }

private:
    Codi::Ref<Codi::Shader> _shader;
    Codi::Ref<Codi::VertexArray> _vertexArray;

    Codi::Ref<Codi::Shader> _blueShader;
    Codi::Ref<Codi::VertexArray> _squareVA;

    Codi::OrthographicCameraController _cameraController;

    glm::vec3 _squarePosition;
    float _squareSpeed = 1.0f;
    glm::vec4 _squareColor = { 1.0f, 0.0f, 0.0f, 1.0f };
};

class Sandbox: public Codi::Application {
public:
    Sandbox() {
        //pushLayer(new ExampleLayer());
        pushLayer(new Sandbox2D());
    }
    ~Sandbox() {}
};

Codi::Application* Codi::CreateApplication() {
    return new Sandbox();
}
