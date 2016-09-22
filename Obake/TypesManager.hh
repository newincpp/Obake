#pragma once

#include <vector>
#include <map>
#include <typeinfo>
#include <typeindex>


namespace Obake
{
	namespace TypeFramework 
	{
		// Type Authorization
		template <typename T>
		struct IsAuthorized {
			static const bool value = false;
		};



		struct Nil {};

		template <typename T, typename U> struct Cons {};

		template <typename List, typename Reversed> struct ListReverseHelper;

		template <typename Reversed>
		struct ListReverseHelper<Nil, Reversed> {
			using Type = Reversed;
		};

		template <typename Head, typename Tail, typename Reversed>
		struct ListReverseHelper<Cons<Head, Tail>, Reversed> {
			using Type = typename ListReverseHelper<Tail, Cons<Head, Reversed>>::Type;
		};

		template <typename T, int N> struct ListMakerKey : ListMakerKey<T, N - 1> {};
		template <typename T> struct ListMakerKey<T, 0> {};

#define START_LIST_(name, modifier) \
  struct name##_ListMaker {}; \
  modifier Nil list_maker_helper_(ListMakerKey<name##_ListMaker, __COUNTER__>);
#define ADD_TO_LIST_(name, type, modifier) \
  modifier Cons<type, decltype(list_maker_helper_(ListMakerKey<name##_ListMaker, __COUNTER__>{}))> \
  list_maker_helper_(ListMakerKey<name##_ListMaker, __COUNTER__>);
#define END_LIST(name) \
  using name = typename ListReverseHelper<decltype(list_maker_helper_(ListMakerKey<name##_ListMaker, __COUNTER__>{})), Nil>::Type;

		static int typesValSize = 0

#define START_LIST(name) START_LIST_(name, static)
#define ADD_TO_LIST(name, type) ADD_TO_LIST_(name, type, static)
#define START_LIST_FUNC(name) START_LIST_(name,)
#define ADD_TO_LIST_FUNC(name, type) ADD_TO_LIST_(name, type,)

#define REGISTER_TYPE_(name, type, modifier) \
	struct name##_ListMaker {}; \
	++typesValSize \
	static std::vector<int, ListMakerKey<name##_ListMaker, __COUNTER__>> typeVec##_REGISTERED_TYPES_SIZE; \

#define REGISTER_TYPE(name, type) REGISTER_TYPE_(name, type, static)


#define AUTH_TYPE(name, type) template <> struct IsAuthorized<type> { static const bool value = true; START_LIST(name) ADD_TO_LIST(name, type) END_LIST(name) };

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