Import("env")

env.Append(
    LINKFLAGS=[
        "-mcpu=cortex-m4",
        "--specs=nosys.specs",
        "--specs=nano.specs",
        "-mthumb",
        "-mfloat-abi=hard",
        "-mfpu=fpv4-sp-d16",
    ]
)
