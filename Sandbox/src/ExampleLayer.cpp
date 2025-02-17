#include "ExampleLayer.h"

#include <imgui/imgui.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

ExampleLayer::ExampleLayer()
	: Layer("Example"), cameraController(1920.0f / 1080.0f, true)
{
	// Data & Layout
	float logoTextureVertices[4 * 5] = {
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
		-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
	};

	float checkerTextureVertices[4 * 5] = {
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
		-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
	};

	float squareVertices[4 * 3] = {
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.5f,  0.5f, 0.0f,
		-0.5f,  0.5f, 0.0f
	};

	BufferLayout logoTextureLayout = {
		{ ShaderDataType::Float3, "a_position" },
		{ ShaderDataType::Float2, "a_textureCoordinates" }
	};

	BufferLayout checkerTextureLayout = {
		{ ShaderDataType::Float3, "a_position" },
		{ ShaderDataType::Float2, "a_textureCoordinates" }
	};

	BufferLayout squareLayout = {
		{ ShaderDataType::Float3, "a_position" },
	};

	// Buffers & arrays
	logoTextureVertexArray = VertexArray::Create();
	checkerTextureVertexArray = VertexArray::Create();

	Reference<VertexBuffer> logoTextureVertexBuffer = VertexBuffer::Create(logoTextureVertices, sizeof(logoTextureVertices));
	logoTextureVertexBuffer->SetLayout(logoTextureLayout);
	logoTextureVertexArray->AddVertexBuffer(logoTextureVertexBuffer);

	Reference<VertexBuffer> checkerTextureVertexBuffer = VertexBuffer::Create(checkerTextureVertices, sizeof(checkerTextureVertices));
	checkerTextureVertexBuffer->SetLayout(checkerTextureLayout);
	checkerTextureVertexArray->AddVertexBuffer(checkerTextureVertexBuffer);

	uint32_t logoTextureIndices[6] = { 0, 1, 2, 2, 3, 0 };
	Reference<IndexBuffer> logoTextureIndexBuffer = IndexBuffer::Create(logoTextureIndices, sizeof(logoTextureIndices) / sizeof(uint32_t));
	logoTextureVertexArray->SetIndexBuffer(logoTextureIndexBuffer);

	uint32_t checkerTextureIndices[6] = { 0, 1, 2, 2, 3, 0 };
	Reference<IndexBuffer> checkerTextureIndexBuffer = IndexBuffer::Create(checkerTextureIndices, sizeof(checkerTextureIndices) / sizeof(uint32_t));
	checkerTextureVertexArray->SetIndexBuffer(checkerTextureIndexBuffer);

	squareVertexArray = VertexArray::Create();

	Reference<VertexBuffer> squareVertexBuffer = VertexBuffer::Create(squareVertices, sizeof(squareVertices));
	squareVertexBuffer->SetLayout(squareLayout);
	squareVertexArray->AddVertexBuffer(squareVertexBuffer);

	uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
	Reference<IndexBuffer> squareIndexBuffer = IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t));
	squareVertexArray->SetIndexBuffer(squareIndexBuffer);

	// Shaders
	string checkerTextureVertexSource = R"(
			#version 330 core

			layout(location = 0) in vec3 a_position;
			layout(location = 1) in vec2 a_textureCoordinates;
			
			out vec2 v_textureCoordinates;

			uniform mat4 u_viewProjection;
			uniform mat4 u_transform;

			void main()
			{
				v_textureCoordinates = a_textureCoordinates;
				gl_Position = u_viewProjection * u_transform * vec4(a_position, 1.0);
			}
		)";

	string checkerTextureFragmentSource = R"(
			#version 330 core

			layout(location = 0) out vec4 color;
			
			in vec2 v_textureCoordinates;

			uniform sampler2D u_texture;

			void main()
			{
				color = texture(u_texture, v_textureCoordinates);
			}
		)";

	logoTextureShader = Shader::Create("assets/shaders/Texture.glsl");
	logoTexture = Texture2D::Create("assets/textures/logo.png");

	logoTextureShader->Bind();
	logoTextureShader->SetInt("u_texture", 0);

	checkerTextureShader = Shader::Create("CheckerTexture", checkerTextureVertexSource, checkerTextureFragmentSource);
	checkerTexture = Texture2D::Create("assets/textures/checkerboard.png");

	checkerTextureShader->Bind();
	checkerTextureShader->SetInt("u_texture", 0);

	string squareVertexSource = R"(
			#version 330 core

			layout(location = 0) in vec3 a_position;
			
			out vec3 v_position;

			uniform mat4 u_viewProjection;
			uniform mat4 u_transform;

			void main()
			{
				v_position = a_position;
				gl_Position = u_viewProjection * u_transform * vec4(a_position, 1.0);
			}
		)";

	string squareFragmentSource = R"(
			#version 330 core

			layout(location = 0) out vec4 color;
			
			in vec3 v_position;
			
			uniform vec3 u_color;

			void main()
			{
				color = vec4(u_color, 1.0);
			}
		)";

	squareShader = Shader::Create("SquaresColorShader", squareVertexSource, squareFragmentSource);
}

void ExampleLayer::OnAttach()
{
}

void ExampleLayer::OnDetach()
{
}

void ExampleLayer::OnUpdate(Timestep timestep)
{
	cameraController.OnUpdate(timestep);

	RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
	RenderCommand::Clear();

	Renderer::BeginScene(cameraController.GetCamera());

	mat4 scale = glm::scale(mat4(1.0f), vec3(0.1f));

	squareShader->Bind();
	squareShader->SetFloat3("u_color", squareShaderColor);

	for (int y = 0; y < 20; y++)
	{
		for (int x = 0; x < 20; x++)
		{
			vec3 position(x * 0.11f, y * 0.11f, 0.0f);
			mat4 transform = translate(mat4(1.0f), position) * scale;

			Renderer::Submit(squareShader, squareVertexArray, transform);
		}
	}

	checkerTexture->Bind();
	Renderer::Submit(checkerTextureShader, checkerTextureVertexArray, glm::scale(mat4(1.0f), vec3(0.5f)));

	logoTexture->Bind();
	Renderer::Submit(logoTextureShader, logoTextureVertexArray, glm::scale(mat4(1.0f), vec3(1.5f)));

	Renderer::EndScene();
}

void ExampleLayer::OnEvent(Event& event)
{
	cameraController.OnEvent(event);
}

void ExampleLayer::OnImGuiRender()
{
	ImGui::Begin("Settings");
	ImGui::ColorEdit3("Squares color", value_ptr(squareShaderColor));
	ImGui::End();
}
