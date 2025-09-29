#include <Codi.h>

#include <glm/gtc/matrix_transform.hpp>

class ExampleLayer: public Codi::Layer {
public:
    ExampleLayer() : Layer("Example"), _camera(-1.6f, 1.6f, -0.9f, 0.9f), _cameraPosition({0.0f, 0.0f, 0.0f}), _rotation(0.0f), _squarePosition(0.0f) {
        _vertexArray.reset(Codi::VertexArray::Create());
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

        _squareVA.reset(Codi::VertexArray::Create());
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

        std::string vertexSrc = R"(
            #version 330 core

            layout(location = 0) in vec3 a_Position;
            layout(location = 1) in vec4 a_Color;

            uniform mat4 u_ViewProjection;
            uniform mat4 u_Transform;

            out vec3 v_Position;
            out vec4 v_Color;

            void main() {
                v_Position = a_Position;
                v_Color = a_Color;
                gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
            }
        )";

        std::string fragmentSrc = R"(
            #version 330 core

            layout(location = 0) out vec4 color;

            in vec3 v_Position;
            in vec4 v_Color;

            void main() {
                color = vec4(v_Position * 0.5 + 0.5, 1.0);
                color = v_Color;
            }
        )";

        _shader.reset(new Codi::Shader(vertexSrc, fragmentSrc));

        std::string blueVertexSrc = R"(
            #version 330 core

            layout(location = 0) in vec3 a_Position;

            uniform mat4 u_ViewProjection;
            uniform mat4 u_Transform;

            out vec3 v_Position;

            void main() {
                v_Position = a_Position;
                gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
            }
        )";

        std::string blueFragmentSrc = R"(
            #version 330 core

            layout(location = 0) out vec4 color;

            void main() {
                color = vec4(0.2, 0.3, 0.8, 1.0);
            }
        )";

        _blueShader.reset(new Codi::Shader(blueVertexSrc, blueFragmentSrc));

        CTRACE("ExampleLayer created");
    }

    void onUpdate(Codi::DeltaTime deltatime) override {
        CTRACE("DeltaTime: {0}s ({1} ms)", deltatime.getSeconds(), deltatime.getMilliseconds());

        if (Codi::Input::IsKeyPressed(CODI_KEY_ESCAPE))
            Codi::Application::Get().close();
        
        // CAMERA MOVEMENT
        if (Codi::Input::IsKeyPressed(CODI_KEY_LEFT))
            _cameraPosition.x += _cameraSpeed * deltatime;
        if (Codi::Input::IsKeyPressed(CODI_KEY_RIGHT))
            _cameraPosition.x -= _cameraSpeed * deltatime;  
        if (Codi::Input::IsKeyPressed(CODI_KEY_UP))
            _cameraPosition.y -= _cameraSpeed * deltatime;   
        if (Codi::Input::IsKeyPressed(CODI_KEY_DOWN))
            _cameraPosition.y += _cameraSpeed * deltatime;

        if (Codi::Input::IsKeyPressed(CODI_KEY_Q))
            _rotation -= _rotationSpeed * deltatime;
        if (Codi::Input::IsKeyPressed(CODI_KEY_E))
            _rotation += _rotationSpeed * deltatime;

        // SQUARE MOVEMENT
        if (Codi::Input::IsKeyPressed(CODI_KEY_A))
            _squarePosition.x -= _squareSpeed * deltatime;   
        if (Codi::Input::IsKeyPressed(CODI_KEY_D))
            _squarePosition.x += _squareSpeed * deltatime; 
        if (Codi::Input::IsKeyPressed(CODI_KEY_W))
            _squarePosition.y += _squareSpeed * deltatime; 
        if (Codi::Input::IsKeyPressed(CODI_KEY_S))
            _squarePosition.y -= _squareSpeed * deltatime;

        Codi::RenderCommand::SetClearColor({ 0.0863f, 0.0902f, 0.1137f, 1.0f });
        Codi::RenderCommand::Clear();

        _camera.setPosition(_cameraPosition);
        _camera.setRotation(_rotation);

        Codi::Renderer::BeginScene(_camera);

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), _squarePosition);
        
        Codi::Renderer::Submit(_blueShader, _squareVA, transform);
        Codi::Renderer::Submit(_shader, _vertexArray);

        Codi::Renderer::EndScene();
    }

private:
    std::shared_ptr<Codi::Shader> _shader;
    std::shared_ptr<Codi::VertexArray> _vertexArray;

    std::shared_ptr<Codi::Shader> _blueShader;
    std::shared_ptr<Codi::VertexArray> _squareVA;

    Codi::OrthographicCamera _camera;
    glm::vec3 _cameraPosition;
    float _cameraSpeed = 1.0f;
    float _rotation = 0.0f;
    float _rotationSpeed = 90.0f;

    glm::vec3 _squarePosition;
    float _squareSpeed = 1.0f;
};

class Sandbox: public Codi::Application {
public:
    Sandbox() {
        pushLayer(new ExampleLayer());
    }
    ~Sandbox() {}
};

Codi::Application* Codi::CreateApplication() {
    return new Sandbox();
}
