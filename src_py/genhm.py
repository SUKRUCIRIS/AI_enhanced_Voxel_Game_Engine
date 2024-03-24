from diffusers import StableDiffusionPipeline
import PIL.Image
from diffusers import AutoPipelineForText2Image
import torch


class hm_ai:
    def __init__(self, lora: bool):
        if lora is False:
            self.pipeline: StableDiffusionPipeline = (
                StableDiffusionPipeline.from_single_file(
                    "aimodels/hm.safetensors", original_config_file=None
                )
            )
        else:
            self.pipeline: AutoPipelineForText2Image = (
                AutoPipelineForText2Image.from_pretrained(
                    "runwayml/stable-diffusion-v1-5", torch_dtype=torch.float16
                ).to("cuda")
            )
            self.pipeline.load_lora_weights("aimodels/", weight_name="hm.safetensors")
        self.pipeline.to("cuda")

    def use(self, prompt: str, neg_prompt: str = None) -> PIL.Image.Image:
        return self.pipeline(
            prompt=prompt,
            negative_prompt=neg_prompt,
            height=512,
            width=512,
            guidance_scale=10,
            num_images_per_prompt=1,
            num_inference_steps=50,
        ).images[0]
