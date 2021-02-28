#ifndef GODOT_QUAT_HPP
#define GODOT_QUAT_HPP

#include <Math.hpp>
#include <Vector3.hpp>

namespace godot {

class Quat {
public:
	union {
		struct {
			real_t x;
			real_t y;
			real_t z;
			real_t w;
		};
		real_t components[4] = { 0, 0, 0, 1.0 };
	};

	inline real_t &operator[](int idx) {
		return components[idx];
	}
	inline const real_t &operator[](int idx) const {
		return components[idx];
	}
	inline real_t length_squared() const;
	bool is_equal_approx(const Quat &p_quat) const;
	real_t length() const;
	void normalize();
	Quat normalized() const;
	bool is_normalized() const;
	Quat inverse() const;
	inline real_t dot(const Quat &p_q) const;

	Vector3 get_euler_xyz() const;
	Vector3 get_euler_yxz() const;
	Vector3 get_euler() const { return get_euler_yxz(); };

	Quat slerp(const Quat &p_to, const real_t &p_weight) const;
	Quat slerpni(const Quat &p_to, const real_t &p_weight) const;
	Quat cubic_slerp(const Quat &p_b, const Quat &p_pre_a, const Quat &p_post_b, const real_t &p_weight) const;

	inline void get_axis_angle(Vector3 &r_axis, real_t &r_angle) const {
		r_angle = 2 * Math::acos(w);
		real_t r = ((real_t)1) / Math::sqrt(1 - w * w);
		r_axis.x = x * r;
		r_axis.y = y * r;
		r_axis.z = z * r;
	}

	void operator*=(const Quat &p_q);
	Quat operator*(const Quat &p_q) const;

	Quat operator*(const Vector3 &v) const {
		return Quat(w * v.x + y * v.z - z * v.y,
				w * v.y + z * v.x - x * v.z,
				w * v.z + x * v.y - y * v.x,
				-x * v.x - y * v.y - z * v.z);
	}
	inline explicit Quat(const godot_quat &p_godot_quat) {
		*this = *((Quat *)&p_godot_quat);
	}

	inline Vector3 xform(const Vector3 &v) const {
#ifdef MATH_CHECKS
		ERR_FAIL_COND_V(!is_normalized(), v);
#endif
		Vector3 u(x, y, z);
		Vector3 uv = u.cross(v);
		return v + ((uv * w) + u.cross(uv)) * ((real_t)2);
	}

	inline Vector3 xform_inv(const Vector3 &v) const {
		return inverse().xform(v);
	}

	inline void operator+=(const Quat &p_q);
	inline void operator-=(const Quat &p_q);
	inline void operator*=(const real_t &s);
	inline void operator/=(const real_t &s);
	inline Quat operator+(const Quat &q2) const;
	inline Quat operator-(const Quat &q2) const;
	inline Quat operator-() const;
	inline Quat operator*(const real_t &s) const;
	inline Quat operator/(const real_t &s) const;

	inline bool operator==(const Quat &p_quat) const;
	inline bool operator!=(const Quat &p_quat) const;

	operator String() const;

	inline Quat() {}

	inline Quat(real_t p_x, real_t p_y, real_t p_z, real_t p_w) :
			x(p_x),
			y(p_y),
			z(p_z),
			w(p_w) {
	}

	Quat(const Vector3 &p_axis, real_t p_angle);

	Quat(const Vector3 &p_euler);

	Quat(const Quat &p_q) :
			x(p_q.x),
			y(p_q.y),
			z(p_q.z),
			w(p_q.w) {
	}

	Quat &operator=(const Quat &p_q) {
		x = p_q.x;
		y = p_q.y;
		z = p_q.z;
		w = p_q.w;
		return *this;
	}

	Quat(const Vector3 &v0, const Vector3 &v1) // shortest arc
	{
		Vector3 c = v0.cross(v1);
		real_t d = v0.dot(v1);

		if (d < -1.0 + CMP_EPSILON) {
			x = 0;
			y = 1;
			z = 0;
			w = 0;
		} else {
			real_t s = Math::sqrt((1.0 + d) * 2.0);
			real_t rs = 1.0 / s;

			x = c.x * rs;
			y = c.y * rs;
			z = c.z * rs;
			w = s * 0.5;
		}
	}
};

real_t Quat::dot(const Quat &p_q) const {
	return x * p_q.x + y * p_q.y + z * p_q.z + w * p_q.w;
}

real_t Quat::length_squared() const {
	return dot(*this);
}

void Quat::operator+=(const Quat &p_q) {
	x += p_q.x;
	y += p_q.y;
	z += p_q.z;
	w += p_q.w;
}

void Quat::operator-=(const Quat &p_q) {
	x -= p_q.x;
	y -= p_q.y;
	z -= p_q.z;
	w -= p_q.w;
}

void Quat::operator*=(const real_t &s) {
	x *= s;
	y *= s;
	z *= s;
	w *= s;
}

void Quat::operator/=(const real_t &s) {
	*this *= 1.0 / s;
}

Quat Quat::operator+(const Quat &q2) const {
	const Quat &q1 = *this;
	return Quat(q1.x + q2.x, q1.y + q2.y, q1.z + q2.z, q1.w + q2.w);
}

Quat Quat::operator-(const Quat &q2) const {
	const Quat &q1 = *this;
	return Quat(q1.x - q2.x, q1.y - q2.y, q1.z - q2.z, q1.w - q2.w);
}

Quat Quat::operator-() const {
	const Quat &q2 = *this;
	return Quat(-q2.x, -q2.y, -q2.z, -q2.w);
}

Quat Quat::operator*(const real_t &s) const {
	return Quat(x * s, y * s, z * s, w * s);
}

Quat Quat::operator/(const real_t &s) const {
	return *this * (1.0 / s);
}

bool Quat::operator==(const Quat &p_quat) const {
	return x == p_quat.x && y == p_quat.y && z == p_quat.z && w == p_quat.w;
}

bool Quat::operator!=(const Quat &p_quat) const {
	return x != p_quat.x || y != p_quat.y || z != p_quat.z || w != p_quat.w;
}

inline Quat operator*(const real_t &p_real, const Quat &p_quat) {
	return p_quat * p_real;
}

} // namespace godot

#endif // GODOT_QUAT_HPP
