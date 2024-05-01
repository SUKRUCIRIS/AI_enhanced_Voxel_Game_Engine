import sys
import subprocess
import os


def install(package: str):
    listx = [sys.executable, "-m", "pip", "install", "--break-system-packages"]
    listx.extend(package.split(" "))
    subprocess.check_call(listx)


def install_dependencies():
    try:
        import torch

        if not torch.cuda.is_available():
            raise Exception("no cuda")
    except:
        install(
            "--upgrade torch torchvision torchaudio --index-url https://download.pytorch.org/whl/cu118"
        )
    try:
        import diffusers
    except:
        install("diffusers")
    try:
        import accelerate
    except:
        install("accelerate")
    try:
        import transformers
    except:
        install("transformers")
    try:
        import safetensors
    except:
        install("safetensors")
    try:
        import peft
    except:
        install("peft")
    try:
        import tk
    except:
        install("tk")
    try:
        import requests
    except:
        install("requests")
    if not os.path.exists("./aimodels/hm.safetensors"):
        import requests
        import shutil

        print("Installing Lora model...")
        response = requests.get(
            "https://civitai.com/api/download/models/22792?type=Model&format=SafeTensor&size=full&fp=fp16",
            stream=True,
            headers={"User-agent": "Mozilla/5.0"},
        )
        os.makedirs(os.path.dirname("aimodels/hm.safetensors"), exist_ok=True)
        if response.status_code == 200:
            with open("aimodels/hm.safetensors", "wb") as f:
                response.raw.decode_content = True
                shutil.copyfileobj(response.raw, f)
