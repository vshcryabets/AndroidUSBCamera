import subprocess
import sys

result = subprocess.run(
    ["python3", "workflow_build.py"] + sys.argv[1:],
    cwd="linux/",
    check=True
)