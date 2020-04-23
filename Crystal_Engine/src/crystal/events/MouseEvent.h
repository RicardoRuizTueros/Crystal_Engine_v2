#pragma once

#include "crystal/events/Event.h"
#include "crystal/core/Input.h"

using namespace std;

namespace Crystal
{

	class MouseMovedEvent : public Event
	{
	public:
		MouseMovedEvent(float x, float y)
		{
			this->x = x;
			this->y = y;
		}

		float GetX() const { return x; }
		float GetY() const { return y; }

		string ToString() const override
		{
			stringstream stream;
			stream << "MouseMovedEvent: " << x << ", " << y;

			return stream.str();
		}

		EVENT_CLASS_TYPE(MouseMoved)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
	private:
		float x, y;
	};

	class MouseScrolledEvent : public Event
	{
	public:
		MouseScrolledEvent(float xOffset, float yOffset)
		{
			this->xOffset = xOffset;
			this->yOffset = yOffset;
		}

		float GetXOffset() const { return xOffset; }
		float GetYOffset() const { return yOffset; }

		string ToString() const override
		{
			stringstream stream;
			stream << "MouseScrolledEvent: " << xOffset << ", " << yOffset;

			return stream.str();
		}

		EVENT_CLASS_TYPE(MouseScrolled)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
	private:
		float xOffset, yOffset;
	};

	class MouseButtonEvent : public Event
	{
	public:
		inline MouseCode GetMouseButton() const { return button; }

		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
	protected:
		MouseCode button;

		MouseButtonEvent(MouseCode button)
		{
			this->button = button;
		}
	};

	class MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonPressedEvent(MouseCode button) : MouseButtonEvent(button)
		{
			this->button = button;
		}

		string ToString() const override
		{
			stringstream stream;
			stream << "MouseButtonPressedEvent: " << button;

			return stream.str();
		}

		EVENT_CLASS_TYPE(MouseButtonPressed)
	};

	class MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonReleasedEvent(MouseCode button) : MouseButtonEvent(button)
		{
			this->button = button;
		}

		string ToString() const override
		{
			stringstream stream;
			stream << "MouseButtonReleasedEvent: " << button;

			return stream.str();
		}

		EVENT_CLASS_TYPE(MouseButtonReleased)
	};
}