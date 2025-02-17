#include "crystalpch.h"

#include "platform/windows/WindowsWindow.h"
#include "platform/openGL/OpenGLContext.h"

#include "crystal/core/Input.h"
#include "crystal/events/ApplicationEvent.h"
#include "crystal/events/MouseEvent.h"
#include "crystal/events/KeyEvent.h"
#include "crystal/renderer/Renderer.h"

namespace Crystal 
{
	static uint8_t GLFWWindowCount = 0;

	static void GLFWErrorCallback(int error, const char* description)
	{
		CRYSTAL_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}

	WindowsWindow::WindowsWindow(const WindowProperties& properties)
	{
		CRYSTAL_PROFILE_FUNCTION();

		Init(properties);
	}

	WindowsWindow::~WindowsWindow()
	{
		CRYSTAL_PROFILE_FUNCTION();

		Shutdown();
	}

	void WindowsWindow::Init(const WindowProperties& properties)
	{
		CRYSTAL_PROFILE_FUNCTION();

		data.title = properties.title;
		data.width = properties.width;
		data.height = properties.height;

		CRYSTAL_CORE_INFO("Creating window {0} ({1}, {2})", properties.title, properties.width, properties.height);

		if (GLFWWindowCount == 0)
		{
			CRYSTAL_PROFILE_SCOPE("glfwInit");
			int success = glfwInit();
			CRYSTAL_CORE_ASSERT(success, "Could not intialize GLFW!");
			glfwSetErrorCallback(GLFWErrorCallback);
		}
		
		{
			CRYSTAL_PROFILE_SCOPE("glfwCreateWindow");

#ifdef CRYSTAL_DEBUG
			if (Renderer::GetAPI() == RendererAPI::API::OpenGL)
				glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

			window = glfwCreateWindow((int)properties.width, (int)properties.height, properties.title.c_str(), nullptr, nullptr);
			GLFWWindowCount++;
		}

		context = GraphicsContext::Create(window);
		context->Init();

		glfwSetWindowUserPointer(window, &data);
		SetVSync(true);

		// Set GLFW callbacks
		glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int width, int height)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.width = width;
			data.height = height;

			WindowResizeEvent event(width, height);
			data.callback(event);
		});

		glfwSetWindowCloseCallback(window, [](GLFWwindow* window)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			WindowCloseEvent event;
			data.callback(event);
		});

		glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
			case GLFW_PRESS:
			{
				KeyPressedEvent event(key, 0);
				data.callback(event);
				break;
			}
			case GLFW_RELEASE:
			{
				KeyReleasedEvent event(key);
				data.callback(event);
				break;
			}
			case GLFW_REPEAT:
			{
				KeyPressedEvent event(key, 1);
				data.callback(event);
				break;
			}
			}
		});

		glfwSetCharCallback(window, [](GLFWwindow* window, unsigned int keycode)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			KeyTypedEvent event(keycode);
		
			data.callback(event);
		});

		glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
			case GLFW_PRESS:
			{
				MouseButtonPressedEvent event(button);
				data.callback(event);
				break;
			}
			case GLFW_RELEASE:
			{
				MouseButtonReleasedEvent event(button);
				data.callback(event);
				break;
			}
			}
		});

		glfwSetScrollCallback(window, [](GLFWwindow* window, double xOffset, double yOffset)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseScrolledEvent event((float)xOffset, (float)yOffset);
			data.callback(event);
		});

		glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xPos, double yPos)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseMovedEvent event((float)xPos, (float)yPos);
			data.callback(event);
		});
	}

	void WindowsWindow::Shutdown()
	{
		CRYSTAL_PROFILE_FUNCTION();

		glfwDestroyWindow(window);
		GLFWWindowCount--;

		if (GLFWWindowCount == 0)
		{
			glfwTerminate();
		}
	}

	void WindowsWindow::OnUpdate()
	{
		CRYSTAL_PROFILE_FUNCTION();

		glfwPollEvents();
		context->SwapBuffers();
	}

	void WindowsWindow::SetVSync(bool enabled)
	{
		CRYSTAL_PROFILE_FUNCTION();

		if (enabled) 
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		data.vSync = enabled;
	}

	bool WindowsWindow::IsVSync() const
	{
		return data.vSync;
	}
}
