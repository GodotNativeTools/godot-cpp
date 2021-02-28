#include <Godot.hpp>
#include <Reference.hpp>
#include <Utilities.hpp>

using namespace godot;

class SimpleClass : public Reference {
	GODOT_CLASS(SimpleClass, Reference);

public:
	String _name;
	int _value;
	Vector2 _vec;

	SimpleClass() {}

	/** `_init` must exist as it is called by Godot. */
	void _init() {
		_name = "SimpleClass";
		_value = 50;
		_vec = Vector2(10, 20);
	}

	void test_void_method() {
		godot::Utilities::print("This is test");
	}

	Variant method(Variant arg) {
		Variant ret;
		ret = arg;

		return ret;
	}

	static void _register_methods() {
		register_method("method", &SimpleClass::method);

		register_method("get_vec_y", &SimpleClass::get_vec_y);

		/**
		 * The line below is equivalent to the following GDScript export:
		 *	 export var _name = "SimpleClass"
		 **/
		register_property<SimpleClass, String>("name", &SimpleClass::_name, String("SimpleClass"));

		/** Alternatively, with getter and setter methods: */
		register_property<SimpleClass, int>("value", &SimpleClass::set_value, &SimpleClass::get_value, 50);
		register_property<SimpleClass, Vector2>("vec", &SimpleClass::set_vec, &SimpleClass::get_vec, Vector2(10, 20));

		/** Registering a signal: **/
		register_signal<SimpleClass>("signal_name0"); // windows: error C2668: 'godot::register_signal': ambiguous call to overloaded function
		register_signal<SimpleClass>("signal_name1", "string_argument", GODOT_VARIANT_TYPE_STRING);
	}

	void set_vec(const Vector2 &p_vec) {
		_vec = p_vec;
	}

	Vector2 get_vec() const {
		return _vec;
	}

	void set_value(int p_value) {
		_value = p_value;
	}

	int get_value() const {
		return _value;
	}

	real_t get_vec_y() const {
		return _vec.y;
	}
};

/** GDNative Initialize **/
extern "C" void GDN_EXPORT godot_gdnative_init(godot_gdnative_init_options *o) {
	godot::Godot::gdnative_init(o);
}

/** GDNative Terminate **/
extern "C" void GDN_EXPORT godot_gdnative_terminate(godot_gdnative_terminate_options *o) {
	godot::Godot::gdnative_terminate(o);
}

/** NativeScript Initialize **/
extern "C" void GDN_EXPORT godot_nativescript_init(void *handle) {
	godot::Godot::nativescript_init(handle);

	godot::register_class<SimpleClass>();
}
