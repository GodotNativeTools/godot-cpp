#include "GodotGlobal.hpp"

#include "String.hpp"

#include "Wrapped.hpp"

#include "Utilities.hpp"

static GDCALLINGCONV void *wrapper_create(void *data, const void *type_tag, godot_object *instance) {
	godot::_Wrapped *wrapper_memory = (godot::_Wrapped *)godot::api->godot_alloc(sizeof(godot::_Wrapped));

	if (!wrapper_memory)
		return NULL;
	wrapper_memory->_owner = instance;
	wrapper_memory->_type_tag = (size_t)type_tag;

	return (void *)wrapper_memory;
}

static GDCALLINGCONV void wrapper_destroy(void *data, void *wrapper) {
	if (wrapper)
		godot::api->godot_free(wrapper);
}

namespace godot {

void *_RegisterState::nativescript_handle;
int _RegisterState::language_index;

const godot_gdnative_core_api_struct *api = nullptr;
const godot_gdnative_ext_nativescript_api_struct *nativescript_api = nullptr;
const godot_gdnative_ext_pluginscript_api_struct *pluginscript_api = nullptr;
const godot_gdnative_ext_android_api_struct *android_api = nullptr;
const godot_gdnative_ext_xr_api_struct *xr_api = nullptr;
const godot_gdnative_ext_videodecoder_api_struct *videodecoder_api = nullptr;
const godot_gdnative_ext_net_api_struct *net_api = nullptr;

const void *gdnlib = nullptr;

void ___register_types();
void ___init_core_bindings();
void ___init_method_bindings();

void Godot::gdnative_init(godot_gdnative_init_options *options) {
	godot::api = options->api_struct;
	godot::gdnlib = options->gd_native_library;

	const godot_gdnative_api_struct *core_extension = godot::api->next;

	// now find our extensions
	for (int i = 0; i < godot::api->num_extensions; i++) {
		switch (godot::api->extensions[i]->type) {
			case GDNATIVE_EXT_NATIVESCRIPT: {
				godot::nativescript_api = (const godot_gdnative_ext_nativescript_api_struct *)godot::api->extensions[i];
			} break;
			case GDNATIVE_EXT_PLUGINSCRIPT: {
				godot::pluginscript_api = (const godot_gdnative_ext_pluginscript_api_struct *)godot::api->extensions[i];
			} break;
			case GDNATIVE_EXT_ANDROID: {
				godot::android_api = (const godot_gdnative_ext_android_api_struct *)godot::api->extensions[i];
			} break;
			case GDNATIVE_EXT_XR: {
				godot::xr_api = (const godot_gdnative_ext_xr_api_struct *)godot::api->extensions[i];
			} break;
			case GDNATIVE_EXT_VIDEODECODER: {
				godot::videodecoder_api = (const godot_gdnative_ext_videodecoder_api_struct *)godot::api->extensions[i];
			} break;
			case GDNATIVE_EXT_NET: {
				godot::net_api = (const godot_gdnative_ext_net_api_struct *)godot::api->extensions[i];
			} break;

			default: break;
		}
	}

	// Initialize the `language_index` here since `__register_types()` makes use of it.
	godot_nativescript_instance_binding_functions binding_funcs = {};
	binding_funcs.alloc_instance_binding_data = wrapper_create;
	binding_funcs.free_instance_binding_data = wrapper_destroy;

	godot::_RegisterState::language_index = godot::nativescript_api->godot_nativescript_register_instance_binding_data_functions(binding_funcs);

	// register these now
	___register_types();
	___init_core_bindings();
	___init_method_bindings();
}

void Godot::gdnative_terminate(godot_gdnative_terminate_options *options) {
	// reserved for future use.
}

void Godot::gdnative_profiling_add_data(const char *p_signature, uint64_t p_time) {
	godot::nativescript_api->godot_nativescript_profiling_add_data(p_signature, p_time);
}

void Godot::nativescript_init(void *handle) {
	godot::_RegisterState::nativescript_handle = handle;
}

void Godot::nativescript_terminate(void *handle) {
	godot::nativescript_api->godot_nativescript_unregister_instance_binding_data_functions(godot::_RegisterState::language_index);
}

static void print_warning(const char *description, const char *function, const char *file, int line) {
	godot::api->godot_print_warning(description, function, file, line);
}
static void print_error(const char *description, const char *function, const char *file, int line) {
	godot::api->godot_print_error(description, function, file, line);
}
static void print_script_error(const char *description, const char *function, const char *file, int line) {
	godot::api->godot_print_script_error(description, function, file, line);
}

} // namespace godot
