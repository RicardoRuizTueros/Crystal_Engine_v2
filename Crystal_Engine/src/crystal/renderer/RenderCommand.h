#pragma once

#include "crystal/renderer/RendererAPI.h"

using namespace glm;
using namespace std;

namespace Crystal
{
	class RenderCommand
	{

	public:
		static void Init()
		{
			rendererAPI->Init();
		}

		static void SetClearColor(const vec4& color)
		{
			rendererAPI->SetClearColor(color);
		}

		static void Clear()
		{
			rendererAPI->Clear();
		}

		static void DrawIndexed(const Reference<VertexArray>& vertexArray, uint32_t count = 0)
		{
			rendererAPI->DrawIndexed(vertexArray, count);
		}

		static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
		{
			rendererAPI->SetViewport(x, y, width, height);
		}

	private:
		static Scope<RendererAPI> rendererAPI;
	};
}