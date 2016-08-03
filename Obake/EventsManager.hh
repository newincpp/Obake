#pragma once
#include "Event.hh"

#include "DEBUG.hh"

namespace Obake
{
	class EventsManager
	{
	private:
		std::map<std::string, Obake::EventBase*> _events;

	public:
		~EventsManager();
		explicit EventsManager();

		template<typename RETURN_TYPE, class CALLBACK_TARGET_CLASS, typename... ARGS>
		void bindEvent(std::string eventName_, CALLBACK_TARGET_CLASS* callbackTarget_, RETURN_TYPE(CALLBACK_TARGET_CLASS::*memberFunction_)(ARGS...))
		{
			if (DEBUG_EVENT_SYSTEM == true)
			{
				std::cout << "[BIND] " << eventName_ << '\n';
			}

			if (_events.find(eventName_) == _events.end())
			{
				_events[eventName_] = new Event<RETURN_TYPE, ARGS...>();
			}
			Event<RETURN_TYPE, ARGS...>* event = static_cast<Event<RETURN_TYPE, ARGS...>*>(_events[eventName_]);
			event->bind(callbackTarget_, memberFunction_);
		}

		template<typename RETURN_TYPE, typename... ARGS>
		void executeEvent(std::string eventName_, ARGS... params)
		{
			if (DEBUG_EVENT_SYSTEM == true)
			{
				std::cout << "[EXEC] " << eventName_ << '\n';
			}

			if (_events.find(eventName_) != _events.end())
			{
	
				Event<void, ARGS...>* event = static_cast<Event<RETURN_TYPE, ARGS...>*>(_events[eventName_]);
				if (event != nullptr)
				{
					(*event)(params...);
				}
			}
		}

		template<typename RETURN_TYPE, typename... ARGS>
		Event<RETURN_TYPE, ARGS...>* getEvent(std::string eventName_)
		{
			return static_cast<Event<RETURN_TYPE, ARGS...>*>(_events[eventName_]);
		}
		
		void reset();
	};
}