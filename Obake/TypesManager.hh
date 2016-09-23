#pragma once

#include <vector>
#include <map>
#include <typeinfo>
#include <typeindex>
#include <iostream>

// Type Authorization
template <typename T>
struct IsAuthorized {
	static const bool value = false;
};

#define AUTH_TYPE(name, type) template <> struct IsAuthorized<type> { static const bool value = true;}; \
	 static std::vector<type> typeVec##name; \

#define GET_TYPE_VEC(name) (typeVec##name)

namespace Obake
{
	namespace TypeFramework 
	{
#define CREATE_TYPE(name, type) typeVec##name

		class TypesManager
		{
		private:
			//std::map<std::type_index, std::vector<std::any>> _typesMap;

		public:
			~TypesManager() {};
			explicit TypesManager() {};

			template <class Type, class... Args>
			inline Type& createType(Args&&... _args)
			{
			static_assert(IsAuthorized<Type>::E_Auth == false, "Type not supported.");

				//_typesMap[std::type_index(typeid(Type))].emplace_back(Args...);
			}
		};
	}
}