from diffusers import StableDiffusionPipeline


class hm_ai:
    pipeline = StableDiffusionPipeline.from_single_file(
        "aimodels/hm.safetensors", original_config_file=None
    )

    def use(self, input: str):
        return self.pipeline(input).images[0]
