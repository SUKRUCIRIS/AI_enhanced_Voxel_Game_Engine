from diffusers import StableDiffusionPipeline
import PIL.Image


class hm_ai:
    def __init__(self):
        self.pipeline: StableDiffusionPipeline = (
            StableDiffusionPipeline.from_single_file(
                "aimodels/hm.safetensors", original_config_file=None
            )
        )
        self.pipeline.to("cuda")

    def use(self, input: str) -> PIL.Image.Image:
        return self.pipeline(input, height=512, width=512).images[0]
