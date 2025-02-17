#pragma once

#include "crystal/scene/Scene.h"

namespace Crystal
{
	class SceneSerializer
	{
	public:
		SceneSerializer(const Reference<Scene>& scene);

		void Serialize(const string& filepath);
		void SerializeRuntime(const string& filepath);

		bool Deserialize(const string& filepath);
		bool DeserializeRuntime(const string& filepath);
		
	private:
		Reference<Scene> scene;
	};
	
}
