#ifndef VARIANT_H
#define VARIANT_H

#include <gdnative/variant.h>

// #include "CoreTypes.hpp"
#include "Defs.hpp"

#include <iostream>

namespace godot {

class AABB;
class Array;
class Basis;
class Callable;
class Color;
class Dictionary;
class NodePath;
class Object;
class PackedByteArray;
class PackedColorArray;
class PackedFloat32Array;
class PackedFloat64Array;
class PackedInt32Array;
class PackedInt64Array;
class PackedStringArray;
class PackedVector2Array;
class PackedVector3Array;
class Plane;
class Quat;
class Rect2;
class Rect2i;
class Signal;
class String;
class StringName;
class Transform;
class Transform2D;
class Variant;
class Vector2;
class Vector2i;
class Vector3;
class Vector3i;
class RID;

class Variant {
	godot_variant _godot_variant;

	friend class Array;

public:
	enum Type {
		NIL,

		// atomic types
		BOOL,
		INT,
		FLOAT,
		STRING,

		// math types
		VECTOR2,
		VECTOR2I,
		RECT2,
		RECT2I,
		VECTOR3,
		VECTOR3I,
		TRANSFORM2D,
		PLANE,
		QUAT,
		AABB,
		BASIS,
		TRANSFORM,

		// misc types
		COLOR,
		STRING_NAME,
		NODE_PATH,
		RID,
		OBJECT,
		CALLABLE,
		SIGNAL,
		DICTIONARY,
		ARRAY,

		// typed arrays
		PACKED_BYTE_ARRAY,
		PACKED_INT32_ARRAY,
		PACKED_INT64_ARRAY,
		PACKED_FLOAT32_ARRAY,
		PACKED_FLOAT64_ARRAY,
		PACKED_STRING_ARRAY,
		PACKED_VECTOR2_ARRAY,
		PACKED_VECTOR3_ARRAY,
		PACKED_COLOR_ARRAY,

		VARIANT_MAX
	};

	enum Operator {
		//comparison
		OP_EQUAL,
		OP_NOT_EQUAL,
		OP_LESS,
		OP_LESS_EQUAL,
		OP_GREATER,
		OP_GREATER_EQUAL,
		//mathematic
		OP_ADD,
		OP_SUBTRACT,
		OP_MULTIPLY,
		OP_DIVIDE,
		OP_NEGATE,
		OP_POSITIVE,
		OP_MODULE,
		//bitwise
		OP_SHIFT_LEFT,
		OP_SHIFT_RIGHT,
		OP_BIT_AND,
		OP_BIT_OR,
		OP_BIT_XOR,
		OP_BIT_NEGATE,
		//logic
		OP_AND,
		OP_OR,
		OP_XOR,
		OP_NOT,
		//containment
		OP_IN,
		OP_MAX
	};

	Variant();

	inline explicit Variant(const godot_variant &variant) :
			_godot_variant(variant) {}

	Variant(const Variant &v);

	Variant(godot_bool p_bool);

	inline Variant(signed int p_int) :
			Variant((godot_int)p_int) {}

	inline Variant(unsigned int p_int) :
			Variant((godot_int)p_int) {}

	inline Variant(signed short p_short) :
			Variant((godot_int)p_short) {}

	inline Variant(unsigned short p_short) :
			Variant((godot_int)p_short) {}

	inline Variant(signed char p_char) :
			Variant((godot_int)p_char) {}

	inline Variant(unsigned char p_char) :
			Variant((unsigned int)p_char) {}

	Variant(godot_int p_int);

	inline Variant(float p_float) :
			Variant((godot_float)p_float) {}

	Variant(godot_float p_double);

	Variant(const String &p_string);

	Variant(const char *const p_cstring);

	Variant(const wchar_t *p_wstring);

	Variant(const char32_t *p_ucs4string);

	Variant(const Vector2 &p_vector2);
	Variant(const Vector2i &p_vector2i);

	Variant(const Rect2 &p_rect2);
	Variant(const Rect2i &p_rect2i);

	Variant(const Vector3 &p_vector3);
	Variant(const Vector3i &p_vector3i);

	Variant(const Plane &p_plane);

	Variant(const godot::AABB &p_aabb);

	Variant(const Quat &p_quat);

	Variant(const Basis &p_transform);

	Variant(const Transform2D &p_transform);

	Variant(const Transform &p_transform);

	Variant(const Color &p_color);

	Variant(const StringName &p_string_name);

	Variant(const NodePath &p_path);

	Variant(const godot::RID &p_rid);

	Variant(const Callable &p_callable);

	Variant(const Signal &p_signal);

	Variant(const Object *p_object);

	Variant(const Dictionary &p_dictionary);

	Variant(const Array &p_array);

	Variant(const PackedByteArray &p_byte_array);

	Variant(const PackedInt32Array &p_int32_array);

	Variant(const PackedInt64Array &p_int64_array);

	Variant(const PackedFloat32Array &p_float32_array);

	Variant(const PackedFloat64Array &p_float64_array);

	Variant(const PackedStringArray &p_string_array);

	Variant(const PackedVector2Array &p_vector2_array);

	Variant(const PackedVector3Array &p_vector3_array);

	Variant(const PackedColorArray &p_color_array);

	Variant &operator=(const Variant &v);

	operator godot_bool() const;
	operator godot_int() const;
	operator signed int() const;
	operator unsigned int() const;
	operator signed short() const;
	operator unsigned short() const;
	operator signed char() const;
	operator unsigned char() const;
	operator uint64_t() const;

	operator godot_float() const;
	operator float() const;

	operator String() const;
	operator Vector2() const;
	operator Vector2i() const;
	operator Rect2() const;
	operator Rect2i() const;
	operator Vector3() const;
	operator Vector3i() const;
	operator Plane() const;
	operator godot::AABB() const;
	operator Quat() const;
	operator Basis() const;
	operator Transform() const;
	operator Transform2D() const;

	operator Color() const;

	operator NodePath() const;
	operator godot::RID() const;
	operator Callable() const;
	operator Signal() const;
	operator godot_object *() const;

	template <typename T>
	operator T *() const { return static_cast<T *>(T::___get_from_variant(*this)); }

	operator Dictionary() const;
	operator Array() const;

	operator PackedByteArray() const;
	operator PackedInt32Array() const;
	operator PackedInt64Array() const;
	operator PackedFloat32Array() const;
	operator PackedFloat64Array() const;
	operator PackedStringArray() const;
	operator PackedVector2Array() const;
	operator PackedVector3Array() const;
	operator PackedColorArray() const;

	bool operator==(const Variant &p_b) const;
	bool operator!=(const Variant &p_b) const;
	bool operator<(const Variant &p_b) const;
	bool operator<=(const Variant &p_b) const;
	bool operator>(const Variant &p_b) const;
	bool operator>=(const Variant &p_b) const;

	Type get_type() const;

	static void evaluate(Operator p_op, const Variant &p_a, const Variant &p_b, Variant &r_ret, bool &r_valid);
	static Variant get_constant_value(Type p_type, const StringName &p_name);
	Variant call(const StringName &p_method, const Variant **p_args, const int p_arg_count);
	Variant get(const Variant &p_key, bool &r_valid);
	Variant get_named(const StringName &p_name, bool &r_valid);
	Variant get_keyed(const Variant &p_key, bool &r_valid);
	Variant get_indexed(godot_int p_index, bool &r_valid, bool &r_oob);
	uint64_t get_indexed_size();
	void set(const Variant &p_key, const Variant &p_value, bool &r_valid);
	void set_named(const StringName &p_name, const Variant &p_value, bool &r_valid);
	void set_keyed(const Variant &p_key, const Variant &p_value, bool &r_valid);
	void set_indexed(godot_int p_index, const Variant &p_value, bool &r_valid);

	bool has_method(const StringName &p_method);

	bool hash_compare(const Variant &p_b) const;
	bool booleanize() const;
	Variant duplicate(bool p_deep) const;
	static void blend(const Variant &p_a, const Variant &p_b, float p_c, Variant &r_dst);
	static void interpolate(const Variant &p_a, const Variant &p_b, float p_c, Variant &r_dst);

	bool iter_init(Variant &r_iter, bool &r_valid) const;
	bool iter_next(Variant &r_iter, bool &r_valid) const;
	Variant iter_get(Variant &r_iter, bool &r_valid) const;

	~Variant();
};

} // namespace godot

#endif // VARIANT_H
