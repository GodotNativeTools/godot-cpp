#ifndef GODOT_TRANSFORM2D_HPP
#define GODOT_TRANSFORM2D_HPP

#include <Math.hpp>
#include <PackedVector2Array.hpp>
#include <Rect2.hpp>
#include <Vector2.hpp>

namespace godot {

struct Transform2D {
	// Warning #1: basis of Transform2D is stored differently from Basis. In terms of elements array, the basis matrix looks like "on paper":
	// M = (elements[0][0] elements[1][0])
	//     (elements[0][1] elements[1][1])
	// This is such that the columns, which can be interpreted as basis vectors of the coordinate system "painted" on the object, can be accessed as elements[i].
	// Note that this is the opposite of the indices in mathematical texts, meaning: $M_{12}$ in a math book corresponds to elements[1][0] here.
	// This requires additional care when working with explicit indices.
	// See https://en.wikipedia.org/wiki/Row-_and_column-major_order for further reading.

	// Warning #2: 2D be aware that unlike 3D code, 2D code uses a left-handed coordinate system: Y-axis points down,
	// and angle is measure from +X to +Y in a clockwise-fashion.

	Vector2 elements[3];

	inline real_t tdotx(const Vector2 &v) const { return elements[0][0] * v.x + elements[1][0] * v.y; }
	inline real_t tdoty(const Vector2 &v) const { return elements[0][1] * v.x + elements[1][1] * v.y; }

	const Vector2 &operator[](int p_idx) const { return elements[p_idx]; }
	Vector2 &operator[](int p_idx) { return elements[p_idx]; }

	inline Vector2 get_axis(int p_axis) const {
		ERR_FAIL_INDEX_V(p_axis, 3, Vector2());
		return elements[p_axis];
	}
	inline void set_axis(int p_axis, const Vector2 &p_vec) {
		ERR_FAIL_INDEX(p_axis, 3);
		elements[p_axis] = p_vec;
	}

	void invert();
	Transform2D inverse() const;

	void affine_invert();
	Transform2D affine_inverse() const;

	void set_rotation(real_t p_rot);
	real_t get_rotation() const;
	real_t get_skew() const;
	void set_skew(float p_angle);
	inline void set_rotation_and_scale(real_t p_rot, const Size2 &p_scale);
	inline void set_rotation_scale_and_skew(real_t p_rot, const Size2 &p_scale, float p_skew);
	void rotate(real_t p_phi);

	void scale(const Size2 &p_scale);
	void scale_basis(const Size2 &p_scale);
	void translate(real_t p_tx, real_t p_ty);
	void translate(const Vector2 &p_translation);

	real_t basis_determinant() const;

	Size2 get_scale() const;
	void set_scale(const Size2 &p_scale);

	inline const Vector2 &get_origin() const { return elements[2]; }
	inline void set_origin(const Vector2 &p_origin) { elements[2] = p_origin; }

	Transform2D scaled(const Size2 &p_scale) const;
	Transform2D basis_scaled(const Size2 &p_scale) const;
	Transform2D translated(const Vector2 &p_offset) const;
	Transform2D rotated(real_t p_phi) const;

	Transform2D untranslated() const;

	void orthonormalize();
	Transform2D orthonormalized() const;
	bool is_equal_approx(const Transform2D &p_transform) const;

	bool operator==(const Transform2D &p_transform) const;
	bool operator!=(const Transform2D &p_transform) const;

	void operator*=(const Transform2D &p_transform);
	Transform2D operator*(const Transform2D &p_transform) const;

	Transform2D interpolate_with(const Transform2D &p_transform, real_t p_c) const;

	inline Vector2 basis_xform(const Vector2 &p_vec) const;
	inline Vector2 basis_xform_inv(const Vector2 &p_vec) const;
	inline Vector2 xform(const Vector2 &p_vec) const;
	inline Vector2 xform_inv(const Vector2 &p_vec) const;
	inline Rect2 xform(const Rect2 &p_rect) const;
	inline Rect2 xform_inv(const Rect2 &p_rect) const;
	inline PackedVector2Array xform(const PackedVector2Array &p_array) const;
	inline PackedVector2Array xform_inv(const PackedVector2Array &p_array) const;

	operator String() const;

	Transform2D(real_t xx, real_t xy, real_t yx, real_t yy, real_t ox, real_t oy) {
		elements[0][0] = xx;
		elements[0][1] = xy;
		elements[1][0] = yx;
		elements[1][1] = yy;
		elements[2][0] = ox;
		elements[2][1] = oy;
	}

	Transform2D(const Vector2 &p_x, const Vector2 &p_y, const Vector2 &p_origin) {
		elements[0] = p_x;
		elements[1] = p_y;
		elements[2] = p_origin;
	}

	Transform2D(real_t p_rot, const Vector2 &p_pos);
	Transform2D() {
		elements[0][0] = 1.0;
		elements[1][1] = 1.0;
	}
	inline explicit Transform2D(const godot_transform2d &p_godot_transform2d) {
		*this = *((Transform2D *)&p_godot_transform2d);
	}
};

Vector2 Transform2D::basis_xform(const Vector2 &p_vec) const {
	return Vector2(
			tdotx(p_vec),
			tdoty(p_vec));
}

Vector2 Transform2D::basis_xform_inv(const Vector2 &p_vec) const {
	return Vector2(
			elements[0].dot(p_vec),
			elements[1].dot(p_vec));
}

Vector2 Transform2D::xform(const Vector2 &p_vec) const {
	return Vector2(
				   tdotx(p_vec),
				   tdoty(p_vec)) +
		   elements[2];
}

Vector2 Transform2D::xform_inv(const Vector2 &p_vec) const {
	Vector2 v = p_vec - elements[2];

	return Vector2(
			elements[0].dot(v),
			elements[1].dot(v));
}

Rect2 Transform2D::xform(const Rect2 &p_rect) const {
	Vector2 x = elements[0] * p_rect.size.x;
	Vector2 y = elements[1] * p_rect.size.y;
	Vector2 pos = xform(p_rect.position);

	Rect2 new_rect;
	new_rect.position = pos;
	new_rect.expand_to(pos + x);
	new_rect.expand_to(pos + y);
	new_rect.expand_to(pos + x + y);
	return new_rect;
}

void Transform2D::set_rotation_and_scale(real_t p_rot, const Size2 &p_scale) {
	elements[0][0] = Math::cos(p_rot) * p_scale.x;
	elements[1][1] = Math::cos(p_rot) * p_scale.y;
	elements[1][0] = -Math::sin(p_rot) * p_scale.y;
	elements[0][1] = Math::sin(p_rot) * p_scale.x;
}

void Transform2D::set_rotation_scale_and_skew(real_t p_rot, const Size2 &p_scale, float p_skew) {
	elements[0][0] = Math::cos(p_rot) * p_scale.x;
	elements[1][1] = Math::cos(p_rot + p_skew) * p_scale.y;
	elements[1][0] = -Math::sin(p_rot + p_skew) * p_scale.y;
	elements[0][1] = Math::sin(p_rot) * p_scale.x;
}

Rect2 Transform2D::xform_inv(const Rect2 &p_rect) const {
	Vector2 ends[4] = {
		xform_inv(p_rect.position),
		xform_inv(Vector2(p_rect.position.x, p_rect.position.y + p_rect.size.y)),
		xform_inv(Vector2(p_rect.position.x + p_rect.size.x, p_rect.position.y + p_rect.size.y)),
		xform_inv(Vector2(p_rect.position.x + p_rect.size.x, p_rect.position.y))
	};

	Rect2 new_rect;
	new_rect.position = ends[0];
	new_rect.expand_to(ends[1]);
	new_rect.expand_to(ends[2]);
	new_rect.expand_to(ends[3]);

	return new_rect;
}

PackedVector2Array Transform2D::xform(const PackedVector2Array &p_array) const {
	PackedVector2Array array;
	array.resize(p_array.size());

	for (int i = 0; i < p_array.size(); ++i) {
		array[i] = xform(p_array[i]);
	}
	return array;
}

PackedVector2Array Transform2D::xform_inv(const PackedVector2Array &p_array) const {
	PackedVector2Array array;
	array.resize(p_array.size());

	for (int i = 0; i < p_array.size(); ++i) {
		array[i] = xform_inv(p_array[i]);
	}
	return array;
}

} // namespace godot

#endif // GODOT_TRANSFORM2D_HPP
