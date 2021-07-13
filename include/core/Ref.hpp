#ifndef REF_H
#define REF_H

#include "Godot.hpp"
#include "GodotGlobal.hpp"
#include "Reference.hpp"
#include "Variant.hpp"

namespace godot {

// Replicates Godot's Ref<T> behavior
// Rewritten from f5234e70be7dec4930c2d5a0e829ff480d044b1d.
template <class T>
class Ref {
	// TODO For this nice check to work, each class must actually #include Reference classes mentionned in its methods,
	// which might be annoying for coders who prefer to forward-declare to reduce compile times
	// static_assert(std::is_base_of<Reference, T>::value,
	// 		"Ref<T> can only be used with classes deriving from Reference");

	T *reference = nullptr;

	void ref(const Ref &p_from) {
		if (p_from.reference == reference)
			return;

		unref();

		reference = p_from.reference;
		if (reference)
			reference->reference();
	}

	void ref_pointer(T *p_ref) {
		ERR_FAIL_COND(p_ref == nullptr);

		if (p_ref->init_ref())
			reference = p_ref;
	}

public:
	inline bool operator<(const Ref<T> &p_r) const {
		return reference < p_r.reference;
	}
	inline bool operator==(const Ref<T> &p_r) const {
		return reference == p_r.reference;
	}
	inline bool operator!=(const Ref<T> &p_r) const {
		return reference != p_r.reference;
	}

	inline T *operator->() {
		return reference;
	}

	inline T *operator*() {
		return reference;
	}

	inline const T *operator->() const {
		return reference;
	}

	inline const T *ptr() const {
		return reference;
	}
	inline T *ptr() {
		return reference;
	}

	inline const T *operator*() const {
		return reference;
	}

	operator Variant() const {
		// Note: the C API handles the cases where the object is a Reference,
		// so the Variant will be correctly constructed with a RefPtr engine-side
		return Variant((Object *)reference);
	}

	void operator=(const Ref &p_from) {
		ref(p_from);
	}

	template <class T_Other>
	void operator=(const Ref<T_Other> &p_from) {
		Reference *refb = const_cast<Reference *>(static_cast<const Reference *>(p_from.ptr()));
		if (refb == nullptr) {
			unref();
			return;
		}
		Ref r;
		r.reference = Object::cast_to<T>(refb);
		ref(r);
		r.reference = nullptr;
	}

	void operator=(const Variant &p_variant) {
		Object *refb = T::___get_from_variant(p_variant);
		if (refb == nullptr) {
			unref();
			return;
		}
		Ref r;
		r.reference = Object::cast_to<T>(refb);
		ref(r);
		r.reference = nullptr;
	}

	Ref(const Ref &p_from) {
		reference = nullptr;
		ref(p_from);
	}

	template <class T_Other>
	Ref(const Ref<T_Other> &p_from) {
		reference = nullptr;
		Reference *refb = const_cast<Reference *>(static_cast<const Reference *>(p_from.ptr()));
		if (refb == nullptr) {
			unref();
			return;
		}
		Ref r;
		r.reference = Object::cast_to<T>(refb);
		ref(r);
		r.reference = nullptr;
	}

	Ref(T *p_reference) {
		if (p_reference)
			ref_pointer(p_reference);
		else
			reference = nullptr;
	}

	Ref(const Variant &p_variant) {
		reference = nullptr;
		Object *refb = T::___get_from_variant(p_variant);
		if (refb == nullptr) {
			unref();
			return;
		}
		Ref r;
		r.reference = Object::cast_to<T>(refb);
		ref(r);
		r.reference = nullptr;
	}

	inline bool is_valid() const { return reference != nullptr; }
	inline bool is_null() const { return reference == nullptr; }

	void unref() {
		//TODO this should be moved to mutexes, since this engine does not really
		// do a lot of referencing on references and stuff
		// mutexes will avoid more crashes?

		if (reference && reference->unreference()) {
			//memdelete(reference);
			reference->free();
		}
		reference = nullptr;
	}

	void instance() {
		//ref(memnew(T));
		ref(T::_new());
	}

	Ref() {
		reference = nullptr;
	}

	~Ref() {
		unref();
	}

	// Used exclusively in the bindings to recreate the Ref Godot encapsulates in return values,
	// without adding to the refcount.
	inline static Ref<T> __internal_constructor(Object *obj) {
		Ref<T> r;
		r.reference = (T *)obj;
		return r;
	}
};

} // namespace godot

#endif
