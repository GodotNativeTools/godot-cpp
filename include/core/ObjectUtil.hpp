#ifndef GODOT_OBJECTUTIL_HPP
#define GODOT_OBJECTUTIL_HPP

#include "Traits.hpp"
#include "GodotScript.h"
#include "TagDB.hpp"

namespace godot {

// In Godot, script inheritance is seamless.
// But using the C++ bindings, Godot objects and "script" classes are different (for now).
// This utility unifies some functions in a non-intrusive way.

// For GodotScript<T>-derivated classes
template<typename T, typename = void>
struct ObjectUtil {

	static inline T *instance() {
		return T::__instance_script();
	}

	template<class C>
	static inline C *cast_to(T *obj) {
		// TODO Ability to cast from a GodotScript to its base Godot class
#ifdef GODOT_TYPE_CHECKS
		// Note: it is currently possible to use dynamic_cast because GodotScript has a virtual function.
		// However we can't count on this if it gets removed one day.
		return dynamic_cast<C*>(obj);
#else
		return static_cast<C*>(obj);
#endif
	}

	static T *get_from_variant(const Variant &a) {
		Object *obj = (Object*)a;
#ifdef GODOT_TYPE_CHECKS
		if(obj == nullptr)
			return nullptr;
		size_t tag = __get_typetag(obj);
		if(!_TagDB::is_tag_compatible(tag, __get_typetag<T>()))
			return nullptr;
#endif
		return (T *) godot::nativescript_api->godot_nativescript_get_userdata(obj);
	}
};

// For godot::Object-derivated classes
template<typename T>
struct ObjectUtil<T, typename enable_if<is_base_of<Object, T>::value>::type> {

	static inline T *instance() {
		return new T;
	}

	template<class C>
	static inline C *cast_to(T *obj) {
		// TODO Ability to cast from a Godot class to its extending GodotScript
#ifdef GODOT_TYPE_CHECKS
		if(obj == nullptr)
			return nullptr;
		if(T::__get_class_name == C::__get_class_name)
			return (C*)obj;
		if(obj->is_class(C::__get_class_name()))
			return (C*)obj;
		return nullptr;
#else
		return (C*)obj;
#endif
	}

	static T *get_from_variant(const Variant &a) {
		Object *obj = (Object*)a;
#ifdef GODOT_TYPE_CHECKS
		if(obj == nullptr)
			return nullptr;
		if(!obj->is_class(T::__get_class_name()))
			return nullptr;
#endif
		return (T *) obj;
	}
};

}

#endif // GODOT_OBJECTUTIL_HPP
