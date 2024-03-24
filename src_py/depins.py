import sys
import subprocess
import os


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
    install("peft")
    install("tk")
    install("requests")
    if not os.path.exists(".\\aimodels\\hm.safetensors"):
        import requests
        import shutil

        print("Installing Lora model...")
        response = requests.get(
            "https://civitai.com/api/download/models/22792?type=Model&format=SafeTensor&size=full&fp=fp16",
            stream=True,
            headers={"User-agent": "Mozilla/5.0"},
        )
        if response.status_code == 200:
            with open("aimodels/hm.safetensors", "wb") as f:
                response.raw.decode_content = True
                shutil.copyfileobj(response.raw, f)
