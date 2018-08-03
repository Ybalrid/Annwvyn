#pragma once

#include "systemMacro.h"
#include "AnnGameObject.hpp"

#include <memory>
#include <vector>
#include <variant>

namespace Annwvyn
{

	class AnnDllExport AnnProceduralGameObjectGenerator
	{

		mutable std::vector<std::variant<Ogre::MeshPtr, Ogre::v1::MeshPtr>> cachedMeshes;

		///Cached pointer to the manager
		AnnGameObjectManager* GameObjectManager;

		std::string makeDotMeshName(const std::string& string) const;

		void clearMeshCache(); //not const, even if vector is mutable, we want to strongly communicate that this changes the state of the object

		bool meshExists(const std::string& meshName) const;

		template <class Generator, class... GeneratorCtorArgs>
		AnnGameObjectPtr getGeneratedObject(const std::string& name, const std::string& identifier, GeneratorCtorArgs&&... ctorArgs) const
		{
			const auto meshName = makeDotMeshName(name);

			if(meshExists(meshName))
				return GameObjectManager->createGameObject(meshName, identifier);

			auto generator = Generator(ctorArgs...);

			auto mesh = generator.realizeMesh(meshName);
			assert(mesh != nullptr);
			cachedMeshes.push_back(mesh);

			//Note, we are relying on the fact that this API uses the **name** of the mesh to select what to create
			auto gameObject = GameObjectManager->createGameObject(meshName, identifier);
			assert(gameObject != nullptr);

			return gameObject;
		}

	public:
		AnnProceduralGameObjectGenerator(AnnGameObjectManager* ptr);
		AnnGameObjectPtr getBox(const std::string& name, const std::string& identifier = "", AnnVect3 size = AnnVect3::UNIT_SCALE) const;
		AnnGameObjectPtr getSphere(const std::string& name, const std::string& identifier = "", float radius = 1, float rings = 16, float segment = 16) const;
	};
}
