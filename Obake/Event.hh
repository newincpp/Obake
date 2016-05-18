#pragma once
#include <map>
#include <iostream>
#include <functional>
#include <memory>

namespace Obake {

	class Event
	{
	private:
		typedef void(EventFunc)(void);
		std::map<size_t, std::function<EventFunc>> _receivers;

		class EventBinding
		{
		private:
			explicit EventBinding();
			std::hash<std::string> _hash;
			size_t _generatedHash;

		public:
			~EventBinding() {};
			template< class F, class Obj >
			explicit EventBinding(F f_, Obj obj_)
			{
				size_t h1 = std::hash<std::string>()(f_);
				size_t h2 = std::hash<std::string>()(obj_);

				_generatedHash = h1 ^ (h2 << 1);
			}

			size_t getHash()
			{
				return _generatedHash;
			}
		};

	public:
		~Event() {};
		explicit Event() { };

		/*	void operator+=(std::function<void(void)>&& f_)
			{*/
		template< class F, class Obj >
		void Bind(F f_, Obj obj_)
		{
			char test1[20];
			sprintf_s(test1, "%p", f_);
			char test2[20];
			sprintf_s(test2, "%p", obj_);
			printf("test1: %s | test2: %s | f_: %p | p_: %p\n", test1, test2, f_, obj_);
			size_t h1 = std::hash<std::string>()(test1);
			size_t h2 = std::hash<std::string>()(test2);
			size_t generatedHash = h1 ^ (h2 << 1);

			std::cout << "GeneratedHash: " << generatedHash << std::endl << std::endl;;

			if (_receivers.find(generatedHash) != _receivers.end())
			{
				std::cout << "Receiver: " << generatedHash << " found";
				_receivers[generatedHash] = std::bind(f_, obj_);
			}
		}

		template< class F, class Obj >
		void Unbind(F f_, Obj obj_)
		{
			char test1[20];
			char test2[20];
			sprintf_s(test1, "%p", f_);
			sprintf_s(test2, "%p", obj_);
			size_t h1 = std::hash<std::string>()(test1);
			size_t h2 = std::hash<std::string>()(test2);
			size_t generatedHash = h1 ^ (h2 << 1);

			_receivers.erase(_receivers.find(generatedHash));
		}

		void operator()()
		{
			for (decltype(_receivers)::value_type receiver : _receivers)
			{
				receiver.second();
			}
		}
	};

	class EventsManager
	{
	private:
		std::map<std::string, Obake::Event> _events;

	public:
		~EventsManager();
		explicit EventsManager();

		void ABAOUABAA();
		void DZAZEA();
	};
}