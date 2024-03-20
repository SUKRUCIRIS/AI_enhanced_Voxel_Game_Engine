import sys
import subprocess


def install(package):
    subprocess.check_call(
        [sys.executable, "-m", "pip", "install", "--upgrade", package]
    )


def install_dependencies():
    install("diffusers")
    install("accelerate")
    install("transformers")
    install("safetensors")
    install("tk")
