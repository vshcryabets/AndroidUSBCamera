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
        ["./ctest", "-s"],
        cwd="./build",
        check=True
    )
