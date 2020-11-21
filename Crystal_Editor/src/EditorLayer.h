#pragma once

#include "Crystal.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Crystal/Scene/SceneSerializer.h"

#include "crystal/utils/PlatformUtils.h"

namespace Crystal
{
	class EditorLayer : public Layer
	{

	public:
		EditorLayer();
		virtual ~EditorLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		void OnUpdate(Timestep timestep) override;
		virtual void OnImGuiRender() override;
		void OnEvent(Event& event) override;

	private:
		bool OnKeyPressed(KeyPressedEvent& event);

		void NewScene();
		void OpenScene();
		void SaveSceneAs();


		OrthographicCameraController cameraController;
		vec2 viewportSize = { 0.0f, 0.0f };
		bool viewportFocused = false, viewportHovered = false;

		Reference<Scene> activeScene;
		Entity cameraA, cameraB;

		// To do: Remove this!
		Reference<VertexArray> squareVertexArray;
		Reference<Shader> flatColorShader;
		Reference<Texture2D> checkerTexture;
		Reference<FrameBuffer> frameBuffer;

		SceneHierarchyPanel sceneHierarchyPanel;
	};
}
