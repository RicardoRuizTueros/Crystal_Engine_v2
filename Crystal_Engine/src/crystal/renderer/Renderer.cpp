#include "crystalpch.h"
#include "Renderer.h"

#include "platform/openGL/OpenGLShader.h"

using namespace glm;
using namespace std;

namespace Crystal
{
	Renderer::SceneData* Renderer::sceneData = new Renderer::SceneData;

	void Renderer::Init()
	{
		RenderCommand::Init();
	}

	void Renderer::BeginScene(OrthographicCamera& camera)
	{
		sceneData->viewProjectionMatrix = camera.GetViewProjectionMatrix();
	}

	void Renderer::EndScene()
	{

	}

	void Renderer::Submit(const Reference<Shader>& shader, const Reference<VertexArray>& vertexArray, const mat4& transform)
	{
		shader->Bind();
		dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_viewProjection", sceneData->viewProjectionMatrix);
		dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_transform", transform);

		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}
}