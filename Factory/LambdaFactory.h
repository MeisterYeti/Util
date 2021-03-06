/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	Copyright (C) 2015 Sascha Brandt <myeti@mail.upb.de>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef UTIL_FACTORY_LAMBDAFACTORY_H
#define UTIL_FACTORY_LAMBDAFACTORY_H

#include "FallbackPolicies.h"
#include <iostream>
#include <functional>
#include <stdexcept>
#include <typeinfo>
#include <string>
#include <map>
namespace Util {
	
//! @addtogroup factory
//! @{
	
template <typename Function>
struct function_traits
  : public function_traits<decltype(&Function::operator())>
{};

template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits<ReturnType(ClassType::*)(Args...) const> {
  typedef ReturnType (*pointer)(Args...);
  typedef std::function<ReturnType(Args...)> function;
};

template <typename Function>
typename function_traits<Function>::pointer
to_function_pointer (Function& lambda) {
  return static_cast<typename function_traits<Function>::pointer>(lambda);
}

template <typename Function>
typename function_traits<Function>::function
to_function (Function& lambda) {
  return static_cast<typename function_traits<Function>::function>(lambda);
}

/**
 * @brief Generic factory for objects allowing lambdas with variable number of arguments as creator functions.
 *
 * The factory template can be instaniated to generate different kinds of factories.
 * The generated factory generates objects of type @a ObjectType.
 *
 * @tparam ObjectType Base type for all objects that are generated by the factory
 * @tparam IdentifierType Type of the identifier that specifies which object creator to call
 * @tparam FallbackPolicy Template with a function @a onUnknownType() that handles the case that the requested object type was not found
 * @author Sascha Brandt
 * @date 2015-05-22
 * @see http://meh.schizofreni.co/programming/magic/2013/01/23/function-pointer-from-lambda.html
 */
template < class ObjectType,
		 typename IdentifierType,
		 template<class, typename> class FallbackPolicy = FallbackPolicies::ExceptionFallback >
class LambdaFactory {
	private:
		struct Creator final {
			void* function;
			const std::type_info* signature;
		};
		typedef std::map<IdentifierType, Creator> registrations_t;
		registrations_t registrations;
	public:
		typedef FallbackPolicy<ObjectType, IdentifierType> fallbackPolicy_t;
		fallbackPolicy_t fallbackPolicy;
		LambdaFactory() : fallbackPolicy() {
		}
		LambdaFactory(fallbackPolicy_t policy) : fallbackPolicy(policy) {
		}
		~LambdaFactory() {
			for(auto entry : registrations)
				delete static_cast<std::function<ObjectType()>*>(entry.second.function);
		}

		template<typename ObjectCreator>
		void registerType(const IdentifierType & id, ObjectCreator creator) {
			if(registrations.find(id) != registrations.end())
				throw std::invalid_argument("the creator already exists");

			auto function = new decltype(to_function(creator))(to_function(creator));

			registrations[id].function = static_cast<void*>(function);
			registrations[id].signature = &typeid(function);
		}

		void unregisterType(const IdentifierType & id) {
			if(registrations.find(id) == registrations.end())
				return;
			delete static_cast<std::function<ObjectType()>*>(registrations[id].function);
			registrations.erase(id);
		}

		template<typename ...Args>
		ObjectType create(const IdentifierType & id, Args... args) {
			if(registrations.find(id) == registrations.end())
				return fallbackPolicy.onUnknownType(std::bind(&LambdaFactory::create<>, this, std::placeholders::_1), id);

			auto creator = registrations.at(id);
			auto function = static_cast<std::function<ObjectType(Args...)>*>(creator.function);

			if(typeid(function) != *(creator.signature))
				throw std::bad_typeid();

			return (*function)(args...);
		}
};

//! @}

}

#endif /* UTIL_FACTORY_LAMBDAFACTORY_H */
