
extends MainLoop

func _initialize():
    OS.exit_code = 1
    var native_script = load("res://gdexample.gdns")
    print("NativeScript ", native_script)
    if native_script == null || !is_instance_valid(native_script):
        print("NativeScript invalid")
        return
    print("Library ", native_script.library)
    if native_script.library == null || !is_instance_valid(native_script.library):
        print("Library invalid")
        return
    var ref = native_script.new()
    print("Reference ", ref)
    if ref == null || !is_instance_valid(ref):
        print("Reference invalid")
        return
    print("Reference value ", ref.value)
    if ref.value != 50:
        print("Reference value mismatch")
        return
    print("Reference vec ", ref.vec)
    if ref.vec != Vector2(10, 20):
        print("Reference vec mismatch")
        return
    print("Reference name ", ref.name)
    if ref.name != "SimpleClass":
        print("Reference name mismatch")
        return
    print("Call method ", ref.method(1))
    if ref.method(1) != 1:
        print("Call method mismatch")
        return
    print("All tests passed")
    OS.exit_code = 0

func _process(_delta):
    return true
