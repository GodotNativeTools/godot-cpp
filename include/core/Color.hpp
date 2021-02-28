#ifndef GODOT_COLOR_HPP
#define GODOT_COLOR_HPP

#include <Math.hpp>

namespace godot {

class String;

struct Color {
	union {
		struct {
			float r;
			float g;
			float b;
			float a;
		};
		float components[4] = { 0, 0, 0, 1.0 };
	};

	uint32_t to_rgba32() const;
	uint32_t to_argb32() const;
	uint32_t to_abgr32() const;
	uint64_t to_rgba64() const;
	uint64_t to_argb64() const;
	uint64_t to_abgr64() const;
	float get_h() const;
	float get_s() const;
	float get_v() const;
	void set_hsv(float p_h, float p_s, float p_v, float p_alpha = 1.0);

	inline float &operator[](int p_idx) {
		return components[p_idx];
	}
	inline const float &operator[](int p_idx) const {
		return components[p_idx];
	}

	bool operator==(const Color &p_color) const {
		return (r == p_color.r && g == p_color.g && b == p_color.b && a == p_color.a);
	}
	bool operator!=(const Color &p_color) const {
		return (r != p_color.r || g != p_color.g || b != p_color.b || a != p_color.a);
	}

	Color operator+(const Color &p_color) const;
	void operator+=(const Color &p_color);

	Color operator-() const;
	Color operator-(const Color &p_color) const;
	void operator-=(const Color &p_color);

	Color operator*(const Color &p_color) const;
	Color operator*(float p_scalar) const;
	void operator*=(const Color &p_color);
	void operator*=(float p_scalar);

	Color operator/(const Color &p_color) const;
	Color operator/(float p_scalar) const;
	void operator/=(const Color &p_color);
	void operator/=(float p_scalar);

	bool is_equal_approx(const Color &p_color) const;

	void invert();
	Color inverted() const;

	inline Color lerp(const Color &p_to, float p_weight) const {
		Color res = *this;

		res.r += (p_weight * (p_to.r - r));
		res.g += (p_weight * (p_to.g - g));
		res.b += (p_weight * (p_to.b - b));
		res.a += (p_weight * (p_to.a - a));

		return res;
	}

	inline Color darkened(float p_amount) const {
		Color res = *this;
		res.r = res.r * (1.0f - p_amount);
		res.g = res.g * (1.0f - p_amount);
		res.b = res.b * (1.0f - p_amount);
		return res;
	}

	inline Color lightened(float p_amount) const {
		Color res = *this;
		res.r = res.r + (1.0f - res.r) * p_amount;
		res.g = res.g + (1.0f - res.g) * p_amount;
		res.b = res.b + (1.0f - res.b) * p_amount;
		return res;
	}

	inline uint32_t to_rgbe9995() const {
		const float pow2to9 = 512.0f;
		const float B = 15.0f;
		const float N = 9.0f;

		float sharedexp = 65408.000f; // Result of: ((pow2to9 - 1.0f) / pow2to9) * powf(2.0f, 31.0f - 15.0f)

		float cRed = Math::max(0.0f, Math::min(sharedexp, r));
		float cGreen = Math::max(0.0f, Math::min(sharedexp, g));
		float cBlue = Math::max(0.0f, Math::min(sharedexp, b));

		float cMax = Math::max(cRed, Math::max(cGreen, cBlue));

		float expp = Math::max(-B - 1.0f, Math::floor(Math::log(cMax) / (float)Math_LN2)) + 1.0f + B;

		float sMax = (float)floor((cMax / Math::pow(2.0f, expp - B - N)) + 0.5f);

		float exps = expp + 1.0f;

		if (0.0 <= sMax && sMax < pow2to9) {
			exps = expp;
		}

		float sRed = Math::floor((cRed / pow(2.0f, exps - B - N)) + 0.5f);
		float sGreen = Math::floor((cGreen / pow(2.0f, exps - B - N)) + 0.5f);
		float sBlue = Math::floor((cBlue / pow(2.0f, exps - B - N)) + 0.5f);

		return (uint32_t(Math::fast_ftoi(sRed)) & 0x1FF) | ((uint32_t(Math::fast_ftoi(sGreen)) & 0x1FF) << 9) | ((uint32_t(Math::fast_ftoi(sBlue)) & 0x1FF) << 18) | ((uint32_t(Math::fast_ftoi(exps)) & 0x1F) << 27);
	}

	inline Color blend(const Color &p_over) const {
		Color res;
		float sa = 1.0 - p_over.a;
		res.a = a * sa + p_over.a;
		if (res.a == 0) {
			return Color(0, 0, 0, 0);
		} else {
			res.r = (r * a * sa + p_over.r * p_over.a) / res.a;
			res.g = (g * a * sa + p_over.g * p_over.a) / res.a;
			res.b = (b * a * sa + p_over.b * p_over.a) / res.a;
		}
		return res;
	}

	inline Color to_linear() const {
		return Color(
				r < 0.04045 ? r * (1.0 / 12.92) : Math::pow((r + 0.055) * (1.0 / (1 + 0.055)), 2.4),
				g < 0.04045 ? g * (1.0 / 12.92) : Math::pow((g + 0.055) * (1.0 / (1 + 0.055)), 2.4),
				b < 0.04045 ? b * (1.0 / 12.92) : Math::pow((b + 0.055) * (1.0 / (1 + 0.055)), 2.4),
				a);
	}
	inline Color to_srgb() const {
		return Color(
				r < 0.0031308 ? 12.92 * r : (1.0 + 0.055) * Math::pow(r, 1.0f / 2.4f) - 0.055,
				g < 0.0031308 ? 12.92 * g : (1.0 + 0.055) * Math::pow(g, 1.0f / 2.4f) - 0.055,
				b < 0.0031308 ? 12.92 * b : (1.0 + 0.055) * Math::pow(b, 1.0f / 2.4f) - 0.055, a);
	}

	static Color hex(uint32_t p_hex);
	static Color hex64(uint64_t p_hex);
	static Color html(const String &p_rgba);
	static bool html_is_valid(const String &p_color);
	static Color named(const String &p_name);
	static Color named(const String &p_name, const Color &p_default);
	static int find_named_color(const String &p_name);
	static int get_named_color_count();
	static String get_named_color_name(int p_idx);
	static Color get_named_color(int p_idx);
	static Color from_string(const String &p_string, const Color &p_default);
	String to_html(bool p_alpha = true) const;
	static Color from_hsv(float p_h, float p_s, float p_v, float p_a);
	static Color from_rgbe9995(uint32_t p_rgbe);

	inline bool operator<(const Color &p_color) const; //used in set keys
	operator String() const;

	// For the binder.
	inline void set_r8(int32_t r8) { r = (Math::clamp(r8, 0, 255) / 255.0); }
	inline int32_t get_r8() const { return int32_t(Math::clamp(r * 255.0, 0.0, 255.0)); }
	inline void set_g8(int32_t g8) { g = (Math::clamp(g8, 0, 255) / 255.0); }
	inline int32_t get_g8() const { return int32_t(Math::clamp(g * 255.0, 0.0, 255.0)); }
	inline void set_b8(int32_t b8) { b = (Math::clamp(b8, 0, 255) / 255.0); }
	inline int32_t get_b8() const { return int32_t(Math::clamp(b * 255.0, 0.0, 255.0)); }
	inline void set_a8(int32_t a8) { a = (Math::clamp(a8, 0, 255) / 255.0); }
	inline int32_t get_a8() const { return int32_t(Math::clamp(a * 255.0, 0.0, 255.0)); }

	inline void set_h(float p_h) { set_hsv(p_h, get_s(), get_v()); }
	inline void set_s(float p_s) { set_hsv(get_h(), p_s, get_v()); }
	inline void set_v(float p_v) { set_hsv(get_h(), get_s(), p_v); }

	inline Color() {}

	/**
	 * RGBA construct parameters.
	 * Alpha is not optional as otherwise we can't bind the RGB version for scripting.
	 */
	inline Color(float p_r, float p_g, float p_b, float p_a) {
		r = p_r;
		g = p_g;
		b = p_b;
		a = p_a;
	}

	/**
	 * RGB construct parameters.
	 */
	inline Color(float p_r, float p_g, float p_b) {
		r = p_r;
		g = p_g;
		b = p_b;
		a = 1.0;
	}

	/**
	 * Construct a Color from another Color, but with the specified alpha value.
	 */
	inline Color(const Color &p_c, float p_a) {
		r = p_c.r;
		g = p_c.g;
		b = p_c.b;
		a = p_a;
	}

	Color(const String &p_code) {
		if (html_is_valid(p_code)) {
			*this = html(p_code);
		} else {
			*this = named(p_code);
		}
	}

	Color(const String &p_code, float p_a) {
		*this = Color(p_code);
		a = p_a;
	}

	inline explicit Color(const godot_color &p_godot_color) {
		*this = *((Color *)&p_godot_color);
	}
};

bool Color::operator<(const Color &p_color) const {
	if (r == p_color.r) {
		if (g == p_color.g) {
			if (b == p_color.b) {
				return (a < p_color.a);
			} else {
				return (b < p_color.b);
			}
		} else {
			return g < p_color.g;
		}
	} else {
		return r < p_color.r;
	}
}

inline Color operator*(float p_scalar, const Color &p_color) {
	return p_color * p_scalar;
}

} // namespace godot

#endif // GODOT_COLOR_HPP
