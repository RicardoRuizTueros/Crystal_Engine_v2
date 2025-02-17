#include "crystalpch.h"
#include "SceneSerializer.h"
#include "crystal/scene/Entity.h"
#include "crystal/scene/Components.h"

#include <fstream>
#include <yaml-cpp/yaml.h>

using namespace YAML;

namespace YAML
{
	template<>
	struct convert<vec3>
	{
		static Node encode(const vec3& vector)
		{
			Node node;

			node.push_back(vector.x);
			node.push_back(vector.y);
			node.push_back(vector.z);
			node.SetStyle(EmitterStyle::Flow);

			return node;
		}

		static bool decode(const Node& node, vec3& vector)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			vector.x = node[0].as<float>();
			vector.y = node[1].as<float>();
			vector.z = node[2].as<float>();

			return true;
		}
	};

	template<>
	struct convert<vec4>
	{
		static Node encode(const vec4& vector)
		{
			Node node;

			node.push_back(vector.x);
			node.push_back(vector.y);
			node.push_back(vector.z);
			node.push_back(vector.w);
			node.SetStyle(EmitterStyle::Flow);
	
			return node;
		}

		static bool decode(const Node& node, vec4& vector)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			vector.x = node[0].as<float>();
			vector.y = node[1].as<float>();
			vector.z = node[2].as<float>();
			vector.w = node[3].as<float>();

			return true;
		}
	};
}

namespace Crystal
{
	Emitter& operator << (Emitter& out, const vec3 vector)
	{
		out << Flow;
		out << BeginSeq << vector.x << vector.y << vector.z << EndSeq;

		return out;
	}

	Emitter& operator << (Emitter& out, const vec4 vector)
	{
		out << Flow;
		out << BeginSeq << vector.x << vector.y << vector.z << vector.w << EndSeq;

		return out;
	}

	SceneSerializer::SceneSerializer(const Reference<Scene>& scene)
	{
		this->scene = scene;
	}

	void SceneSerializer::Serialize(const string& filepath)
	{
		Emitter out;
		out << BeginMap;
		out << Key << "Scene" << Value << "Untitled scene";
		out << Key << "Entities" << Value << BeginSeq;

		scene->registry.each([&](auto entityID)
			{
				Entity entity = { entityID, scene.get() };

				if (!entity)
					return;

				SerializeEntity(out, entity);
			});

		out << EndSeq;
		out << EndMap;

		ofstream fout(filepath);
		fout << out.c_str();
	}

	void SceneSerializer::SerializeRuntime(const string& filepath)
	{
		CRYSTAL_CORE_ASSERT(false, "Not implemented");
	}

	bool SceneSerializer::Deserialize(const string& filepath)
	{
		Node data = LoadFile(filepath);

		if (!data["Scene"])
			return false;

		string sceneName = data["Scene"].as<string>();

		auto entities = data["Entities"];
		if (entities)
		{
			for (auto entity : entities)
			{
					uint64_t uuid = entity["Entity"].as<uint64_t>();

				string name;
				auto tagComponent = entity["TagComponent"];
				if (tagComponent)
					name = tagComponent["Tag"].as<string>();

				Entity deserializedEntity = scene->CreateEntity(name);

				auto transformComponent = entity["TransformComponent"];
				if (transformComponent)
				{
					auto& deserializedTransformComponent = deserializedEntity.GetComponent<TransformComponent>();
					deserializedTransformComponent.translation = transformComponent["Translation"].as<vec3>();
					deserializedTransformComponent.rotation = transformComponent["Rotation"].as<vec3>();
					deserializedTransformComponent.scale = transformComponent["Scale"].as<vec3>();
				}

				auto cameraComponent = entity["CameraComponent"];
				if (cameraComponent)
				{
					auto& deserializedCameraComponent = deserializedEntity.AddComponent<CameraComponent>();
					deserializedCameraComponent.fixedAspectRatio = cameraComponent["FixedAspectRatio"].as<bool>();
					deserializedCameraComponent.primary = cameraComponent["Primary"].as<bool>();

					auto& camera = cameraComponent["Camera"];
					auto& deserializedCamera = deserializedCameraComponent.camera;
					deserializedCamera.SetProjectionType((SceneCamera::ProjectionType)camera["ProjectionType"].as<int>());
					deserializedCamera.SetPerspeciveVerticalFieldOfView(camera["PerspectiveFOV"].as<float>());
					deserializedCamera.SetPerspectiveNearClip(camera["PerspectiveNear"].as<float>());
					deserializedCamera.SetPerspectiveFarClip(camera["PerspectiveFar"].as<float>());
					deserializedCamera.SetOrthographicSize(camera["OrthographicSize"].as<float>());
					deserializedCamera.SetOrthographicNearClip(camera["OrthographicNear"].as<float>());
					deserializedCamera.SetOrthographicFarClip(camera["OrthographicFar"].as<float>());
				}

				auto spriteRendererComponent = entity["SpriteRendererComponent"];
				if (spriteRendererComponent)
				{
					auto& deserializedSpriRendererComponent = deserializedEntity.AddComponent<SpriteRendererComponent>();
					deserializedSpriRendererComponent.color = spriteRendererComponent["Color"].as<vec4>();
				}
			}
		}

		return true;
	}

	bool SceneSerializer::DeserializeRuntime(const string& filepath)
	{
		CRYSTAL_CORE_ASSERT(false, "Not implemented");
		return false;
	}

	static void SerializeEntity(Emitter& out, Entity entity)
	{
		out << BeginMap;
		out << Key << "Entity" << Value << "7777"; // Todo: Replace with entity UUID

		if (entity.HasComponent<TagComponent>())
		{
			out << Key << "TagComponent";
			out << BeginMap;

			auto& tag = entity.GetComponent<TagComponent>().tag;
			out << Key << "Tag" << Value << tag;
			out << EndMap;
		}

		if (entity.HasComponent<TransformComponent>())
		{
			out << Key << "TransformComponent";
			out << BeginMap;

			auto& transformComponent = entity.GetComponent<TransformComponent>();
			out << Key << "Translation" << Value << transformComponent.translation;
			out << Key << "Rotation" << Value << transformComponent.rotation;
			out << Key << "Scale" << Value << transformComponent.scale;

			out << EndMap;
		}

		if (entity.HasComponent<CameraComponent>())
		{
			out << Key << "CameraComponent";
			out << BeginMap;

			auto& cameraComponent = entity.GetComponent<CameraComponent>();
			auto& camera = cameraComponent.camera;

			out << Key << "Primary" << Value << cameraComponent.primary;
			out << Key << "FixedAspectRatio" << Value << cameraComponent.fixedAspectRatio;

			out << Key << "Camera";
			out << BeginMap;
			out << Key << "ProjectionType" << Value << (int)camera.GetProjectionType();
			out << Key << "PerspectiveFOV" << Value << camera.GetPerspectiveVerticalFieldOfView();
			out << Key << "PerspectiveNear" << Value << camera.GetPerspectiveNearClip();
			out << Key << "PerspectiveFar" << Value << camera.GetPerspectiveFarClip();
			out << Key << "OrthographicSize" << Value << camera.GetOrthographicSize();
			out << Key << "OrthographicNear" << Value << camera.GetOrthographicNearClip();
			out << Key << "OrthographicFar" << Value << camera.GetOrthographicFarClip();
			out << EndMap;

			out << EndMap;
		}

		if (entity.HasComponent<SpriteRendererComponent>())
		{
			out << Key << "SpriteRendererComponent";
			out << BeginMap;

			auto& spriteRendererComponent = entity.GetComponent<SpriteRendererComponent>();
			out << Key << "Color" << Value << spriteRendererComponent.color;

			out << EndMap;
		}

		out << EndMap;
	}
}
