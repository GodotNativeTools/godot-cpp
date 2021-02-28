#include "Variant.hpp"

#include <gdnative/variant.h>

#include "CoreTypes.hpp"
#include "Defs.hpp"
#include "GodotGlobal.hpp"
#include "Object.hpp"

#include <iostream>

namespace godot {

Variant::Variant() {
	godot::api->godot_variant_new_nil(&_godot_variant);
}

Variant::Variant(const Variant &v) {
	godot::api->godot_variant_new_copy(&_godot_variant, &v._godot_variant);
}

Variant::Variant(godot_bool p_bool) {
	godot::api->godot_variant_new_bool(&_godot_variant, p_bool);
}

Variant::Variant(godot_int p_int) {
	godot::api->godot_variant_new_int(&_godot_variant, p_int);
}

Variant::Variant(godot_float p_float) {
	godot::api->godot_variant_new_float(&_godot_variant, p_float);
}

Variant::Variant(const String &p_string) {
	godot::api->godot_variant_new_string(&_godot_variant, (godot_string *)&p_string);
}

Variant::Variant(const char *const p_cstring) {
	String s = String(p_cstring);
	godot::api->godot_variant_new_string(&_godot_variant, (godot_string *)&s);
}

Variant::Variant(const wchar_t *p_wstring) {
	String s = p_wstring;
	godot::api->godot_variant_new_string(&_godot_variant, (godot_string *)&s);
}

Variant::Variant(const char32_t *p_ucs4string) {
	String s = String(p_ucs4string);
	godot::api->godot_variant_new_string(&_godot_variant, (godot_string *)&s);
}

Variant::Variant(const Vector2 &p_vector2) {
	godot::api->godot_variant_new_vector2(&_godot_variant, (godot_vector2 *)&p_vector2);
}

Variant::Variant(const Vector2i &p_vector2i) {
	godot::api->godot_variant_new_vector2i(&_godot_variant, (godot_vector2i *)&p_vector2i);
}

Variant::Variant(const Rect2 &p_rect2) {
	godot::api->godot_variant_new_rect2(&_godot_variant, (godot_rect2 *)&p_rect2);
}

Variant::Variant(const Rect2i &p_rect2i) {
	godot::api->godot_variant_new_rect2i(&_godot_variant, (godot_rect2i *)&p_rect2i);
}

Variant::Variant(const Vector3 &p_vector3) {
	godot::api->godot_variant_new_vector3(&_godot_variant, (godot_vector3 *)&p_vector3);
}

Variant::Variant(const Vector3i &p_vector3i) {
	godot::api->godot_variant_new_vector3i(&_godot_variant, (godot_vector3i *)&p_vector3i);
}

Variant::Variant(const Plane &p_plane) {
	godot::api->godot_variant_new_plane(&_godot_variant, (godot_plane *)&p_plane);
}

Variant::Variant(const godot::AABB &p_aabb) {
	godot::api->godot_variant_new_aabb(&_godot_variant, (godot_aabb *)&p_aabb);
}

Variant::Variant(const Quat &p_quat) {
	godot::api->godot_variant_new_quat(&_godot_variant, (godot_quat *)&p_quat);
}

Variant::Variant(const Basis &p_transform) {
	godot::api->godot_variant_new_basis(&_godot_variant, (godot_basis *)&p_transform);
}

Variant::Variant(const Transform2D &p_transform) {
	godot::api->godot_variant_new_transform2d(&_godot_variant, (godot_transform2d *)&p_transform);
}

Variant::Variant(const Transform &p_transform) {
	godot::api->godot_variant_new_transform(&_godot_variant, (godot_transform *)&p_transform);
}

Variant::Variant(const Color &p_color) {
	godot::api->godot_variant_new_color(&_godot_variant, (godot_color *)&p_color);
}

Variant::Variant(const StringName &p_string_name) {
	godot::api->godot_variant_new_string_name(&_godot_variant, (godot_string_name *)&p_string_name);
}

Variant::Variant(const NodePath &p_path) {
	godot::api->godot_variant_new_node_path(&_godot_variant, (godot_node_path *)&p_path);
}

Variant::Variant(const godot::RID &p_rid) {
	godot::api->godot_variant_new_rid(&_godot_variant, (godot_rid *)&p_rid);
}

Variant::Variant(const Callable &p_callable) {
	godot::api->godot_variant_new_callable(&_godot_variant, (godot_callable *)&p_callable);
}

Variant::Variant(const Signal &p_signal) {
	godot::api->godot_variant_new_signal(&_godot_variant, (godot_signal *)&p_signal);
}

Variant::Variant(const Object *p_object) {
	if (p_object)
		godot::api->godot_variant_new_object(&_godot_variant, p_object->_owner);
	else
		godot::api->godot_variant_new_nil(&_godot_variant);
}

Variant::Variant(const Dictionary &p_dictionary) {
	godot::api->godot_variant_new_dictionary(&_godot_variant, (godot_dictionary *)&p_dictionary);
}

Variant::Variant(const Array &p_array) {
	godot::api->godot_variant_new_array(&_godot_variant, (godot_array *)&p_array);
}

Variant::Variant(const PackedByteArray &p_byte_array) {
	godot::api->godot_variant_new_packed_byte_array(&_godot_variant, (godot_packed_byte_array *)&p_byte_array);
}

Variant::Variant(const PackedInt32Array &p_int32_array) {
	godot::api->godot_variant_new_packed_int32_array(&_godot_variant, (godot_packed_int32_array *)&p_int32_array);
}

Variant::Variant(const PackedInt64Array &p_int64_array) {
	godot::api->godot_variant_new_packed_int64_array(&_godot_variant, (godot_packed_int64_array *)&p_int64_array);
}

Variant::Variant(const PackedFloat32Array &p_float32_array) {
	godot::api->godot_variant_new_packed_float32_array(&_godot_variant, (godot_packed_float32_array *)&p_float32_array);
}

Variant::Variant(const PackedFloat64Array &p_float64_array) {
	godot::api->godot_variant_new_packed_float64_array(&_godot_variant, (godot_packed_float64_array *)&p_float64_array);
}

Variant::Variant(const PackedStringArray &p_string_array) {
	godot::api->godot_variant_new_packed_string_array(&_godot_variant, (godot_packed_string_array *)&p_string_array);
}

Variant::Variant(const PackedVector2Array &p_vector2_array) {
	godot::api->godot_variant_new_packed_vector2_array(&_godot_variant, (godot_packed_vector2_array *)&p_vector2_array);
}

Variant::Variant(const PackedVector3Array &p_vector3_array) {
	godot::api->godot_variant_new_packed_vector3_array(&_godot_variant, (godot_packed_vector3_array *)&p_vector3_array);
}

Variant::Variant(const PackedColorArray &p_color_array) {
	godot::api->godot_variant_new_packed_color_array(&_godot_variant, (godot_packed_color_array *)&p_color_array);
}

Variant &Variant::operator=(const Variant &v) {
	godot::api->godot_variant_new_copy(&_godot_variant, &v._godot_variant);
	return *this;
}

Variant::operator godot_bool() const {
	return booleanize();
}
Variant::operator godot_int() const {
	return godot::api->godot_variant_as_int(&_godot_variant);
}
Variant::operator signed int() const {
	return godot::api->godot_variant_as_int(&_godot_variant);
}
Variant::operator unsigned int() const {
	return godot::api->godot_variant_as_int(&_godot_variant);
}
Variant::operator signed short() const {
	return godot::api->godot_variant_as_int(&_godot_variant);
}
Variant::operator unsigned short() const {
	return godot::api->godot_variant_as_int(&_godot_variant);
}
Variant::operator signed char() const {
	return godot::api->godot_variant_as_int(&_godot_variant);
}
Variant::operator unsigned char() const {
	return godot::api->godot_variant_as_int(&_godot_variant);
}
Variant::operator uint64_t() const {
	return godot::api->godot_variant_as_int(&_godot_variant);
}

Variant::operator godot_float() const {
	return godot::api->godot_variant_as_float(&_godot_variant);
}
Variant::operator float() const {
	return godot::api->godot_variant_as_float(&_godot_variant);
}

Variant::operator String() const {
	godot_string s = godot::api->godot_variant_as_string(&_godot_variant);
	return String(s);
}
Variant::operator Vector2() const {
	godot_vector2 s = godot::api->godot_variant_as_vector2(&_godot_variant);
	return Vector2(s);
}
Variant::operator Vector2i() const {
	godot_vector2i s = godot::api->godot_variant_as_vector2i(&_godot_variant);
	return Vector2i(s);
}
Variant::operator Rect2() const {
	godot_rect2 s = godot::api->godot_variant_as_rect2(&_godot_variant);
	return Rect2(s);
}
Variant::operator Rect2i() const {
	godot_rect2i s = godot::api->godot_variant_as_rect2i(&_godot_variant);
	return Rect2i(s);
}
Variant::operator Vector3() const {
	godot_vector3 s = godot::api->godot_variant_as_vector3(&_godot_variant);
	return Vector3(s);
}
Variant::operator Vector3i() const {
	godot_vector3i s = godot::api->godot_variant_as_vector3i(&_godot_variant);
	return Vector3i(s);
}
Variant::operator Plane() const {
	godot_plane s = godot::api->godot_variant_as_plane(&_godot_variant);
	return Plane(s);
}
Variant::operator godot::AABB() const {
	godot_aabb s = godot::api->godot_variant_as_aabb(&_godot_variant);
	return godot::AABB(s);
}
Variant::operator Quat() const {
	godot_quat s = godot::api->godot_variant_as_quat(&_godot_variant);
	return Quat(s);
}
Variant::operator Basis() const {
	godot_basis s = godot::api->godot_variant_as_basis(&_godot_variant);
	return Basis(s);
}
Variant::operator Transform() const {
	godot_transform s = godot::api->godot_variant_as_transform(&_godot_variant);
	return Transform(s);
}
Variant::operator Transform2D() const {
	godot_transform2d s = godot::api->godot_variant_as_transform2d(&_godot_variant);
	return Transform2D(s);
}

Variant::operator Color() const {
	godot_color s = godot::api->godot_variant_as_color(&_godot_variant);
	return Color(s);
}
Variant::operator NodePath() const {
	godot_node_path ret = godot::api->godot_variant_as_node_path(&_godot_variant);
	return NodePath(ret);
}
Variant::operator godot::RID() const {
	godot_rid s = godot::api->godot_variant_as_rid(&_godot_variant);
	return godot::RID(s);
}
Variant::operator Callable() const {
	godot_callable s = godot::api->godot_variant_as_callable(&_godot_variant);
	return Callable(s);
}
Variant::operator Signal() const {
	godot_signal s = godot::api->godot_variant_as_signal(&_godot_variant);
	return Signal(s);
}

Variant::operator Dictionary() const {
	godot_dictionary ret = godot::api->godot_variant_as_dictionary(&_godot_variant);
	return Dictionary(ret);
}

Variant::operator Array() const {
	godot_array ret = godot::api->godot_variant_as_array(&_godot_variant);
	return Array(ret);
}

Variant::operator PackedByteArray() const {
	godot_packed_byte_array ret = godot::api->godot_variant_as_packed_byte_array(&_godot_variant);
	return PackedByteArray(ret);
}
Variant::operator PackedInt32Array() const {
	godot_packed_int32_array ret = godot::api->godot_variant_as_packed_int32_array(&_godot_variant);
	return PackedInt32Array(ret);
}
Variant::operator PackedInt64Array() const {
	godot_packed_int64_array ret = godot::api->godot_variant_as_packed_int64_array(&_godot_variant);
	return PackedInt64Array(ret);
}
Variant::operator PackedFloat32Array() const {
	godot_packed_float32_array ret = godot::api->godot_variant_as_packed_float32_array(&_godot_variant);
	return PackedFloat32Array(ret);
}
Variant::operator PackedFloat64Array() const {
	godot_packed_float64_array ret = godot::api->godot_variant_as_packed_float64_array(&_godot_variant);
	return PackedFloat64Array(ret);
}
Variant::operator PackedStringArray() const {
	godot_packed_string_array ret = godot::api->godot_variant_as_packed_string_array(&_godot_variant);
	return PackedStringArray(ret);
}
Variant::operator PackedVector2Array() const {
	godot_packed_vector2_array ret = godot::api->godot_variant_as_packed_vector2_array(&_godot_variant);
	return PackedVector2Array(ret);
}
Variant::operator PackedVector3Array() const {
	godot_packed_vector3_array ret = godot::api->godot_variant_as_packed_vector3_array(&_godot_variant);
	return PackedVector3Array(ret);
}
Variant::operator PackedColorArray() const {
	godot_packed_color_array ret = godot::api->godot_variant_as_packed_color_array(&_godot_variant);
	return PackedColorArray(ret);
}
Variant::operator godot_object *() const {
	return godot::api->godot_variant_as_object(&_godot_variant);
}

bool Variant::operator==(const Variant &b) const {
	godot_variant result;
	bool valid = false;
	godot::api->godot_variant_evaluate(GODOT_VARIANT_OP_EQUAL, &_godot_variant, &b._godot_variant, &result, &valid);
	if (!valid) {
		return false;
	}
	valid = godot::api->godot_variant_as_bool(&result);
	godot_variant_destroy(&result);
	return valid;
}

bool Variant::operator!=(const Variant &b) const {
	return !(*this == b);
}

bool Variant::operator<(const Variant &b) const {
	godot_variant result;
	bool valid = false;
	godot::api->godot_variant_evaluate(GODOT_VARIANT_OP_LESS, &_godot_variant, &b._godot_variant, &result, &valid);
	if (!valid) {
		return false;
	}
	valid = godot::api->godot_variant_as_bool(&result);
	godot_variant_destroy(&result);
	return valid;
}

bool Variant::operator<=(const Variant &b) const {
	godot_variant result;
	bool valid = false;
	godot::api->godot_variant_evaluate(GODOT_VARIANT_OP_LESS_EQUAL, &_godot_variant, &b._godot_variant, &result, &valid);
	if (!valid) {
		return false;
	}
	valid = godot::api->godot_variant_as_bool(&result);
	godot_variant_destroy(&result);
	return valid;
}

bool Variant::operator>(const Variant &b) const {
	return !(*this <= b);
}

bool Variant::operator>=(const Variant &b) const {
	return !(*this < b);
}

Variant::Type Variant::get_type() const {
	return (Type)godot::api->godot_variant_get_type(&_godot_variant);
}

void Variant::evaluate(Operator p_op, const Variant &p_a, const Variant &p_b, Variant &r_ret, bool &r_valid) {
	godot::api->godot_variant_evaluate((godot_variant_operator)p_op, &p_a._godot_variant, &p_b._godot_variant, &r_ret._godot_variant, &r_valid);
}

Variant Variant::get_constant_value(Type p_type, const StringName &p_name) {
	Variant constant;
	constant._godot_variant = godot::api->godot_variant_get_constant_value((godot_variant_type)p_type, (godot_string_name *)&p_name);
	return constant;
}

Variant Variant::call(const StringName &method, const Variant **args, const int arg_count) {
	godot_variant v;
	godot::api->godot_variant_call(&_godot_variant, (const godot_string_name *)&method, (const godot_variant **)args, arg_count, &v, nullptr);
	return Variant(v);
}

bool Variant::has_method(const StringName &method) {
	return godot::api->godot_variant_has_method(&_godot_variant, (godot_string_name *)&method);
}

bool Variant::hash_compare(const Variant &b) const {
	return godot::api->godot_variant_hash_compare(&_godot_variant, &b._godot_variant);
}

bool Variant::booleanize() const {
	return godot::api->godot_variant_booleanize(&_godot_variant);
}

Variant Variant::duplicate(bool p_deep) const {
	Variant copy;
	*(godot_variant *)&copy = godot::api->godot_variant_duplicate(&_godot_variant, p_deep);
	return copy;
}

void Variant::blend(const Variant &p_a, const Variant &p_b, float p_c, Variant &r_dst) {
	godot::api->godot_variant_blend(&p_a._godot_variant, &p_b._godot_variant, p_c, &r_dst._godot_variant);
}

void Variant::interpolate(const Variant &p_a, const Variant &p_b, float p_c, Variant &r_dst) {
	godot::api->godot_variant_interpolate(&p_a._godot_variant, &p_b._godot_variant, p_c, &r_dst._godot_variant);
}

bool Variant::iter_init(Variant &r_iter, bool &r_valid) const {
	return godot::api->godot_variant_iter_init(&_godot_variant, &r_iter._godot_variant, &r_valid);
}

bool Variant::iter_next(Variant &r_iter, bool &r_valid) const {
	return godot::api->godot_variant_iter_next(&_godot_variant, &r_iter._godot_variant, &r_valid);
}

Variant Variant::iter_get(Variant &r_iter, bool &r_valid) const {
	Variant result;
	*(godot_variant *)&result = godot::api->godot_variant_iter_get(&_godot_variant, &r_iter._godot_variant, &r_valid);
	return result;
}

Variant::~Variant() {
	godot::api->godot_variant_destroy(&_godot_variant);
}

} // namespace godot
