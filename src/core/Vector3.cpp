#include <Vector3.hpp>

#include <Basis.hpp>

namespace godot {

void Vector3::rotate(const Vector3 &p_axis, real_t p_phi) {
	*this = Basis(p_axis, p_phi).xform(*this);
}

Vector3 Vector3::rotated(const Vector3 &p_axis, real_t p_phi) const {
	Vector3 r = *this;
	r.rotate(p_axis, p_phi);
	return r;
}

void Vector3::set_axis(int p_axis, real_t p_value) {
	ERR_FAIL_INDEX(p_axis, 3);
	coord[p_axis] = p_value;
}

real_t Vector3::get_axis(int p_axis) const {
	ERR_FAIL_INDEX_V(p_axis, 3, 0);
	return operator[](p_axis);
}

int Vector3::min_axis() const {
	return x < y ? (x < z ? 0 : 2) : (y < z ? 1 : 2);
}

int Vector3::max_axis() const {
	return x < y ? (y < z ? 2 : 1) : (x < z ? 2 : 0);
}

void Vector3::snap(Vector3 p_step) {
	x = Math::snapped(x, p_step.x);
	y = Math::snapped(y, p_step.y);
	z = Math::snapped(z, p_step.z);
}

Vector3 Vector3::snapped(Vector3 p_step) const {
	Vector3 v = *this;
	v.snap(p_step);
	return v;
}

Vector3 Vector3::cubic_interpolate(const Vector3 &p_b, const Vector3 &p_pre_a, const Vector3 &p_post_b, real_t p_weight) const {
	Vector3 p0 = p_pre_a;
	Vector3 p1 = *this;
	Vector3 p2 = p_b;
	Vector3 p3 = p_post_b;

	real_t t = p_weight;
	real_t t2 = t * t;
	real_t t3 = t2 * t;

	Vector3 out;
	out = 0.5 * ((p1 * 2.0) +
						(-p0 + p2) * t +
						(2.0 * p0 - 5.0 * p1 + 4.0 * p2 - p3) * t2 +
						(-p0 + 3.0 * p1 - 3.0 * p2 + p3) * t3);
	return out;
}

Vector3 Vector3::move_toward(const Vector3 &p_to, const real_t p_delta) const {
	Vector3 v = *this;
	Vector3 vd = p_to - v;
	real_t len = vd.length();
	return len <= p_delta || len < CMP_EPSILON ? p_to : v + vd / len * p_delta;
}

Basis Vector3::outer(const Vector3 &p_b) const {
	Vector3 row0(x * p_b.x, x * p_b.y, x * p_b.z);
	Vector3 row1(y * p_b.x, y * p_b.y, y * p_b.z);
	Vector3 row2(z * p_b.x, z * p_b.y, z * p_b.z);

	return Basis(row0, row1, row2);
}

Basis Vector3::to_diagonal_matrix() const {
	return Basis(x, 0, 0,
			0, y, 0,
			0, 0, z);
}

bool Vector3::is_equal_approx(const Vector3 &p_v) const {
	return Math::is_equal_approx(x, p_v.x) && Math::is_equal_approx(y, p_v.y) && Math::is_equal_approx(z, p_v.z);
}

Vector3::operator String() const {
	return (String(x) + ", " + String(y) + ", " + String(z));
}

} // namespace godot
