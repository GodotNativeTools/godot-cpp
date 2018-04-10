#ifndef GODOTSCRIPT_H
#define GODOTSCRIPT_H

#include <core/Variant.hpp>
#include "Object.hpp"

// Place this into GodotScript<T>-derivated classes
#define GODOT_CLASS(Name)                                             \
	public:                                                           \
		inline static const char *__get_class_name() {                \
			return static_cast<const char *>(#Name);                  \
		}                                                             \
		inline static size_t __get_base_typetag() {                   \
			return 0;                                                 \
		}                                                             \
	private:

// Place this into classes inheriting GodotScript<T>-derivated classes
#define GODOT_SUBCLASS(Name, Base)                                    \
	public:                                                           \
		inline static const char *__get_class_name() {                \
			return static_cast<const char *>(#Name);                  \
		}                                                             \
		inline static size_t __get_base_typetag() {                   \
			return __get_type_tag<Base>();                            \
		}                                                             \
	private:

namespace godot {


// Typetags gives us a chance to verify the type of the NativeScript void* pointer.
// TODO this must ideally be a GDNative feature, because exploiting Object meta is slow and annoying

template <class T>
inline size_t __get_typetag() {
	// TODO For some reason static_cast doesn't work... maybe this should be reviewed
	return (size_t)(&T::__get_class_name);
}

inline size_t __get_typetag(Object *obj) {
	return obj->get_meta("&");
}

template <class T>
inline void __add_typetag(Object *obj) {
	obj->set_meta("&", __get_typetag<T>());
}

inline void __remove_typetag(Object *obj) {
	obj->set_meta("&", Variant());
}


// Your custom C++ classes Godot will access should inherit this,
// where T is the type of the Godot object you want to extend
template<class T>
class GodotScript {
public:
	T *owner;

	// GodotScript() {}

	void _init() {}

	// Don't forget to call the base if you override this function.
	// Also, don't register it
	virtual void _notification(int p_what) {
		if(p_what == Object::NOTIFICATION_PREDELETE) {
			__remove_typetag(owner);
		}
	}

	static const char *__get_base_class_name()
	{
		return T::__get_class_name();
	}

	static T *__instance_base() {
		return new T;
	}

	static void _register_methods() {}
};

// Creates a new script the same way it would happen in Godot scripting:
// The script is created and attached to a new object of the type it extends
template <class T>
T *__instance_script() {
	// TODO use Godot allocator
	auto *s = new T();
	auto *b = T::__instance_base(); // made that function so we can rule out a template param
	__add_typetag<T>(b);
	s->owner = b;
	s->_init();
	return s;
}

// Called by the engine
template <class T>
T *__instance_script_on_existing_object(godot_object *p) {
	// TODO use Godot allocator
	T *s = new T();
	*(godot_object **) &s->owner = p;
	__add_typetag<T>(s->owner);
	s->_init();
	return s;
}

// Called by the engine
template <class T>
void __destroy_script(godot_object *p) {
	// No need to check the type here, we should be able to trust the engine

	// Argh we can't remove the typetag from here, the Object is being destroyed...
	//__remove_typetag((Object*)p);

	T *s = (T*)p;
	delete s;
}

}

#endif // GODOTSCRIPT_H
