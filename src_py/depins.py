import sys
import subprocess


def install(package: str):
    listx = [sys.executable, "-m", "pip", "install", "--upgrade"]
    listx.extend(package.split(" "))
    subprocess.check_call(listx)


def install_dependencies():
    install(
        "torch torchvision torchaudio --index-url https://download.pytorch.org/whl/cu118"
    )
    install("diffusers")
    install("accelerate")
    install("transformers")
    install("safetensors")
    install("tk")
