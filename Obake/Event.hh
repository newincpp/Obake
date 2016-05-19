#pragma once
#include <map>
#include <iostream>
#include <functional>
#include <string>

namespace Obake {

	template<class CALLBACK_TARGET_CLASS, typename RETURN_TYPE>
	std::function<RETURN_TYPE()> bindFunction(RETURN_TYPE(CALLBACK_TARGET_CLASS::*memberFunction_)(), CALLBACK_TARGET_CLASS* callbackTarget_)
	{
		return std::function<RETURN_TYPE()>(std::bind(memberFunction_, callbackTarget_));
	}

	template<class CALLBACK_TARGET_CLASS, typename RETURN_TYPE, typename P0>
	std::function<RETURN_TYPE(P0)> bindFunction(RETURN_TYPE(CALLBACK_TARGET_CLASS::*memberFunction_)(P0), CALLBACK_TARGET_CLASS* callbackTarget_)
	{
		return std::function<RETURN_TYPE(P0)>(std::bind(memberFunction_, callbackTarget_, std::placeholders::_1));
	}

	template<class CALLBACK_TARGET_CLASS, typename RETURN_TYPE, typename P0, typename P1>
	std::function<RETURN_TYPE(P0, P1)> bindFunction(RETURN_TYPE(CALLBACK_TARGET_CLASS::*memberFunction_)(P0, P1), CALLBACK_TARGET_CLASS* callbackTarget_)
	{
		return std::function<RETURN_TYPE(P0, P1)>(std::bind(memberFunction_, callbackTarget_, std::placeholders::_1, std::placeholders::_2));
	}

	template<class CALLBACK_TARGET_CLASS, typename RETURN_TYPE, typename P0, typename P1, typename P2>
	std::function<RETURN_TYPE(P0, P1, P2)> bindFunction(RETURN_TYPE(CALLBACK_TARGET_CLASS::*memberFunction_)(P0, P1, P2), CALLBACK_TARGET_CLASS* callbackTarget_)
	{
		return std::function<RETURN_TYPE(P0, P1, P2)>(std::bind(memberFunction_, callbackTarget_, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	}

	template<class CALLBACK_TARGET_CLASS, typename RETURN_TYPE, typename P0, typename P1, typename P2, typename P3>
	std::function<RETURN_TYPE(P0, P1, P2, P3)> bindFunction(RETURN_TYPE(CALLBACK_TARGET_CLASS::*memberFunction_)(P0, P1, P2, P3), CALLBACK_TARGET_CLASS* callbackTarget_)
	{
		return std::function<RETURN_TYPE(P0, P1, P2, P3)>(std::bind(memberFunction_, callbackTarget_, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
			std::placeholders::_4));
	}

	template<class CALLBACK_TARGET_CLASS, typename RETURN_TYPE, typename P0, typename P1, typename P2, typename P3, typename P4>
	std::function<RETURN_TYPE(P0, P1, P2, P3, P4)> bindFunction(RETURN_TYPE(CALLBACK_TARGET_CLASS::*memberFunction_)(P0, P1, P2, P3, P4), CALLBACK_TARGET_CLASS* callbackTarget_)
	{
		return std::function<RETURN_TYPE(P0, P1, P2, P3, P4)>(std::bind(memberFunction_, callbackTarget_, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
			std::placeholders::_4, std::placeholders::_5));
	}

	class EventBase 
	{
	public:
		virtual ~EventBase() {};
		explicit EventBase() { };
	};

	template<typename RETURN_TYPE, typename... ARGS>
	class Event : public EventBase
	{
	private:
		std::map<size_t, std::function<RETURN_TYPE(ARGS...)>> _receivers;

		template<class CALLBACK_TARGET_CLASS>
		size_t _generateHash(CALLBACK_TARGET_CLASS* callbackTarget_, RETURN_TYPE(CALLBACK_TARGET_CLASS::*memberFunction_)(ARGS...))
		{
			char buff1[20];
			char buff2[20];
			sprintf_s(buff1, "%p", memberFunction_);
			sprintf_s(buff2, "%p", callbackTarget_);

			size_t h1 = std::hash<std::string>()(buff1);
			size_t h2 = std::hash<std::string>()(buff2);
			return h1 ^ (h2 << 1);
		}

		bool _isReceiver(size_t hash)
		{
			return _receivers.find(hash) != _receivers.end();
		}

	public:
		~Event() {};
		explicit Event() { };

		template<class CALLBACK_TARGET_CLASS>
		void bind(CALLBACK_TARGET_CLASS* callbackTarget_, RETURN_TYPE(CALLBACK_TARGET_CLASS::*memberFunction_)(ARGS...))
		{
			size_t generatedHash = _generateHash(callbackTarget_, memberFunction_);
				if (!_isReceiver(generatedHash))
					_receivers[generatedHash] = bindFunction<CALLBACK_TARGET_CLASS, RETURN_TYPE, ARGS...>(memberFunction_, callbackTarget_);
		}

		template<class CALLBACK_TARGET_CLASS>
		void unbind(CALLBACK_TARGET_CLASS* callbackTarget_, RETURN_TYPE(CALLBACK_TARGET_CLASS::*memberFunction_)(ARGS...))
		{
			_receivers.erase(_receivers.find(_generateHash(callbackTarget_, memberFunction_)));
		}

		void operator()(ARGS... params)
		{
			for (decltype(_receivers)::value_type receiver : _receivers)
			{
				receiver.second(params...);
			}
		}
	};
}