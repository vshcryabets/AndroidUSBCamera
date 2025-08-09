import subprocess
import sys

if "--prepare" in sys.argv:
    subprocess.run(
        ["cmake", "-B", "./build", "-DCMAKE_BUILD_TYPE=Release"],
        check=True
    )
if "--build" in sys.argv:
    subprocess.run(
        ["cmake", "--build", "./build", "--config", "Release"],
        check=True
    )
if "--test" in sys.argv:
    subprocess.run(
        ["./Decoders_tests", "-s"],
        cwd="./build/modules/decoders",
        check=True
    )
    subprocess.run(
        ["./Encoders_tests", "-s"],
        cwd="./build/modules/encoders",
        check=True
    )
    subprocess.run(
        ["./images_tests", "-s"],
        cwd="./build/modules/images",
        check=True
    )
    subprocess.run(
        ["./Sources_tests"],
        cwd="./build/modules/sources",
        check=True
    )