from diffusers import StableDiffusionPipeline
import PIL.Image
from diffusers import AutoPipelineForText2Image
from diffusers import AutoPipelineForImage2Image
import torch


class hm_ai:
    def __init__(self, lora: bool):
        if lora is False:
            self.pipeline: StableDiffusionPipeline = (
                StableDiffusionPipeline.from_single_file(
                    "aimodels/hm.safetensors",
                    original_config_file=None,
                    torch_dtype=torch.float16,
                )
            )
        else:
            self.pipeline: AutoPipelineForText2Image = (
                AutoPipelineForText2Image.from_pretrained(
                    "runwayml/stable-diffusion-v1-5", torch_dtype=torch.float16
                ).to("cuda")
            )
            self.pipeline.load_lora_weights("aimodels/", weight_name="hm.safetensors")
            print("Loaded model 1")
            self.pipeline_text: AutoPipelineForImage2Image = (
                AutoPipelineForImage2Image.from_pretrained(
                    "runwayml/stable-diffusion-v1-5", torch_dtype=torch.float16
                ).to("cuda")
            )
            self.pipeline_text.load_lora_weights(
                "aimodels/", weight_name="texture.safetensors"
            )
            print("Loaded model 2")
            self.pipeline_text_2: AutoPipelineForText2Image = (
                AutoPipelineForText2Image.from_pretrained(
                    "runwayml/stable-diffusion-v1-5", torch_dtype=torch.float16
                ).to("cuda")
            )
            self.pipeline_text_2.load_lora_weights(
                "aimodels/", weight_name="texture.safetensors"
            )
            print("Loaded model 3")

        def dummy(images: list, **kwargs):
            false_list = [False] * images.__len__()
            return images, false_list

        self.pipeline.safety_checker = dummy
        self.pipeline_text.safety_checker = dummy
        self.pipeline_text_2.safety_checker = dummy
        self.pipeline.to("cuda")
        self.pipeline_text.to("cuda")
        self.pipeline_text_2.to("cuda")

    def use(self, prompt: str, seed: int, neg_prompt: str = None) -> PIL.Image.Image:
        generator = torch.Generator(device="cuda").manual_seed(seed)
        return self.pipeline(
            prompt=prompt,
            negative_prompt=neg_prompt,
            height=512,
            width=512,
            guidance_scale=7,
            num_images_per_prompt=1,
            num_inference_steps=50,
            generator=generator,
        ).images[0]

    def use_texture(
        self,
        prompt: str,
        seed: int,
        base_image: PIL.Image.Image,
        neg_prompt: str = None,
    ) -> PIL.Image.Image:
        generator = torch.Generator(device="cuda").manual_seed(seed)
        return self.pipeline_text(
            prompt=prompt,
            negative_prompt=neg_prompt,
            height=512,
            width=512,
            guidance_scale=7,
            num_images_per_prompt=1,
            num_inference_steps=50,
            image=base_image,
            generator=generator,
        ).images[0]

    def use_texture_2(
        self, prompt: str, seed: int, neg_prompt: str = None
    ) -> PIL.Image.Image:
        generator = torch.Generator(device="cuda").manual_seed(seed)
        return self.pipeline_text_2(
            prompt=prompt,
            negative_prompt=neg_prompt,
            height=512,
            width=512,
            guidance_scale=7,
            num_images_per_prompt=1,
            num_inference_steps=50,
            generator=generator,
        ).images[0]
