#ifndef GODOT_HPP
#define GODOT_HPP

#include <cstdlib>
#include <cstring>

#include <gdnative_api_struct.gen.h>
#include <nativescript/godot_nativescript.h>


#include <core/CoreTypes.hpp>
#include <core/Variant.hpp>
#include <core/Ref.hpp>

#include <Object.hpp>

#include <core/GodotGlobal.hpp>
#include <core/GodotScript.h>
#include "ObjectUtil.hpp"
#include "Traits.hpp"
#include "TagDB.hpp"


namespace godot {


#ifdef GODOT_TYPE_CHECKS
static void _print_arg_error(String expected, String got) {
	// TODO Print call stack if possible, since it's an error from the caller (GDScript usage?)
	Godot::print_error(String("expected ") + expected + String(", got ") + got, "", "", -1);
}
#endif

template<class T>
struct _ArgCast {
#ifdef GODOT_TYPE_CHECKS
	// For basic types only
	static T _arg_cast(Variant a) {
		if(a.get_type() != VariantTraits<T>::id()) {
			_print_arg_error(VariantTraits<T>::str(), a);
		}
		return a;
	}
#else
	// Pass-through convert all to Variant without checks
	static T _arg_cast(Variant a)
	{
		return a;
	}
#endif
};

// For object types
template<class T>
struct _ArgCast<T*> {
	static T *_arg_cast(Variant a) {
		return ObjectUtil<T>::get_from_variant(a);
	}
};

// For reference types
template<class T>
struct _ArgCast< Ref<T> > {
	static Ref<T> _arg_cast(Variant a) {
		T *obj = ObjectUtil<T>::get_from_variant(a);
		return obj;
	}
};

// For actual variants (type check is left to you)
template<>
struct _ArgCast<Variant> {
	static Variant _arg_cast(Variant a) {
		return a;
	}
};


// instance and destroy funcs

template<class T>
void *_godot_class_instance_func(godot_object *p, void *method_data)
{
	return __instance_script_on_existing_object<T>(p);
}

template<class T>
void _godot_class_destroy_func(godot_object *p, void *method_data, void *data)
{
	__destroy_script<T>(data);
}






// method registering

typedef godot_variant (*__godot_wrapper_method)(godot_object *, void *, void *, int, godot_variant **);


template<class T, class R, class ...args>
const char *___get_method_class_name(R (T::*p)(args... a))
{
	return T::__get_class_name();
}

template<class T, class R, class ...args>
const char *___get_method_class_name(R (T::*p)(args... a) const)
{
	return T::__get_class_name();
}







// Okay, time for some template magic.
// Many thanks to manpat from the GDL Discord Server.


// This is stuff that's available in C++14 I think, but whatever.

template<int... I>
struct __Sequence{};

template<int N, int... I>
struct __construct_sequence {
	using type = typename __construct_sequence<N-1, N-1, I...>::type;
};

template<int... I>
struct __construct_sequence<0, I...> {
	using type = __Sequence<I...>;
};


// Now the wrapping part.
template<class T, class R, class... As>
struct _WrappedMethod {
	R (T::*f)(As...);

	template<int... I>
	void apply(Variant* ret, T* obj, Variant** args, __Sequence<I...>) {
		*ret = (obj->*f)( _ArgCast<As>::_arg_cast(*args[I])... );
	}
};

template<class T, class... As>
struct _WrappedMethod<T, void, As...> {
	void (T::*f)(As...);

	template<int... I>
	void apply(Variant* ret, T* obj, Variant** args, __Sequence<I...>) {
		(obj->*f)( _ArgCast<As>::_arg_cast(*args[I])... );
	}
};


template<class T, class R, class... As>
godot_variant __wrapped_method(godot_object *, void *method_data, void *user_data, int num_args, godot_variant **args)
{
	godot_variant v;
	godot::api->godot_variant_new_nil(&v);

	T *obj = (T *) user_data;
	_WrappedMethod<T, R, As...> *method = (_WrappedMethod<T, R, As...>*) method_data;

	Variant *var = (Variant *) &v;
	Variant **arg = (Variant **) args;

#ifdef GODOT_TYPE_CHECKS
	if(num_args != sizeof...(As)) {
		// TODO Print call stack if possible, since it's an error from the caller (GDScript usage?)
		String msg = String("expected ") + String::num(sizeof...(As)) + String(" arguments, got ") + String::num(num_args);
		Godot::print_error(msg, "", "", -1);
		return v;
	}
#endif

	method->apply(var, obj, arg, typename __construct_sequence<sizeof...(As)>::type {});

	return v;
}

template<class T, class R, class... As>
void *___make_wrapper_function(R (T::*f)(As...))
{
	using MethodType = _WrappedMethod<T, R, As...>;
	MethodType *p = (MethodType *) godot::api->godot_alloc(sizeof(MethodType));
	p->f = f;
	return (void *) p;
}

template<class T, class R, class... As>
__godot_wrapper_method ___get_wrapper_function(R (T::*f)(As...))
{
	return (__godot_wrapper_method) &__wrapped_method<T, R, As...>;
}





template<class T, class R, class ...A>
void *___make_wrapper_function(R (T::*f)(A...) const)
{
	return ___make_wrapper_function((R (T::*)(A...)) f);
}


template<class T, class R, class ...A>
__godot_wrapper_method ___get_wrapper_function(R (T::*f)(A...) const)
{
	return ___get_wrapper_function((R (T::*)(A...)) f);
}













template<class M>
void register_method_cn(const char *class_name, const char *name, M method_ptr, godot_method_rpc_mode rpc_type = GODOT_METHOD_RPC_MODE_DISABLED)
{
	godot_instance_method method = {};
	method.method_data = ___make_wrapper_function(method_ptr);
	method.free_func = godot::api->godot_free;
	method.method = (__godot_wrapper_method) ___get_wrapper_function(method_ptr);


	godot_method_attributes attr = {};
	attr.rpc_type = rpc_type;

	godot::nativescript_api->godot_nativescript_register_method(godot::_RegisterState::nativescript_handle, class_name, name, attr, method);
}

template<class M>
void register_method(const char *name, M method_ptr, godot_method_rpc_mode rpc_type = GODOT_METHOD_RPC_MODE_DISABLED)
{
	register_method_cn<M>(___get_method_class_name(method_ptr), name, method_ptr, rpc_type);
}



template<class T, class P>
struct _PropertySetFunc {
	void (T::*f)(P);
	static void _wrapped_setter(godot_object *object, void *method_data, void *user_data, godot_variant *value)
	{
		_PropertySetFunc<T, P> *set_func = (_PropertySetFunc<T, P> *) method_data;
		T *obj = (T *) user_data;

		Variant *v = (Variant *) value;

		(obj->*(set_func->f))(_ArgCast<P>::_arg_cast(*v));
	}
};

template<class T, class P>
struct _PropertyGetFunc {
	P (T::*f)();
	static godot_variant _wrapped_getter(godot_object *object, void *method_data, void *user_data)
	{
		_PropertyGetFunc<T, P> *get_func = (_PropertyGetFunc<T, P> *) method_data;
		T *obj = (T *) user_data;

		godot_variant var;
		godot::api->godot_variant_new_nil(&var);

		Variant *v = (Variant *) &var;

		*v = (obj->*(get_func->f))();

		return var;
	}
};






template<class T, class P>
struct _PropertyDefaultSetFunc {
	P (T::*f);
	static void _wrapped_setter(godot_object *object, void *method_data, void *user_data, godot_variant *value)
	{
		_PropertyDefaultSetFunc<T, P> *set_func = (_PropertyDefaultSetFunc<T, P> *) method_data;
		T *obj = (T *) user_data;

		Variant *v = (Variant *) value;

		(obj->*(set_func->f)) = _ArgCast<P>::_arg_cast(*v);
	}
};

template<class T, class P>
struct _PropertyDefaultGetFunc {
	P (T::*f);
	static godot_variant _wrapped_getter(godot_object *object, void *method_data, void *user_data)
	{
		_PropertyDefaultGetFunc<T, P> *get_func = (_PropertyDefaultGetFunc<T, P> *) method_data;
		T *obj = (T *) user_data;

		godot_variant var;
		godot::api->godot_variant_new_nil(&var);

		Variant *v = (Variant *) &var;

		*v = (obj->*(get_func->f));

		return var;
	}
};


template<class T, class P>
void register_property(const char *name, P (T::*var), P default_value, godot_method_rpc_mode rpc_mode = GODOT_METHOD_RPC_MODE_DISABLED,  godot_property_usage_flags usage = GODOT_PROPERTY_USAGE_DEFAULT, godot_property_hint hint = GODOT_PROPERTY_HINT_NONE, String hint_string = "")
{
	Variant def_val = default_value;

	usage = (godot_property_usage_flags) ((int) usage | GODOT_PROPERTY_USAGE_SCRIPT_VARIABLE);

	if (def_val.get_type() == Variant::OBJECT) {
		Object *o = def_val;
		if (o && o->is_class("Resource")) {
			hint = (godot_property_hint) ((int) hint | GODOT_PROPERTY_HINT_RESOURCE_TYPE);
			hint_string = o->get_class();
		}
	}

	godot_string *_hint_string = (godot_string*) &hint_string;

	godot_property_attributes attr = {};
	attr.type = def_val.get_type();
	attr.default_value = *(godot_variant *) &def_val;
	attr.hint = hint;
	attr.rset_type = rpc_mode;
	attr.usage = usage;
	attr.hint_string = *_hint_string;

	_PropertyDefaultSetFunc<T, P> *wrapped_set = (_PropertyDefaultSetFunc<T, P> *)godot::api->godot_alloc(sizeof(_PropertyDefaultSetFunc<T, P>));
	wrapped_set->f = var;

	_PropertyDefaultGetFunc<T, P> *wrapped_get = (_PropertyDefaultGetFunc<T, P> *) godot::api->godot_alloc(sizeof(_PropertyDefaultGetFunc<T, P>));
	wrapped_get->f = var;

	godot_property_set_func set_func = {};
	set_func.method_data = (void *) wrapped_set;
	set_func.free_func   = godot::api->godot_free;
	set_func.set_func    = &_PropertyDefaultSetFunc<T, P>::_wrapped_setter;

	godot_property_get_func get_func = {};
	get_func.method_data = (void *) wrapped_get;
	get_func.free_func   = godot::api->godot_free;
	get_func.get_func    = &_PropertyDefaultGetFunc<T, P>::_wrapped_getter;

	godot::nativescript_api->godot_nativescript_register_property(godot::_RegisterState::nativescript_handle, T::__get_class_name(), name, &attr, set_func, get_func);
}




template<class T, class P>
void register_property(const char *name, void (T::*setter)(P), P (T::*getter)(), P default_value, godot_method_rpc_mode rpc_mode = GODOT_METHOD_RPC_MODE_DISABLED,  godot_property_usage_flags usage = GODOT_PROPERTY_USAGE_DEFAULT, godot_property_hint hint = GODOT_PROPERTY_HINT_NONE, String hint_string = "")
{
	Variant def_val = default_value;

	godot_property_attributes attr = {};
	attr.type = def_val.get_type();
	attr.default_value = *(godot_variant *) &def_val;
	attr.hint = hint;
	attr.rset_type = rpc_mode;
	attr.usage = usage;

	_PropertySetFunc<T, P> *wrapped_set = (_PropertySetFunc<T, P> *) godot::api->godot_alloc(sizeof(_PropertySetFunc<T, P>));
	wrapped_set->f = setter;

	_PropertyGetFunc<T, P> *wrapped_get = (_PropertyGetFunc<T, P> *) godot::api->godot_alloc(sizeof(_PropertyGetFunc<T, P>));
	wrapped_get->f = getter;

	godot_property_set_func set_func = {};
	set_func.method_data = (void *) wrapped_set;
	set_func.free_func   = godot::api->godot_free;
	set_func.set_func    = &_PropertySetFunc<T, P>::_wrapped_setter;

	godot_property_get_func get_func = {};
	get_func.method_data = (void *) wrapped_get;
	get_func.free_func   = godot::api->godot_free;
	get_func.get_func    = &_PropertyGetFunc<T, P>::_wrapped_getter;

	godot::nativescript_api->godot_nativescript_register_property(godot::_RegisterState::nativescript_handle, T::__get_class_name(), name, &attr, set_func, get_func);

}

template<class T, class P>
void register_property(const char *name, void (T::*setter)(P), P (T::*getter)() const, P default_value, godot_method_rpc_mode rpc_mode = GODOT_METHOD_RPC_MODE_DISABLED,  godot_property_usage_flags usage = GODOT_PROPERTY_USAGE_DEFAULT, godot_property_hint hint = GODOT_PROPERTY_HINT_NONE, String hint_string = "")
{
	register_property(name, setter, (P (T::*)()) getter, default_value, rpc_mode, usage, hint, hint_string);
}

template<class T>
void register_signal(String name, Dictionary args = Dictionary())
{
	godot_signal signal = {};
	signal.name = *(godot_string *)&name;
	signal.num_args = args.size();
	signal.num_default_args = 0;

	// Need to check because malloc(0) is platform-dependent. Zero arguments will leave args to NULL.
	if(signal.num_args != 0) {
		signal.args = (godot_signal_argument*) godot::api->godot_alloc(sizeof(godot_signal_argument) * signal.num_args);
		memset((void *) signal.args, 0, sizeof(godot_signal_argument) * signal.num_args);
	}

	for (int i = 0; i < signal.num_args; i++) {
		// Array entry = args[i];
		// String name = entry[0];
		String name = args.keys()[i];
		godot_string *_key = (godot_string *)&name;
		godot::api->godot_string_new_copy(&signal.args[i].name, _key);

		// if (entry.size() > 1) {
		// 	signal.args[i].type = entry[1];
		// }
		signal.args[i].type = args.values()[i];
	}

	godot::nativescript_api->godot_nativescript_register_signal(godot::_RegisterState::nativescript_handle, T::__get_class_name(), &signal);

	for (int i = 0; i < signal.num_args; i++) {
		godot::api->godot_string_destroy(&signal.args[i].name);
	}

	if(signal.args) {
		godot::api->godot_free(signal.args);
	}
}

template<class T, class... Args>
void register_signal(String name, Args... varargs)
{
	register_signal<T>(name, Dictionary::make(varargs...));
}

// Class registration

template<class T>
void register_class()
{
	godot_instance_create_func create = {};
	create.create_func = _godot_class_instance_func<T>;

	godot_instance_destroy_func destroy = {};
	destroy.destroy_func = _godot_class_destroy_func<T>;


	godot::nativescript_api->godot_nativescript_register_class(godot::_RegisterState::nativescript_handle, T::__get_class_name(), T::__get_base_class_name(), create, destroy);
	T::_register_methods();

	_TagDB::add(__get_typetag<T>(), T::__get_base_typetag());

	// This is needed for typetag unregistration...
	// As a result, you should not bind this method yourself, it is always called as virtual (like in Godot core).
	// Also, a registration variant has to be used because the function is part of GodotScript and GodotScript has no __get_class_name.
	// (using the template magic would yield the class where the function is defined, not the class we are currently registering)
	register_method_cn(T::__get_class_name(), "_notification", &T::_notification);
}

template<class T>
void register_tool_class()
{
	godot_instance_create_func create = {};
	create.create_func = _godot_class_instance_func<T>;

	godot_instance_destroy_func destroy = {};
	destroy.destroy_func = _godot_class_destroy_func<T>;


	godot::nativescript_api->godot_nativescript_register_tool_class(godot::_RegisterState::nativescript_handle, T::__get_class_name(), T::__get_base_class_name(), create, destroy);
	T::_register_methods();

	_TagDB::add(__get_typetag<T>(), T::__get_base_typetag());

	register_method_cn(T::__get_class_name(), "_notification", &T::_notification);
}


}

#endif // GODOT_H
