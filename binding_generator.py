#!/usr/bin/env python

import json
import re

# comment.

# Convenience function for using template get_node
def correct_method_name(method_list):
    for method in method_list:
        if method["name"] == "get_node":
            method["name"] = "get_node_internal"


classes = []

def generate_bindings(path, use_template_get_node):

    global classes
    classes = json.load(open(path))

    icalls = set()

    for c in classes:
        # print c['name']
        used_classes = get_used_classes(c)
        if use_template_get_node and c["name"] == "Node":
            correct_method_name(c["methods"])

        header_func = generate_class_header
        impl_func = generate_class_implementation

        if c["name"] == "CoreConstants":
            # Special generator since we want this to be included in core types.
            # So we don't want this to depend on Object or Variant.
            header_func = generate_core_constants_header
            impl_func = generate_core_constants_implementation

        header = header_func(used_classes, c, use_template_get_node)
        impl = impl_func(icalls, used_classes, c, use_template_get_node)

        header_file = open("include/gen/" + strip_name(c["name"]) + ".hpp", "w+")
        header_file.write(header)

        source_file = open("src/gen/" + strip_name(c["name"]) + ".cpp", "w+")
        source_file.write(impl)


    icall_header_file = open("include/gen/__icalls.hpp", "w+")
    icall_header_file.write(generate_icall_header(icalls))

    register_types_file = open("src/gen/__register_types.cpp", "w+")
    register_types_file.write(generate_type_registry(classes))

    init_method_bindings_file = open("src/gen/__init_method_bindings.cpp", "w+")
    init_method_bindings_file.write(generate_init_method_bindings(classes))


def generate_builtin_bindings(path):
    global gen_core_types
    gen_core_types = []
    global builtin_types
    builtin_types = json.load(open(path))

    for tp in builtin_types:
        header_func = generate_builtin_type_header
        impl_func = generate_builtin_type_implementation

        if not is_core_type(tp["name"]):
            if tp["name"] == "Utilities":
                header_func = generate_utilities_header
                impl_func = generate_utilities_implementation
            else:
                continue

        if is_inlined_type(tp["name"]):
            # Don't generate inlined math types.
            continue

        gen_core_types.append(tp["name"])

        (used_types, has_varargs, complete_types) = get_used_types(tp)

        header = header_func(tp, used_types, has_varargs, complete_types)

        impl = impl_func(tp, used_types, has_varargs, complete_types)

        header_file = open("include/gen/" + strip_name(tp["name"]) + ".hpp", "w+")
        header_file.write(header)

        source_file = open("src/gen/" + strip_name(tp["name"]) + ".cpp", "w+")
        source_file.write(impl)

    init_method_bindings_file = open("src/gen/__init_core_bindings.cpp", "w+")
    init_method_bindings_file.write(generate_init_core_bindings(builtin_types))


def is_reference_type(t):
    for c in classes:
        if c['name'] != t:
            continue
        if c['is_reference']:
            return True
    return False

def make_gdnative_type(t, ref_allowed, is_return):
    if is_enum(t):
        if t == "enum.Vector3::Axis":
            # Exception: enums in builtin types are not currently supported.
            # This can be removed when we re-implement Vector3 in the bindings.
            # Or when Godot is properly fixed
            return "int "
        enum_name = remove_enum_prefix(t)
        if is_nested_type(enum_name):
            return enum_name + " "
        else:
            return "CoreConstants::" + enum_name + " "
    elif is_class_type(t):
        if is_reference_type(t) and ref_allowed:
            return "Ref<" + strip_name(t) + "> "
        else:
            return strip_name(t) + " *"
    else:
        if not is_primitive(strip_name(t)) and not is_return:
            return strip_name(t) + " &"
        if t == "int":
            return "int64_t "
        if t == "float" or t == "real":
            return "double "
        return strip_name(t) + " "


def generate_class_header(used_classes, c, use_template_get_node):

    class_name = strip_name(c["name"])

    source = []
    source.append("#ifndef GODOT_CPP_" + strip_name(c["name"]).upper() + "_HPP")
    source.append("#define GODOT_CPP_" + strip_name(c["name"]).upper() + "_HPP")
    source.append("")
    source.append("")

    source.append("#include <gdnative_api_struct.gen.h>")
    source.append("#include <stdint.h>")
    source.append("")

    source.append("#include <core/CoreTypes.hpp>")


    # Ref<T> is not included in object.h in Godot either,
    # so don't include it here because it's not needed
    if class_name != "Object" and class_name != "Reference":
        source.append("#include <core/Ref.hpp>")
        ref_allowed = True
    else:
        source.append("#include <core/TagDB.hpp>")
        ref_allowed = False

    source.append("#include <core/Variant.hpp>")

    included = []

    for used_class in used_classes:
        if is_enum(used_class) and is_nested_type(used_class):
            used_class_name = remove_enum_prefix(extract_nested_type(used_class))
            if used_class_name not in included:
                included.append(used_class_name)
                source.append("#include \"" + used_class_name + ".hpp\"")
        elif is_enum(used_class) and is_nested_type(used_class) and not is_nested_type(used_class, class_name):
            used_class_name = remove_enum_prefix(used_class)
            if used_class_name not in included:
                included.append(used_class_name)
                source.append("#include \"" + used_class_name + ".hpp\"")
        elif is_enum(used_class) and not is_nested_type(used_class):
            used_class_name = "CoreConstants"
            if used_class_name not in included:
                included.append(used_class_name)
                source.append("#include \"" + used_class_name + ".hpp\"")


    source.append("")

    if c["base_class"] != "":
        source.append("#include \"" + strip_name(c["base_class"]) + ".hpp\"")


    source.append("namespace godot {")
    source.append("")


    for used_type in used_classes:
        if is_enum(used_type) or is_nested_type(used_type, class_name):
            continue
        else:
            source.append("class " + strip_name(used_type) + ";")


    source.append("")

    vararg_templates = ""

    # generate the class definition here
    source.append("class " + class_name + (" : public _Wrapped" if c["base_class"] == "" else (" : public " + strip_name(c["base_class"])) ) + " {")

    if c["base_class"] == "":
        source.append("public: enum { ___CLASS_IS_SCRIPT = 0, };")
        source.append("")
        source.append("private:")

    if c["singleton"]:
        source.append("\tstatic " + class_name + " *_singleton;")
        source.append("")
        source.append("\t" + class_name + "();")
        source.append("")

    # Generate method table
    source.append("\tstruct ___method_bindings {")

    for method in c["methods"]:
        source.append("\t\tgodot_method_bind *mb_" + method["name"] + ";")

    source.append("\t};")
    source.append("\tstatic ___method_bindings ___mb;")
    source.append("\tstatic void *_detail_class_tag;")
    source.append("")
    source.append("public:")
    source.append("\tstatic void ___init_method_bindings();")

    # class id from core engine for casting
    source.append("\tinline static size_t ___get_id() { return (size_t)_detail_class_tag; }")

    source.append("")


    if c["singleton"]:
        source.append("\tstatic inline " + class_name + " *get_singleton()")
        source.append("\t{")
        source.append("\t\tif (!" + class_name + "::_singleton) {")
        source.append("\t\t\t" + class_name + "::_singleton = new " + class_name + ";")
        source.append("\t\t}")
        source.append("\t\treturn " + class_name + "::_singleton;")
        source.append("\t}")
        source.append("")

        # godot::api->godot_global_get_singleton((char *) \"" + strip_name(c["name"]) + "\");"

    # class name:
    # Two versions needed needed because when the user implements a custom class,
    # we want to override `___get_class_name` while `___get_godot_class_name` can keep returning the base name
    source.append("\tstatic inline const char *___get_class_name() { return (const char *) \"" + strip_name(c["name"]) + "\"; }")
    source.append("\tstatic inline const char *___get_godot_class_name() { return (const char *) \"" + strip_name(c["name"]) + "\"; }")

    source.append("\tstatic inline Object *___get_from_variant(Variant a) { godot_object *o = (godot_object*) a; return (o) ? (Object *) godot::nativescript_api->godot_nativescript_get_instance_binding_data(godot::_RegisterState::language_index, o) : nullptr; }")

    enum_values = []

    source.append("\n\t// enums")
    for enum in c["enums"]:
        if "." in enum["name"]:
            # Exception for some core enums like `Variant.Type` and `Variant.Operator`.
            continue
        source.append("\tenum " + strip_name(enum["name"]) + " {")
        for value in enum["values"]:
            source.append("\t\t" + remove_nested_type_prefix(value) + " = " + str(enum["values"][value]) + ",")
            enum_values.append(value)
        source.append("\t};")

    source.append("\n\t// constants")

    for name in c["constants"]:
        if name not in enum_values:
            source.append("\tconst static int " + name + " = " + str(c["constants"][name]) + ";")


    if c["instantiable"]:
        source.append("")
        source.append("")
        source.append("\tstatic " + class_name + " *_new();")

    source.append("\n\t// methods")


    if class_name == "Object":
        source.append("#ifndef GODOT_CPP_NO_OBJECT_CAST")
        source.append("\ttemplate<class T>")
        source.append("\tstatic T *cast_to(const Object *obj);")
        source.append("#endif")
        source.append("")

    for method in c["methods"]:
        method_signature = ""

        # TODO decide what to do about virtual methods
        # method_signature += "virtual " if method["is_virtual"] else ""
        method_signature += make_gdnative_type(method["return_type"], ref_allowed, True)
        method_name = escape_cpp(method["name"])
        method_signature +=  method_name + "("


        has_default_argument = False
        method_arguments = ""

        for i, argument in enumerate(method["arguments"]):
            method_signature += "const " + make_gdnative_type(argument["type"], ref_allowed, False)
            argument_name = escape_cpp(argument["name"])
            method_signature += argument_name
            method_arguments += argument_name


            # default arguments
            def escape_default_arg(_type, default_value):
                if _type == "Color":
                    return "Color(" + default_value + ")"
                if _type == "bool" or _type == "int":
                    return default_value.lower()
                if _type == "Array":
                    return "Array()"
                if _type in ["PackedByteArray", "PackedInt32Array", "PackedInt64Array", "PackedFloat32Array", "PackedFloat64Array", "PackedStringArray", "PackedVector2Array", "PackedVector3Array", "PackedColorArray"]:
                    return _type + "()"
                if _type == "Vector2":
                    return "Vector2" + default_value
                if _type == "Vector2i":
                    return "Vector2i" + default_value
                if _type == "Vector3":
                    return "Vector3" + default_value
                if _type == "Vector3i":
                    return "Vector3i" + default_value
                if _type == "Transform":
                    return "Transform()"
                if _type == "Transform2D":
                    return "Transform2D()"
                if _type == "Rect2":
                    return "Rect2" + default_value
                if _type == "Rect2i":
                    return "Rect2i" + default_value
                if _type == "Variant":
                    return "Variant()" if default_value == "Null" else default_value
                if _type == "String" or _type == "StringName":
                    return "\"" + default_value + "\""
                if _type == "RID":
                    return "RID()"

                if default_value == "Null" or default_value == "[Object:null]":
                    return "nullptr"

                return default_value




            if argument["has_default_value"] or has_default_argument:
                method_signature += " = " + escape_default_arg(argument["type"], argument["default_value"])
                has_default_argument = True



            if i != len(method["arguments"]) - 1:
                method_signature += ", "
                method_arguments += ","

        if method["has_varargs"]:
            if len(method["arguments"]) > 0:
                method_signature += ", "
                method_arguments += ", "
            vararg_templates += "\ttemplate <class... Args> " + method_signature + "Args... args){\n\t\treturn " + method_name + "(" + method_arguments + "Array::make(args...));\n\t}\n"""
            method_signature += "const Array& __var_args = Array()"

        method_signature += ")" + (" const" if method["is_const"] else "")


        source.append("\t" + method_signature + ";")

    source.append(vararg_templates)

    if use_template_get_node and class_name == "Node":
        # Extra definition for template get_node that calls the renamed get_node_internal; has a default template parameter for backwards compatibility.
        source.append("\ttemplate <class T = Node>")
        source.append("\tT *get_node(const NodePath path) const {")
        source.append("\t\treturn Object::cast_to<T>(get_node_internal(path));")
        source.append("\t}")

        source.append("};")
        source.append("")

        # ...And a specialized version so we don't unnecessarily cast when using the default.
        source.append("template <>")
        source.append("inline Node *Node::get_node<Node>(const NodePath path) const {")
        source.append("\treturn get_node_internal(path);")
        source.append("}")
        source.append("")
    else:
        source.append("};")
        source.append("")

    source.append("}")
    source.append("")

    source.append("#endif")


    return "\n".join(source)


def generate_core_constants_header(used_classes, c, use_template_get_node):
    class_name = "CoreConstants"

    source = []
    source.append("#ifndef GODOT_CPP_" + strip_name(c["name"]).upper() + "_HPP")
    source.append("#define GODOT_CPP_" + strip_name(c["name"]).upper() + "_HPP")
    source.append("")
    source.append("")
    source.append("namespace godot {")
    source.append("")

    source.append("namespace CoreConstants {")

    enum_values = []

    source.append("\n\t// Enums.")
    for enum in c["enums"]:
        if "." in enum["name"]:
            # Exception for some core enums like `Variant.Type` and `Variant.Operator`.
            continue
        source.append("\tenum " + strip_name(enum["name"]) + " {")
        for value in enum["values"]:
            source.append("\t\t" + remove_nested_type_prefix(value) + " = " + str(enum["values"][value]) + ",")
            enum_values.append(value)
        source.append("\t};")

    source.append("\n\t// Constants.")

    for name in c["constants"]:
        if name not in enum_values:
            source.append("\tconst static int " + name + " = " + str(c["constants"][name]) + ";")

    source.append("}")
    source.append("}")
    source.append("")

    source.append("#endif")

    return "\n".join(source)


def generate_class_implementation(icalls, used_classes, c, use_template_get_node):
    class_name = strip_name(c["name"])

    ref_allowed = class_name != "Object" and class_name != "Reference"

    source = []
    source.append("#include \"" + class_name + ".hpp\"")
    source.append("")
    source.append("")

    source.append("#include <core/GodotGlobal.hpp>")
    source.append("#include <core/CoreTypes.hpp>")
    source.append("#include <core/Ref.hpp>")

    source.append("#include <core/Godot.hpp>")
    source.append("")


    source.append("#include \"__icalls.hpp\"")
    source.append("")
    source.append("")

    for used_class in used_classes:
        if is_enum(used_class):
            continue
        else:
            source.append("#include \"" + strip_name(used_class) + ".hpp\"")

    source.append("")
    source.append("")

    source.append("namespace godot {")


    core_object_name = "this"


    source.append("")
    source.append("")

    if c["singleton"]:
        source.append("" + class_name + " *" + class_name + "::_singleton = NULL;")
        source.append("")
        source.append("")

        # FIXME Test if inlining has a huge impact on binary size
        source.append(class_name + "::" + class_name + "() {")
        source.append("\t_owner = godot::api->godot_global_get_singleton((char *) \"" + strip_name(c["name"]) + "\");")
        source.append("}")

        source.append("")
        source.append("")

    # Method table initialization
    source.append(class_name + "::___method_bindings " + class_name + "::___mb = {};")
    source.append("")

    source.append("void *" + class_name + "::_detail_class_tag = nullptr;")
    source.append("")

    source.append("void " + class_name + "::___init_method_bindings() {")

    for method in c["methods"]:
        source.append("\t___mb.mb_" + method["name"] + " = godot::api->godot_method_bind_get_method(\"" + c["name"] + "\", \"" + ("get_node" if use_template_get_node and method["name"] == "get_node_internal" else method["name"]) + "\");")

    source.append("\tgodot_string_name class_name;")
    source.append("\tgodot::api->godot_string_name_new_with_latin1_chars(&class_name, \"" + c["name"] + "\");")
    source.append("\t_detail_class_tag = godot::api->godot_get_class_tag(&class_name);")
    source.append("\tgodot::api->godot_string_name_destroy(&class_name);")

    source.append("}")
    source.append("")

    if c["instantiable"]:
        source.append(class_name + " *" + strip_name(c["name"]) + "::_new()")
        source.append("{")
        source.append("\treturn (" + class_name + " *) godot::nativescript_api->godot_nativescript_get_instance_binding_data(godot::_RegisterState::language_index, godot::api->godot_get_class_constructor((char *)\"" + c["name"] + "\")());")
        source.append("}")

    for method in c["methods"]:
        method_signature = ""

        method_signature += make_gdnative_type(method["return_type"], ref_allowed, True)
        method_signature += strip_name(c["name"]) + "::" + escape_cpp(method["name"]) + "("

        for i, argument in enumerate(method["arguments"]):
            method_signature += "const " + make_gdnative_type(argument["type"], ref_allowed, False)
            method_signature += escape_cpp(argument["name"])

            if i != len(method["arguments"]) - 1:
                method_signature += ", "

        if method["has_varargs"]:
            if len(method["arguments"]) > 0:
                method_signature += ", "
            method_signature += "const Array& __var_args"

        method_signature += ")" + (" const" if method["is_const"] else "")

        source.append(method_signature + " {")


        if method["name"] == "free":
            # dirty hack because Object::free is marked virtual but doesn't actually exist...
            source.append("\tgodot::api->godot_object_destroy(_owner);")
            source.append("}")
            source.append("")
            continue

        return_statement = ""
        return_type_is_ref = is_reference_type(method["return_type"]) and ref_allowed

        if method["return_type"] != "void":
            if is_class_type(method["return_type"]):
                if is_enum(method["return_type"]):
                    if method["return_type"] == "enum.Vector3::Axis":
                        # Exception since enums in builtin types are not supported.
                        # Can be removed when fixed in Godot.
                        return_statement += "return (int) "
                    else:
                        if is_nested_type(method["return_type"]):
                            return_statement += "return (" + remove_enum_prefix(method["return_type"]) + ") "
                        else:
                            return_statement += "return (CoreConstants::" + remove_enum_prefix(method["return_type"]) + ") "

                elif return_type_is_ref:
                    return_statement += "return Ref<" + strip_name(method["return_type"]) + ">::__internal_constructor("
                else:
                    return_statement += "return " + ("(" + strip_name(method["return_type"]) + " *) " if is_class_type(method["return_type"]) else "")
            else:
                return_statement += "return "

        def get_icall_type_name(name):
            if is_enum(name):
                return "int"
            if is_class_type(name):
                return "Object"
            return name



        if method["has_varargs"]:

            if len(method["arguments"]) != 0:
                source.append("\tVariant __given_args[" + str(len(method["arguments"])) + "];")

            for i, argument in enumerate(method["arguments"]):
                source.append("\tgodot::api->godot_variant_new_nil((godot_variant *) &__given_args[" + str(i) + "]);")

            source.append("")


            for i, argument in enumerate(method["arguments"]):
                source.append("\t__given_args[" + str(i) + "] = " + escape_cpp(argument["name"]) + ";")

            source.append("")

            size = ""
            if method["has_varargs"]:
                size = "(__var_args.size() + " + str(len(method["arguments"])) + ")"
            else:
                size = "(" + str(len(method["arguments"])) + ")"

            source.append("\tgodot_variant **__args = (godot_variant **) alloca(sizeof(godot_variant *) * " + size + ");")

            source.append("")

            for i, argument in enumerate(method["arguments"]):
                source.append("\t__args[" + str(i) + "] = (godot_variant *) &__given_args[" + str(i) + "];")

            source.append("")

            if method["has_varargs"]:
                source.append("\tfor (int i = 0; i < __var_args.size(); i++) {")
                source.append("\t\t__args[i + " + str(len(method["arguments"])) + "] = (godot_variant *) &((Array &) __var_args)[i];")
                source.append("\t}")

            source.append("")

            source.append("\tVariant __result;")
            source.append("\t*(godot_variant *) &__result = godot::api->godot_method_bind_call(___mb.mb_" + method["name"] + ", ((const Object *) " + core_object_name + ")->_owner, (const godot_variant **) __args, " + size + ", nullptr);")

            source.append("")

            if is_class_type(method["return_type"]):
                source.append("\tObject *obj = Object::___get_from_variant(__result);")
                source.append("\tif (obj->has_method(\"reference\"))")
                source.append("\t\tobj->callv(\"reference\", Array());")

                source.append("")


            for i, argument in enumerate(method["arguments"]):
                source.append("\tgodot::api->godot_variant_destroy((godot_variant *) &__given_args[" + str(i) + "]);")

            source.append("")

            if method["return_type"] != "void":
                cast = ""
                if is_class_type(method["return_type"]):
                    if return_type_is_ref:
                        cast += "Ref<" + strip_name(method["return_type"]) + ">::__internal_constructor(__result);"
                    else:
                        cast += "(" + strip_name(method["return_type"]) + " *) " + strip_name(method["return_type"] + "::___get_from_variant(") + "__result);"
                else:
                    cast += "__result;"
                source.append("\treturn " + cast)


        else:

            args = []
            for arg in method["arguments"]:
                args.append(get_icall_type_name(arg["type"]))

            icall_ret_type = get_icall_type_name(method["return_type"])

            icall_sig = tuple((icall_ret_type, tuple(args)))

            icalls.add(icall_sig)

            icall_name = get_icall_name(icall_sig)

            return_statement += icall_name + "(___mb.mb_" + method["name"] + ", (const Object *) " + core_object_name

            for arg in method["arguments"]:
                arg_is_ref = is_reference_type(arg["type"]) and ref_allowed
                return_statement += ", " + escape_cpp(arg["name"]) + (".ptr()" if arg_is_ref else "")

            return_statement += ")"

            if return_type_is_ref:
                return_statement += ")"

            source.append("\t" + return_statement + ";")

        source.append("}")
        source.append("")

    source.append("}")

    return "\n".join(source)


def generate_core_constants_implementation(icalls, used_classes, c, use_template_get_node):
    # Nothing to here since it has only constants.

    source = []
    source.append('#include "CoreConstants.hpp"')
    source.append("")

    return "\n".join(source)

def generate_icall_header(icalls):

    source = []
    source.append("#ifndef GODOT_CPP__ICALLS_HPP")
    source.append("#define GODOT_CPP__ICALLS_HPP")

    source.append("")

    source.append("#include <gdnative_api_struct.gen.h>")
    source.append("#include <stdint.h>")
    source.append("")

    source.append("#include <core/GodotGlobal.hpp>")
    source.append("#include <core/CoreTypes.hpp>")
    source.append("#include \"Object.hpp\"")
    source.append("")
    source.append("")

    source.append("namespace godot {")
    source.append("")

    for icall in icalls:
        ret_type = icall[0]
        args = icall[1]

        method_signature = "static inline "

        method_signature += get_icall_return_type(ret_type) + get_icall_name(icall) + "(godot_method_bind *mb, const Object *inst"

        for i, arg in enumerate(args):
            method_signature += ", const "

            if is_core_type(arg):
                method_signature += arg + "&"
            elif arg == "int":
                method_signature += "int64_t "
            elif arg == "float":
                method_signature += "double "
            elif is_primitive(arg):
                method_signature += arg + " "
            else:
                method_signature += "Object *"

            method_signature += "arg" + str(i)

        method_signature += ")"

        source.append(method_signature + " {")

        if ret_type != "void":
            source.append("\t" + ("godot_object *" if is_class_type(ret_type) else get_icall_return_type(ret_type)) + "ret;")
            if is_class_type(ret_type):
                source.append("\tret = nullptr;")


        source.append("\tconst void *args[" + ("1" if len(args) == 0 else "") + "] = {")

        for i, arg in enumerate(args):

            wrapped_argument = "\t\t"
            if is_primitive(arg) or is_core_type(arg):
                wrapped_argument += "(void *) &arg" + str(i)
            else:
                wrapped_argument += "(void *) (arg" + str(i) + ") ? arg" + str(i) + "->_owner : nullptr"

            wrapped_argument += ","
            source.append(wrapped_argument)

        source.append("\t};")
        source.append("")

        source.append("\tgodot::api->godot_method_bind_ptrcall(mb, inst->_owner, args, " + ("nullptr" if ret_type == "void" else "&ret") + ");")

        if ret_type != "void":
            if is_class_type(ret_type):
                source.append("\tif (ret) {")
                source.append("\t\treturn (Object *) godot::nativescript_api->godot_nativescript_get_instance_binding_data(godot::_RegisterState::language_index, ret);")
                source.append("\t}")
                source.append("")
                source.append("\treturn (Object *) ret;")
            else:
                source.append("\treturn ret;")

        source.append("}")
        source.append("")

    source.append("}")
    source.append("")

    source.append("#endif")

    return "\n".join(source)


def generate_builtin_type_header(tp, used_types, has_varargs, complete_types):
    source = []

    source.append(f"#ifndef GODOT_CPP_BUILTIN_{camel_to_snake(strip_name(tp['name'])).upper()}_HPP")
    source.append(f"#define GODOT_CPP_BUILTIN_{camel_to_snake(strip_name(tp['name'])).upper()}_HPP")
    source.append("")

    type_name = strip_name(tp["name"])
    gdnative_type_name = builtin_type_to_gdnative(type_name)

    source.append("#include <gdnative_api_struct.gen.h>")

    if type_name == "Array" or type_name == "Dictionary":
        source.append("")
        source.append("#include <template/Helpers.hpp>")

    if has_varargs:
        source.append("")
        source.append("#include <Array.hpp>")

    if len(complete_types) > 0:
        source.append("")

    for used_type in complete_types:
        if used_type == type_name:
            continue
        if used_type == "Array" and has_varargs:
            continue
        source.append(f"#include <{used_type}.hpp>")

    source.append("")
    source.append("namespace godot {")
    source.append("")
    if type_name == "String":
        # Also include stringifiable types.
        for stringifiable_type in get_stringifiable_types():
            if not is_primitive(stringifiable_type) and get_godot_type(stringifiable_type) not in used_types:
                used_types.append(get_godot_type(stringifiable_type))
    for used_type in used_types:
        if used_type == type_name:
            continue
        if used_type == "Array" and has_varargs:
            continue
        source.append(f"class {used_type};")
    source.append("")
    source.append(f"class {type_name} {{")
    source.append(f"\t{gdnative_type_name} _{gdnative_type_name};")
    source.append("")
    source.append("\tfriend class Variant;")
    source.append("")

    source.append("\tstatic struct ___bindings_struct {")
    for index in range(len(tp["constructors"])):
        source.append(f"\t\tgodot_ptr_constructor constructor{index};")
    for member in tp["members"]:
        source.append(f"\t\tgodot_ptr_getter get_{member['name']};")
        source.append(f"\t\tgodot_ptr_setter set_{member['name']};")
    for method in tp["methods"]:
        source.append(f"\t\tgodot_ptr_builtin_method method_{escape_cpp(method['name'])};")
    for operator in tp["operators"]:
        if is_operator_unary(operator["operator"]):
            source.append(f"\t\tgodot_ptr_operator_evaluator op_{get_operator_id_name(operator['operator'])};")
        else:
            source.append(f"\t\tgodot_ptr_operator_evaluator op_{get_operator_id_name(operator['operator'])}_{operator['other_type']};")
    if tp["is_keyed"]:
        source.append("\t\tgodot_ptr_keyed_getter keyed_getter;")
        source.append("\t\tgodot_ptr_keyed_setter keyed_setter;")
        source.append("\t\tgodot_ptr_keyed_checker keyed_checker;")
    elif tp["has_indexing"]:
        source.append("\t\tgodot_ptr_indexed_getter indexed_getter;")
        source.append("\t\tgodot_ptr_indexed_setter indexed_setter;")

    source.append("\t} ___bindings;")
    source.append("")
    source.append("public:")

    # Default hidden constructor: from GDNative type to GodotCPP type.
    source.append(f"\tinline explicit {type_name}(const {gdnative_type_name} contents) :")
    source.append(f"\t\t_{gdnative_type_name}(contents) {{}}")

    source.append("")
    source.append("\tstatic void ___init_bindings();")

    # Special helper to easily make arrays and dictionaries from varargs.
    if type_name == "Array" or type_name == "Dictionary":
        is_array = type_name == "Array"
        source.append("")
        source.append("\ttemplate<class... Args>")
        source.append(f"\tstatic {type_name} make(Args... args) {{")
        source.append(f"\t\treturn helpers::{'append_all' if is_array else 'add_all'}({type_name}(), args...);")
        source.append("\t}")

    source.append("")
    source.append("\t// Constructors.")
    for constructor in tp["constructors"]:
        first = True
        arguments = []
        for arg in constructor["arguments"]:
            if not first:
                arguments.append(", ")
            first = False
            arguments.append(get_argument_declaration(arg, with_default=True))

        source.append(f"\t{constructor['name']}({''.join(arguments)});")
    if type_name == "String" or type_name == "StringName":
        # Special case: make a constructor from const char *.
        source.append(f"\t{type_name}(const char *from);")
    if type_name == "String":
        # Special case: make a constructor from const wchar_t * and variants.
        source.append(f"\t{type_name}(const char16_t *from);")
        source.append(f"\t{type_name}(const char32_t *from);")
        source.append(f"\t{type_name}(const wchar_t *from);")
        # Also constructor from other types.
        for stringifiable in get_stringifiable_types():
            if is_primitive(stringifiable):
                source.append(f"\t{type_name}({get_godot_type(stringifiable)} from);")
            else:
                source.append(f"\t{type_name}(const {get_godot_type(stringifiable)} &from);")

    if need_destructor(type_name):
        source.append("")
        source.append("\t// Destructor.")
        source.append(f"\t~{type_name}();")

    source.append("")
    source.append("\t// Constants.")
    for constant in tp["constants"]:
        source.append(f"\tstatic {get_godot_type(constant['type'])} {escape_cpp(constant['name'])};")

    source.append("")
    source.append("\t// Members.")
    for member in tp["members"]:
        if not has_method(tp, f"get_{member['name']}"):
            source.append(f"\t{get_godot_type(member['type'])} get_{member['name']}() const;")
        if not has_method(tp, f"set_{member['name']}"):
            source.append(f"\tvoid set_{member['name']}({get_argument_declaration(member, with_default=True)});")

    source.append("")
    source.append("\t// Methods.")
    for method in tp["methods"]:
        method_name = escape_cpp(method["name"])
        if not method["has_varargs"]:
            signature = "\t"
            if method["is_static"]:
                signature += f"static "
            signature += f"{get_godot_type(method['return_type'])} {escape_cpp(method_name)}("
            first = True
            for arg in method["arguments"]:
                if not first:
                    signature += ", "
                first = False
                signature += get_argument_declaration(arg, with_default=True)

            signature += f"){' const' if method['is_const'] else ''};"
            source.append(signature)
        else: # Varargs.
            # Make a function that takes an array.
            signature = "\t"
            if method["is_static"]:
                signature = f"static "
            signature = f"{get_godot_type(method['return_type'])} {escape_cpp(method_name)}(const Array &arguments"
            signature += f"){' const' if method['is_const'] else ''};"
            source.append(signature)
            # Make a variadic template version:
            source.append("\ttemplate<class... Args>")
            source.append(f"\tinline {'static ' if method['is_static'] else ''} {get_godot_type(method['return_type'])} {'' if is_primitive(method['return_type']) else '&'}{escape_cpp(method_name)}(Args... arguments) {{")
            if method["return_type"] == "void":
                source.append(f"\t\t{method_name}(Array::make(arguments...));")
            else:
                source.append(f"\t\treturn {method_name}(Array::make(arguments...));")
            source.append("\t}")
            source.append("")


    source.append("")
    source.append("\t// Operators.")
    if type_name == "String":
        # Special case: make an operator to const char * and variants.
        source.append("\toperator const char *() const;")
        source.append("\toperator const char16_t *() const;")
        source.append("\toperator const char32_t *() const;")
        source.append("\toperator const wchar_t *() const;")

    # Add assignment operator.
    source.append(f"\t{type_name} &operator =(const {type_name} &other);")

    for operator in tp["operators"]:
        signature = "\t"
        if operator['name'] == 'in':
            # Not a C++ operator so just create a function.
            signature += f"{get_godot_type(operator['return_type'])} operator_in("
        else:
            operator_name = get_operator_cpp_name(operator['operator'])
            signature += f"{get_godot_type(operator['return_type'])} operator {operator_name}("

        if not is_operator_unary(operator["operator"]):
            signature += get_argument_declaration({"type": operator["other_type"], "name": "other", "has_default_value": False}, with_default=False)
        signature += ") const;"
        source.append(signature)

    if tp["is_keyed"]:
        source.append("")
        source.append(f"\tconst Variant &operator[](const Variant &key) const;")
        source.append(f"\tVariant &operator[](const Variant &key);")

    elif tp["has_indexing"] or type_name == "String":
        indexed_type = get_godot_type(tp["indexed_type"]) if type_name != "String" else "char32_t"
        source.append(f"\tconst {indexed_type} &operator[](godot_int index) const;")
        source.append(f"\t{indexed_type} &operator[](godot_int index);")

    source.append("")
    source.append("};")
    source.append("}")

    source.append("#endif")

    return "\n".join(source)

def generate_builtin_type_implementation(tp, used_types, has_varargs, complete_types):
    source = []

    type_name = tp['name']
    gdnative_type_name = builtin_type_to_gdnative(type_name)
    type_enum = builtin_type_to_gdnative_enum(type_name)

    source.append(f"#include \"{type_name}.hpp\"")
    source.append("")
    source.append(f"#include <core/GodotGlobal.hpp>")
    for used_type in used_types:
        if used_type == type_name:
            continue
        source.append(f"#include \"{used_type}.hpp\"")
    if type_name == "String":
        source.append("")
        source.append(f"#include <Utilities.hpp>") # For string conversion.
    source.append("")
    source.append(f"#include <cstring>")
    source.append("")
    source.append("namespace godot {")

    source.append("")
    source.append(f"{type_name}::___bindings_struct {type_name}::___bindings = {{}};")
    source.append("")
    source.append(f"void {type_name}::___init_bindings() {{")
    for index, constructor in enumerate(tp['constructors']):
        source.append(f"\t___bindings.constructor{index} = godot::api->godot_variant_get_ptr_constructor({type_enum}, {index});")
    for member in tp["members"]:
        source.append(f"\t___bindings.get_{member['name']} = godot::api->godot_variant_get_ptr_getter_with_cstring({type_enum}, \"{member['name']}\");")
        source.append(f"\t___bindings.set_{member['name']} = godot::api->godot_variant_get_ptr_setter_with_cstring({type_enum}, \"{member['name']}\");")
    for method in tp["methods"]:
        source.append(f"\t___bindings.method_{escape_cpp(method['name'])} = godot::api->godot_variant_get_ptr_builtin_method_with_cstring({type_enum}, \"{method['name']}\");")
    for operator in tp["operators"]:
        if is_operator_unary(operator['operator']):
            source.append(f"\t___bindings.op_{get_operator_id_name(operator['operator'])} = godot::api->godot_variant_get_ptr_operator_evaluator((godot_variant_operator){operator['operator']}, {builtin_type_to_gdnative_enum(type_name)}, {builtin_type_to_gdnative_enum(operator['other_type'])});")
        else:
            source.append(f"\t___bindings.op_{get_operator_id_name(operator['operator'])}_{operator['other_type']} = godot::api->godot_variant_get_ptr_operator_evaluator((godot_variant_operator){operator['operator']}, {builtin_type_to_gdnative_enum(type_name)}, {builtin_type_to_gdnative_enum(operator['other_type'])});")
    if tp["is_keyed"]:
        source.append(f"\t___bindings.keyed_getter = godot::api->godot_variant_get_ptr_keyed_getter({type_enum});")
        source.append(f"\t___bindings.keyed_setter = godot::api->godot_variant_get_ptr_keyed_setter({type_enum});")
    elif tp["has_indexing"]:
        source.append(f"\t___bindings.indexed_getter = godot::api->godot_variant_get_ptr_indexed_getter({type_enum});")
        source.append(f"\t___bindings.indexed_setter = godot::api->godot_variant_get_ptr_indexed_setter({type_enum});")
    if len(tp["constants"]) > 0:
        source.append(f"\tgodot_variant constant;")
        for constant in tp["constants"]:
            source.append(f"\tconstant = godot::api->godot_variant_get_constant_value_with_cstring({type_enum}, \"{constant['name']}\");")
            source.append(f"\t*(({builtin_type_to_gdnative(constant['type'])} *)&{constant['name']}) = godot::api->godot_variant_as_{camel_to_snake(constant['type'])}(&constant);")
            source.append(f"\tgodot::api->godot_variant_destroy(&constant);")
    source.append("}")

    source.append("")
    for constant in tp["constants"]:
        default_value = ""
        if is_primitive(constant["type"]):
            if constant["type"] == "int":
                default_value = "0"
            elif constant["type"] == "float":
                default_value = "0.0"
            elif constant["type"] == "bool":
                default_value = "false"
        else:
            default_value = f"{get_godot_type(constant['type'])}({builtin_type_to_gdnative(constant['type'])}())"


        source.append(f"{get_godot_type(constant['type'])} {type_name}::{escape_cpp(constant['name'])} = {default_value};")
    source.append("")


    for index, constructor in enumerate(tp["constructors"]):
        source.append("")
        first = True
        arguments = []
        for arg in constructor["arguments"]:
            if not first:
                arguments.append(", ")
            first = False
            arguments.append(get_argument_declaration(arg, with_default=False))

        source.append(f"{type_name}::{constructor['name']}({''.join(arguments)}) {{")
        if len(arguments) == 0:
                source.append(f"\t___bindings.constructor{index}(&_{gdnative_type_name}, nullptr);")
                # source.append(f"\tmemset(&_{gdnative_type_name}, 0, sizeof({gdnative_type_name}));")
        else:
            source.append("\tconst void *args [] = {")
            for arg in constructor["arguments"]:
                source.append(f"\t\t(void *)&{escape_cpp(arg['name'])},")
            source.append("\t};")
            source.append(f"\t___bindings.constructor{index}(&_{gdnative_type_name}, args);")

        source.append("}")

    if type_name == "String" or type_name == "StringName":
        # Special case: make a constructor from const char *.
        source.append("")
        source.append(f"{type_name}::{type_name}(const char *from) {{")
        source.append(f"\tgodot::api->{gdnative_type_name}_new_with_latin1_chars(&_{gdnative_type_name}, from);")
        source.append("}")
    if type_name == "String":
        # Special case: make a constructor from const wchar_t * and variants.
        source.append("")
        source.append(f"{type_name}::{type_name}(const char16_t *from) {{")
        source.append(f"\tgodot::api->{gdnative_type_name}_new_with_utf16_chars(&_{gdnative_type_name}, from);")
        source.append("}")
        source.append("")
        source.append(f"{type_name}::{type_name}(const char32_t *from) {{")
        source.append(f"\tgodot::api->{gdnative_type_name}_new_with_utf32_chars(&_{gdnative_type_name}, from);")
        source.append("}")
        source.append("")
        source.append(f"{type_name}::{type_name}(const wchar_t *from) {{")
        source.append(f"\tgodot::api->{gdnative_type_name}_new_with_wide_chars(&_{gdnative_type_name}, from);")
        source.append("}")

        # Also constructor from other types, for convenience.
        for stringifiable in get_stringifiable_types():
            if is_primitive(stringifiable):
                source.append(f"{type_name}::{type_name}({get_godot_type(stringifiable)} from) {{")
            else:
                source.append(f"{type_name}::{type_name}(const {get_godot_type(stringifiable)} &from) {{")
            source.append("\t*this = Utilities::str(from);")
            source.append("}")

    if need_destructor(type_name):
        source.append("")
        source.append(f"{type_name}::~{type_name}() {{")
        source.append(f"\tgodot::api->{gdnative_type_name}_destroy(&_{gdnative_type_name});")
        source.append("}")

    for member in tp["members"]:
        # Avoid cases where getter/setter are define as functions (like Transform2D).
        if not has_method(tp, f"get_{member['name']}"):
            source.append("")
            source.append(f"{get_godot_type(member['type'])} {type_name}::get_{member['name']}() const {{")
            source.append(f"\t{builtin_type_to_gdnative(member['type'])} result;")
            source.append(f"\t___bindings.get_{member['name']}((const void *)&_{gdnative_type_name}, (void *)&result);")
            if is_primitive(member["type"]):
                source.append(f"\treturn result;")
            elif member["type"] == "Object":
                source.append(f"\treturn ({member['type']} *)result;")
            else:
                source.append(f"\treturn {member['type']}(result);")
            source.append("}")

        if not has_method(tp, f"set_{member['name']}"):
            source.append("")
            source.append(f"void {type_name}::set_{member['name']}({get_argument_declaration(member, with_default=False)}) {{")
            source.append(f"\t___bindings.set_{member['name']}((void *)&_{gdnative_type_name}, (const void *)&{member['name']});")
            source.append("}")

    for method in tp["methods"]:
        source.append("")
        method_name = escape_cpp(method["name"])
        if not method["has_varargs"]:
            signature = f"{get_godot_type(method['return_type'])} {type_name}::{method_name}("
            first = True
            for arg in method["arguments"]:
                if not first:
                    signature += ", "
                first = False
                signature += get_argument_declaration(arg, with_default=False)

            signature += f"){' const' if method['is_const'] else ''} {{"
            source.append(signature)

            has_return = method["return_type"] != "void"

            if has_return:
                source.append(f"\t{get_godot_type(method['return_type'])} result;")

            this_pointer = f"(void *)&_{gdnative_type_name}"
            if method["is_static"]:
                this_pointer = f"nullptr"

            if len(method['arguments']) != 0:
                source.append("\tconst void *args [] = {")
                for arg in method["arguments"]:
                    source.append(f"\t\t(void *)&{escape_cpp(arg['name'])},")
                source.append("\t};")

                source.append(f"\t___bindings.method_{method_name}({this_pointer}, args, {'&result' if has_return else 'nullptr'}, {len(method['arguments'])});")
            else:
                source.append(f"\t___bindings.method_{method_name}({this_pointer}, nullptr, {'&result' if has_return else 'nullptr'}, {len(method['arguments'])});")

            if has_return:
                # if is_primitive(method["return_type"]):
                #     source.append(f"\treturn result;")
                # # elif method["return_type"] == "Object":
                # #     source.append(f"\treturn ({method['return_type']} *)result;")
                # else:
                    source.append(f"\treturn result;")

            source.append("}")

        else: # Varargs.
            signature = f"{get_godot_type(method['return_type'])} {type_name}::{method_name}(const Array &arguments"
            signature += f"){' const' if method['is_const'] else ''} {{"
            source.append(signature)

            source.append("\tgodot_variant **__args = (godot_variant **)alloca(sizeof(godot_variant *) * arguments.size());")
            source.append("\tfor (int i = 0; i < arguments.size(); i++) {")
            source.append("\t\t__args[i] = (godot_variant *)&arguments[i];")
            source.append("\t}")
            source.append("")
            if method["return_type"] == "void":
                source.append(f"\t___bindings.method_{method_name}((void *)&_{gdnative_type_name}, (const void **)__args, nullptr, arguments.size());")
            else:
                source.append(f"\t{builtin_type_to_gdnative(method['return_type'])} result;")
                source.append(f"\t___bindings.method_{method_name}((void *)&_{gdnative_type_name}, (const void **)__args, &result, arguments.size());")
                if is_primitive(method["return_type"]):
                    source.append(f"\treturn result;")
                elif method["return_type"] == "Object":
                    source.append(f"\treturn ({method['return_type']} *)result;")
                else:
                    source.append(f"\treturn {method['return_type']}(result);")

            source.append("}")

    if type_name == "String":
        # Special case: make an operator from const char * and variants.
        source.append("")
        source.append(f"{type_name}::operator const char *() const {{")
        source.append(f"\treturn godot::api->{gdnative_type_name}_to_latin1_chars(&_{gdnative_type_name});")
        source.append("}")
        source.append("")
        source.append(f"{type_name}::operator const char16_t *() const {{")
        source.append(f"\treturn godot::api->{gdnative_type_name}_to_utf16_chars(&_{gdnative_type_name});")
        source.append("}")
        source.append("")
        source.append(f"{type_name}::operator const char32_t *() const {{")
        source.append(f"\treturn godot::api->{gdnative_type_name}_to_utf32_chars(&_{gdnative_type_name});")
        source.append("}")
        source.append("")
        source.append(f"{type_name}::operator const wchar_t *() const {{")
        source.append(f"\treturn godot::api->{gdnative_type_name}_to_wide_chars(&_{gdnative_type_name});")
        source.append("}")

    # Add assignment operator.
    source.append("")
    source.append(f"{type_name} &{type_name}::operator =(const {type_name} &other) {{")
    if need_destructor(type_name):
        source.append(f"\tgodot::api->{gdnative_type_name}_destroy(&_{gdnative_type_name});")
    source.append(f"\tgodot::api->{gdnative_type_name}_new_copy(&_{gdnative_type_name}, &other._{gdnative_type_name});")
    source.append("\treturn *this;")
    source.append("}")

    for operator in tp["operators"]:
        source.append("")
        signature = ""
        if operator['name'] == 'in':
            # Not a C++ operator so just create a function.
            signature += f"{get_godot_type(operator['return_type'])} {type_name}::operator_in("
        else:
            operator_name = get_operator_cpp_name(operator['operator'])
            signature += f"{get_godot_type(operator['return_type'])} {type_name}::operator {operator_name}("

        is_unary = is_operator_unary(operator["operator"])
        op_name = f"op_{get_operator_id_name(operator['operator'])}"
        if not is_unary:
            signature += get_argument_declaration({"type": operator["other_type"], "name": "other", "has_default_value": False}, with_default=False)
            op_name = f"op_{get_operator_id_name(operator['operator'])}_{operator['other_type']}"

        signature += ") const {"
        source.append(signature)

        source.append(f"\t{builtin_type_to_gdnative(operator['return_type'])} result;")
        source.append(f"\t___bindings.{op_name}((void *)&_{gdnative_type_name}, {'nullptr' if is_unary else '(void *)&other'}, (void *)&result);")
        source.append(f"\treturn {operator['return_type']}(result);")
        source.append("}")

    if tp["is_keyed"]:
        source.append("")
        source.append(f"const Variant &{type_name}::operator[](const Variant &key) const {{")
        source.append(f"\treturn *((Variant *)godot::api->{gdnative_type_name}_operator_index_const(&_{gdnative_type_name}, (const godot_variant *)&key));")
        source.append("}")

        source.append("")
        source.append(f"Variant &{type_name}::operator[](const Variant &key) {{")
        source.append(f"\treturn *((Variant *)godot::api->{gdnative_type_name}_operator_index(&_{gdnative_type_name}, (const godot_variant *)&key));")
        source.append("}")


    elif tp["has_indexing"] or type_name == "String":
        indexed_type = get_godot_type(tp["indexed_type"]) if type_name != "String" else "char32_t"

        source.append("")
        source.append(f"const {indexed_type} &{type_name}::operator[](godot_int index) const {{")
        source.append(f"\treturn *(({indexed_type} *)godot::api->{gdnative_type_name}_operator_index_const(&_{gdnative_type_name}, index));")
        source.append("}")

        source.append("")
        source.append(f"{indexed_type} &{type_name}::operator[](godot_int index) {{")
        source.append(f"\treturn *(({indexed_type} *)godot::api->{gdnative_type_name}_operator_index(&_{gdnative_type_name}, index));")
        source.append("}")

    source.append("")
    source.append("}")

    return "\n".join(source)


def generate_utilities_header(tp, used_types, has_varargs, complete_types):
    source = []

    source.append(f"#ifndef GODOT_CPP_BUILTIN_UTILITIES_HPP")
    source.append(f"#define GODOT_CPP_BUILTIN_UTILITIES_HPP")
    source.append("")

    source.append("#include <gdnative_api_struct.gen.h>")

    if has_varargs:
        source.append("")
        source.append("#include <Array.hpp>")
    source.append("#include <Variant.hpp>")

    if len(complete_types) > 0:
        source.append("")

    for used_type in complete_types:
        if used_type == "Array" and has_varargs:
            continue
        source.append(f"#include <{used_type}.hpp>")

    source.append("")
    source.append("namespace godot {")
    source.append("")
    for used_type in used_types:
        if used_type == "Array" and has_varargs:
            continue
        source.append(f"class {used_type};")

    source.append("")
    source.append("class Utilities {")
    source.append("")
    source.append("\tstatic struct ___bindings_struct {")
    for method in tp["methods"]:
        source.append(f"\t\tgodot_ptr_utility_function {escape_cpp(method['name'])};")

    source.append("\t} ___bindings;")
    source.append("")
    source.append("\tUtilities() {} // Not meant to be instanced.")

    source.append("")
    source.append("public:")
    source.append("\tstatic void ___init_bindings();")
    source.append("")

    for method in tp["methods"]:
        method_name = escape_cpp(method['name'])
        signature = f"\tstatic {get_godot_type(method['return_type'])} {method_name}("
        first = True
        if not method["has_varargs"]:
            for arg in method['arguments']:
                if not first:
                    signature += ", "
                first = False
                signature += get_argument_declaration(arg, with_default=True)
            signature += ");"
            source.append(signature)
        else:
            # Make a function that takes an array.
            signature = f"\tstatic {get_godot_type(method['return_type'])} {method_name}(const Array &arguments);"
            source.append(signature)
            # Make a variadic template version:
            source.append("\ttemplate<class... Args>")
            source.append(f"\tinline static {get_godot_type(method['return_type'])} {method_name}(Args... arguments) {{")
            if method["return_type"] == "void":
                source.append(f"\t\t{method_name}(Array::make(arguments...));")
            else:
                source.append(f"\t\treturn {method_name}(Array::make(arguments...));")
            source.append("\t}")
            source.append("")

    source.append("")
    source.append("};")
    source.append("}")

    source.append("#endif")
    return "\n".join(source)


def generate_utilities_implementation(tp, used_types, has_varargs, complete_types):
    source = []

    source.append(f"#include \"Utilities.hpp\"")
    source.append("")
    source.append(f"#include <core/GodotGlobal.hpp>")
    source.append("")
    for used_type in used_types:
        source.append(f"#include \"{used_type}.hpp\"")

    source.append("namespace godot {")

    source.append("")
    source.append("Utilities::___bindings_struct Utilities::___bindings = {};")
    source.append("")
    source.append("void Utilities::___init_bindings() {")
    for method in tp["methods"]:
        source.append(f"\t___bindings.{escape_cpp(method['name'])} = godot::api->godot_variant_get_ptr_utility_function_with_cstring(\"{method['name']}\");")
    source.append("}")

    for method in tp["methods"]:
        source.append("")
        method_name = escape_cpp(method['name'])
        signature = f"{get_godot_type(method['return_type'])} Utilities::{method_name}("
        first = True
        if not method["has_varargs"]:
            for arg in method["arguments"]:
                if not first:
                    signature += ", "
                first = False
                signature += get_argument_declaration(arg, with_default=True)
            signature += ") {"
            source.append(signature)

            has_return = method["return_type"] != "void"

            if has_return:
                source.append(f"\t{builtin_type_to_gdnative(method['return_type'])} result;")

            if len(method['arguments']) != 0:
                source.append("\tconst void *args [] = {")
                for arg in method["arguments"]:
                    source.append(f"\t\t(void *)&{escape_cpp(arg['name'])},")
                source.append("\t};")

                source.append(f"\t___bindings.{method_name}({'&result' if has_return else 'nullptr'}, args, {len(method['arguments'])});")
            else:
                source.append(f"\t___bindings.{method_name}({'&result' if has_return else 'nullptr'}, nullptr, {len(method['arguments'])});")

            if has_return:
                if is_primitive(method["return_type"]):
                    source.append(f"\treturn result;")
                elif method["return_type"] == "Object":
                    source.append(f"\treturn ({method['return_type']} *)result;")
                else:
                    source.append(f"\treturn {method['return_type']}(result);")

            source.append("}")
        else: # Varargs.
            signature = f"{get_godot_type(method['return_type'])} Utilities::{method_name}(const Array &arguments) {{"
            source.append(signature)

            source.append("\tgodot_variant **__args = (godot_variant **)alloca(sizeof(godot_variant *) * arguments.size());")
            source.append("\tfor (int i = 0; i < arguments.size(); i++) {")
            source.append("\t\t__args[i] = (godot_variant *)&arguments[i];")
            source.append("\t}")
            source.append("")
            if method["return_type"] == "void":
                source.append(f"\t___bindings.{method_name}(nullptr, (const void **)__args, arguments.size());")
            else:
                source.append(f"\t{builtin_type_to_gdnative(method['return_type'])} result;")
                source.append(f"\t___bindings.{method_name}(&result, (const void **)__args, arguments.size());")
                if is_primitive(method["return_type"]):
                    source.append(f"\treturn result;")
                elif method["return_type"] == "Object":
                    source.append(f"\treturn ({method['return_type']} *)result;")
                else:
                    source.append(f"\treturn {method['return_type']}(result);")
            source.append("}")

    source.append("")
    source.append("}")
    return "\n".join(source)


def generate_type_registry(classes):
    source = []

    source.append("#include \"TagDB.hpp\"")
    source.append("#include <typeinfo>")
    source.append("\n")

    for c in classes:
        if c["name"] == "CoreConstants":
            continue
        source.append("#include <" + strip_name(c["name"]) + ".hpp>")

    source.append("")
    source.append("")

    source.append("namespace godot {")

    source.append("void ___register_types()")
    source.append("{")

    for c in classes:
        if c["name"] == "CoreConstants":
            continue
        class_name = strip_name(c["name"])
        base_class_name = strip_name(c["base_class"])

        class_type_hash = "typeid(" + class_name + ").hash_code()"

        base_class_type_hash = "typeid(" + base_class_name + ").hash_code()"

        if base_class_name == "":
            base_class_type_hash = "0"

        source.append("\tgodot::_TagDB::register_global_type(\"" + c["name"] + "\", " + class_type_hash + ", " + base_class_type_hash + ");")

    source.append("}")

    source.append("")
    source.append("}")


    return "\n".join(source)


def generate_init_method_bindings(classes):
    source = []

    for c in classes:
        source.append("#include <" + strip_name(c["name"]) + ".hpp>")

    source.append("")
    source.append("")

    source.append("namespace godot {")

    source.append("void ___init_method_bindings() {")

    for c in classes:
        if c["name"] == "CoreConstants":
            continue
        source.append("\t" + strip_name(c["name"]) + "::___init_method_bindings();")

    source.append("}")

    source.append("")
    source.append("}")

    return "\n".join(source)


def generate_init_core_bindings(builtin_types):
    source = []

    for tp in builtin_types:
        if not is_core_type(tp["name"]) and not tp["name"] == "Utilities":
            continue
        if is_inlined_type(tp["name"]):
            # Not generated, so there's no bindings.
            continue
        source.append("#include <" + strip_name(tp["name"]) + ".hpp>")

    source.append("")
    source.append("")

    source.append("namespace godot {")

    source.append("void ___init_core_bindings() {")

    for tp in builtin_types:
        if not is_core_type(tp["name"]) and not tp["name"] == "Utilities":
            continue
        if is_inlined_type(tp["name"]):
            # Not generated, so there's no bindings.
            continue
        source.append("\t" + strip_name(tp["name"]) + "::___init_bindings();")

    source.append("}")

    source.append("")
    source.append("}")

    return "\n".join(source)


def get_icall_return_type(t):
    if is_class_type(t):
        return "Object *"
    if t == "int":
        return "int64_t "
    if t == "float" or t == "real":
        return "double "
    return t + " "


def get_icall_name(sig):
    ret_type = sig[0]
    args = sig[1]

    name = "___godot_icall_"
    name += strip_name(ret_type)
    for arg in args:
        name += "_" + strip_name(arg)

    return name


def get_godot_type(tp):
    if tp == "int":
        return "int64_t"
    if tp == "float" or tp == "real":
        return "double"
    if tp == "Nil":
        return "Variant"
    if tp == "Object":
        return "Object *"
    return tp


def get_used_classes(c):
    classes = []
    for method in c["methods"]:
        if is_class_type(method["return_type"]) and not (method["return_type"] in classes):
            classes.append(method["return_type"])

        for arg in method["arguments"]:
            if is_class_type(arg["type"]) and not (arg["type"] in classes):
                classes.append(arg["type"])

    return classes


def get_used_types(tp):
    has_varargs = False
    types = []
    complete_types = [] # Types in arguments with a default value (doesn't allow incomplete type).

    for constructor in tp["constructors"]:
        if not is_primitive(constructor["return_type"]) and not (constructor["return_type"] in types) and not (constructor["return_type"] == "Nil"):
            types.append(constructor["return_type"])

        for arg in constructor["arguments"]:
            if not is_primitive(arg["type"]) and not (arg["type"] in types) and not (arg["type"] == "Nil"):
                types.append(arg["type"])

    for method in tp["methods"]:
        if method["has_varargs"]:
            has_varargs = True
        if not is_primitive(method["return_type"]) and not (method["return_type"] == "Nil"):
            # If this is varargs then we can't use the incomplete type.
            if method["has_varargs"]:
                if method["return_type"] not in complete_types:
                    complete_types.append(method["return_type"])
            elif method["return_type"] not in types:
                types.append(method["return_type"])

        for arg in method["arguments"]:
            if not is_primitive(arg["type"]) and not (arg["type"] == "Nil"):
                if arg["has_default_value"]:
                    if arg["type"] not in complete_types:
                        complete_types.append(arg["type"])
                elif arg["type"] not in types:
                    types.append(arg["type"])

    for member in tp["members"]:
        if not is_primitive(member["type"]) and not (member["type"] in types) and not (member["type"] == "Nil"):
            types.append(member["type"])

    for constant in tp["constants"]:
        if not is_primitive(constant["type"]) and not (constant["type"] in types) and not (constant["type"] == "Nil"):
            types.append(constant["type"])

    for operator in tp["operators"]:
        if not is_primitive(operator["other_type"]) and not (operator["other_type"] in types) and not (operator["other_type"] == "Nil"):
            types.append(operator["other_type"])

    # Deduplicate includes.
    for type_name in complete_types:
        if type_name in types:
            types.remove(type_name)

    types.sort()
    return (types, has_varargs, complete_types)


def has_method(tp, method_name):
    for method in tp["methods"]:
        if method["name"] == method_name:
            return True
    return False


def has_member(tp, member_name):
    for member in tp["members"]:
        if member["name"] == member_name:
            return True
    return False


def get_argument_declaration(argument, with_default):
    arg = ""
    if is_primitive(argument["type"]):
        arg += f"{get_godot_type(argument['type'])} "
    else:
        arg += f"const {get_godot_type(argument['type'])} &"

    arg += f"{escape_cpp(argument['name'])}"

    if with_default and argument['has_default_value']:
        arg += f" = {parse_default_value(argument['type'], argument['default_value'])}"

    return arg


def is_operator_unary(operator):
    # This is in sync with the operator enum.
    # If changed in Godot it needs to be changed here.
    operator_map = [
        False, ## equal
        False, ## not_equal
        False, ## less
        False, ## less_or_equal
        False, ## greater
        False, ## greater_or_equal
        False, ## plus
        False, ## minus
        False, ## times
        False, ## divided
        True, ## negate
        True, ## positive
        False, ## modulo
        False, ## shift_left
        False, ## shift_right
        False, ## bit_and
        False, ## bit_or
        False, ## bit_xor
        True, ## bit_negate
        False, ## and
        False, ## or
        False, ## xor
        False, ## not
        False, ## in
    ]
    return operator_map[operator]


def get_operator_id_name(operator):
    # This is in sync with the operator enum.
    # If changed in Godot it needs to be changed here.
    operator_map = [
        "equal",
        "not_equal",
        "less",
        "less_or_equal",
        "greater",
        "greater_or_equal",
        "plus",
        "minus",
        "times",
        "divided",
        "negate",
        "positive",
        "modulo",
        "shift_left",
        "shift_right",
        "bit_and",
        "bit_or",
        "bit_xor",
        "bit_negate",
        "and",
        "or",
        "xor",
        "not",
        "in",
    ]
    return operator_map[operator]


def get_operator_cpp_name(operator):
    # This is in sync with the operator enum.
    # If changed in Godot it needs to be changed here.
    operator_map = [
        "==",
        "!=",
        "<",
        "<=",
        ">",
        ">=",
        "+",
        "-",
        "*",
        "/",
        "-",
        "+",
        "%",
        "<<",
        ">>",
        "&",
        "|",
        "^",
        "~",
        "&&",
        "||",
        "xor", # There's no "xor" in C++ but also not used in Godot anyway.
        "!",
        "in", # Here for consistency, but this case is treated apart in generation.
    ]
    return operator_map[operator]


def get_stringifiable_types():
    return [
        "bool",
        "int",
        "float",
        "Vector2",
        "Vector2i",
        "Rect2",
        "Rect2i",
        "Vector3",
        "Vector3i",
        "Transform2D",
        "Plane",
        "Quat",
        "AABB",
        "Basis",
        "Transform",
        "Color",
        "RID",
        "Callable",
        "Signal",
        "Dictionary",
        "Array",
        "PackedByteArray",
        "PackedInt32Array",
        "PackedInt64Array",
        "PackedFloat32Array",
        "PackedFloat64Array",
        "PackedStringArray",
        "PackedVector2Array",
        "PackedVector3Array",
        "PackedColorArray",
    ]


def strip_name(name):
    if len(name) == 0:
        return name
    if name[0] == '_':
        return name[1:]
    return name

def extract_nested_type(nested_type):
    return strip_name(nested_type[:nested_type.find("::")])

def remove_nested_type_prefix(name):
    return name if name.find("::") == -1 else strip_name(name[name.find("::") + 2:])

def remove_enum_prefix(name):
    return strip_name(name[name.find("enum.") + 5:])

def builtin_type_to_gdnative(name):
    if name == "Object":
        return "godot_" + camel_to_snake(name) + " *"
    return "godot_" + camel_to_snake(name)

def builtin_type_to_gdnative_enum (name):
    return "GODOT_VARIANT_TYPE_" + camel_to_snake(name).upper()

def camel_to_snake(name):
    name = re.sub('(.)([A-Z][a-z]+)', r'\1_\2', name)
    name = re.sub('([a-z0-9])([A-Z])', r'\1_\2', name)
    return name.replace("2_D", "2D").replace("3_D", "3D").lower()

def is_nested_type(name, type = ""):
    return name.find(type + "::") != -1

def is_enum(name):
    return name.find("enum.") == 0

def is_class_type(name):
    return not is_core_type(name) and not is_primitive(name)

def is_core_type(name):
    core_types = [
                  "String",
                  "Vector2",
                  "Vector2i",
                  "Rect2",
                  "Rect2i",
                  "Vector3",
                  "Vector3i",
                  "Transform2D",
                  "Plane",
                  "Quat",
                  "AABB",
                  "Basis",
                  "Transform",
                  "Color",
                  "StringName",
                  "NodePath",
                  "RID",
                  "Callable",
                  "Signal",
                  "Dictionary",
                  "Array",
                  "PackedByteArray",
                  "PackedInt32Array",
                  "PackedInt64Array",
                  "PackedFloat32Array",
                  "PackedFloat64Array",
                  "PackedStringArray",
                  "PackedVector2Array",
                  "PackedVector3Array",
                  "PackedColorArray",
                  "Error",
                  "Variant",
                ]
    return name in core_types


def is_inlined_type(name):
    # Types that are hard-copied from Godot, instead of having bindings generated.
    # So you can use them with inline methods for better performance.
    inlined = [
        "Vector2",
        "Vector2i",
        "Rect2",
        "Rect2i",
        "Vector3",
        "Vector3i",
        "Transform2D",
        "Plane",
        "Quat",
        "AABB",
        "Basis",
        "Transform",
        "Color",
    ]
    return name in inlined


def is_primitive(name):
    core_types = ["int", "bool", "float", "void"]
    return name in core_types


def need_destructor(name):
    destructable_types = [
        "Array",
        "Callable",
        "Dictionary",
        "NodePath",
        "PackedByteArray",
        "PackedInt32Array",
        "PackedInt64Array",
        "PackedFloat32Array",
        "PackedFloat64Array",
        "PackedStringArray",
        "PackedVector2Array",
        "PackedVector3Array",
        "PackedColorArray",
        "Signal",
        "String",
        "StringName",
        "Object",
    ]
    return name in destructable_types


def escape_cpp(name):
    escapes = {
        "class":    "_class",
        "char":     "_char",
        "short":    "_short",
        "bool":     "_bool",
        "int":      "_int",
        "default":  "_default",
        "case":     "_case",
        "switch":   "_switch",
        "export":   "_export",
        "template": "_template",
        "new":      "new_",
        "operator": "_operator",
        "typeof": "type_of",
        "typename": "type_name",
    }
    if name in escapes:
        return escapes[name]
    return name


def parse_default_value(type_name, value):
    if type_name == "bool":
        if value == "True":
            return "true"
        else:
            return "false"
    elif type_name == "int":
        return value
    elif type_name == "float":
        return value
    elif type_name == "String":
        return f'"{value}"'
    elif type_name == "Array":
        if value == "[]":
            return "Array()"
        else:
            raise Exception(f"Array default value not implemented: type: {type_name}, value: {value}")
    elif type_name == "Variant":
        if value == "Null":
            return "Variant()"
        else:
            raise Exception(f"Can't parse Variant default value: type: {type_name}, value: {value}")
