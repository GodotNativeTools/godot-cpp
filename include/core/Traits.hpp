#ifndef GODOT_TRAITS_H
#define GODOT_TRAITS_H

#include "Variant.hpp"

namespace godot {

template<typename T>
struct VariantTraits { };

#define VARIANT_TRAITS(m_ctype, m_id)      \
	template <>                            \
	struct VariantTraits<m_ctype> {        \
		static String str() {              \
			return String(#m_ctype);       \
		}                                  \
		static int id() {                  \
			return m_id;                   \
		}                                  \
	}

VARIANT_TRAITS(bool, Variant::BOOL);
VARIANT_TRAITS(int, Variant::INT);
VARIANT_TRAITS(float, Variant::REAL);
VARIANT_TRAITS(Vector2, Variant::VECTOR2);
VARIANT_TRAITS(Vector3, Variant::VECTOR3);
VARIANT_TRAITS(Color, Variant::COLOR);
VARIANT_TRAITS(String, Variant::STRING);

VARIANT_TRAITS(PoolByteArray, Variant::POOL_BYTE_ARRAY);
VARIANT_TRAITS(PoolIntArray, Variant::POOL_INT_ARRAY);
VARIANT_TRAITS(PoolRealArray, Variant::POOL_REAL_ARRAY);
VARIANT_TRAITS(PoolVector2Array, Variant::POOL_VECTOR2_ARRAY);
VARIANT_TRAITS(PoolVector3Array, Variant::POOL_VECTOR3_ARRAY);
VARIANT_TRAITS(PoolColorArray, Variant::POOL_COLOR_ARRAY);
VARIANT_TRAITS(PoolStringArray, Variant::POOL_STRING_ARRAY);

VARIANT_TRAITS(Array, Variant::ARRAY);
VARIANT_TRAITS(Dictionary, Variant::DICTIONARY);
VARIANT_TRAITS(RID, Variant::_RID);
VARIANT_TRAITS(NodePath, Variant::NODE_PATH);
VARIANT_TRAITS(Rect2, Variant::RECT2);
VARIANT_TRAITS(AABB, Variant::AABB);
VARIANT_TRAITS(Transform2D, Variant::TRANSFORM2D);
VARIANT_TRAITS(Transform, Variant::TRANSFORM);
VARIANT_TRAITS(Basis, Variant::BASIS);
VARIANT_TRAITS(Plane, Variant::PLANE);
VARIANT_TRAITS(Quat, Variant::QUAT);


template<bool C, typename T = void>
struct enable_if {
	typedef T type;
};

template<typename T>
struct enable_if<false, T> { };

template<typename, typename>
struct is_same {
	static bool const value = false;
};

template<typename A>
struct is_same<A, A> {
	static bool const value = true;
};

template<typename B, typename D>
struct is_base_of {
	static D * create_d();
	// We exploit function overloading, these actually never get called
	static char (& chk(B *))[1]; // Array of 1 function reference taking a B* and returning a char
	static char (& chk(...))[2];
	static bool const value = sizeof chk(create_d()) == sizeof(char) && // if D can be casted to B, sizeof will be 1
							  !is_same<B    volatile const,
									   void volatile const>::value; // B should not be void*
};

}

#endif // GODOT_TRAITS_H
