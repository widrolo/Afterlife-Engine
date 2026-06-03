#pragma once

#include <string>
#include <Engine/WTL/vector.h>
#include <Engine/Math/Transform.h>
#include <Engine/Types/CommonTypes.h>

#include <Engine/Components/Component.h>

namespace WEditor
{
	class SectorList;
	class EntityList;
	class ComponentList;
}

namespace WEngine
{
	class InputComponent;
	class Sector;

	struct CoreSystems;
	struct SpawnArgs;

	class Entity
	{
		friend Sector;
		friend WEditor::SectorList;
		friend WEditor::EntityList;
		friend WEditor::ComponentList;
	public:
		Entity() = default;
		virtual ~Entity() = default;

	public:
		Transform transform;
		std::string entityName;
		InputComponent* input = nullptr;
		Sector* parentSector = nullptr;

	private:
		wtl::vector<Component*> m_components;
		bool m_hasEverStarted : 1 = false;
		bool m_isStationary : 1 = true;

	public:

		void EntityDestroy();

		/**
		 * Called right after creating an entity.
		 * @param args arguments containing
		 */
		virtual void Awake(const SpawnArgs& args);

		/**
		 * Called right after Awake. Used if a component needs to initialize after all other components.
		 */
		virtual void LateAwake();

		virtual void Start();
		virtual void Stop();

		template<class T>
		T* GetComponent()
		{
			static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");
			const uint64 hash = T::StaticGetHash();

			for (auto c : m_components)
			{
				if (c->GetHash() == hash)
					return static_cast<T*>(c);
			}

			return nullptr;
		}
		void CreateComponent(const ComponentArgs& ca);
		virtual void Tick(float32 dt);
		virtual void PhysicsTick(float32 tr);
		virtual void Draw();
		virtual void DrawDebug();

		bool IsStationary() const { return m_isStationary; }
	private:
		void EntityStart(const SpawnArgs& args);
		void Internal_ParentSector(Sector* parentSector);
		void InitBaseComponents();
	};
}
