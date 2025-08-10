import subprocess
import sys

result = subprocess.run(
    ["python3", "workflow_build.py"] + sys.argv[1:],
    cwd="linux/",
    check=True
)

# Gradle
if "--prepare" in sys.argv:
    subprocess.run(
        ["chmod", "+x", "gradlew"],
        check=True
    )
if "--build" in sys.argv:
    subprocess.run(
        ["./gradlew", "assembleDebug"],
        check=True
    )
if "--test" in sys.argv:
    subprocess.run(
        ["./gradlew", "testDebugUnitTest"],
        check=True
    )
