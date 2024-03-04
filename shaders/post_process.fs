#version 400 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D deferred;
uniform sampler2D gNormal;

uniform int vignette;
uniform int kernel;
uniform int wave;
uniform int inverse;
uniform int fxaa;

uniform vec2 screenResolution;

float offset_x = 1.0f / screenResolution.x;  
float offset_y = 1.0f / screenResolution.y;  

vec2 offsets[9] = vec2[]
(
    vec2(-offset_x,  offset_y), vec2( 0.0f,    offset_y), vec2( offset_x,  offset_y),
    vec2(-offset_x,  0.0f),     vec2( 0.0f,    0.0f),     vec2( offset_x,  0.0f),
    vec2(-offset_x, -offset_y), vec2( 0.0f,   -offset_y), vec2( offset_x, -offset_y) 
);

float kernel_arr[9] = float[]
(
    1,  1, 1,
    1, -10, 1,
    1,  1, 1
);

#define FXAA_SPAN_MAX 8.0
//#define FXAA_REDUCE_MUL (1.0/FXAA_SPAN_MAX)
#define FXAA_REDUCE_MUL 0
#define FXAA_REDUCE_MIN (1.0/128.0)
//#define FXAA_SUBPIX_SHIFT (1.0/4.0)
#define FXAA_SUBPIX_SHIFT 0

vec3 FxaaPixelShader(vec4 uv, sampler2D tex, vec2 rcpFrame) {
  vec3 rgbNW = textureLod(tex, uv.zw, 0.0).xyz;
  vec3 rgbNE = textureLod(tex, uv.zw + vec2(1,0)*rcpFrame.xy, 0.0).xyz;
  vec3 rgbSW = textureLod(tex, uv.zw + vec2(0,1)*rcpFrame.xy, 0.0).xyz;
  vec3 rgbSE = textureLod(tex, uv.zw + vec2(1,1)*rcpFrame.xy, 0.0).xyz;
  vec3 rgbM  = textureLod(tex, uv.xy, 0.0).xyz;

  vec3 luma = vec3(0.299, 0.587, 0.114);
  float lumaNW = dot(rgbNW, luma);
  float lumaNE = dot(rgbNE, luma);
  float lumaSW = dot(rgbSW, luma);
  float lumaSE = dot(rgbSE, luma);
  float lumaM  = dot(rgbM,  luma);

  float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
  float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));

  vec2 dir;
  dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
  dir.y =  ((lumaNW + lumaSW) - (lumaNE + lumaSE));

  float dirReduce = max(
      (lumaNW + lumaNE + lumaSW + lumaSE) * (0.25 * FXAA_REDUCE_MUL),
      FXAA_REDUCE_MIN);
  float rcpDirMin = 1.0/(min(abs(dir.x), abs(dir.y)) + dirReduce);
  
  dir = min(vec2( FXAA_SPAN_MAX,  FXAA_SPAN_MAX),
        max(vec2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX),
        dir * rcpDirMin)) * rcpFrame.xy;

  vec3 rgbA = (1.0/2.0) * (
      textureLod(tex, uv.xy + dir * (1.0/3.0 - 0.5), 0.0).xyz +
      textureLod(tex, uv.xy + dir * (2.0/3.0 - 0.5), 0.0).xyz);
  vec3 rgbB = rgbA * (1.0/2.0) + (1.0/4.0) * (
      textureLod(tex, uv.xy + dir * (0.0/3.0 - 0.5), 0.0).xyz +
      textureLod(tex, uv.xy + dir * (3.0/3.0 - 0.5), 0.0).xyz);
  
  float lumaB = dot(rgbB, luma);

  if((lumaB < lumaMin) || (lumaB > lumaMax)) return rgbA;
  
  return rgbB; 
}

vec4 texture_fxaa(sampler2D text, vec2 coord){
  if(fxaa==1){
    vec4 gnorm=texture(gNormal, TexCoords);
    vec3 normal=gnorm.xyz;
    if(length(normal)==0){
      return texture(text, coord);
    }
    vec2 rcpFrame = 1/screenResolution;
    return vec4(FxaaPixelShader(vec4(coord, coord - (rcpFrame * (0.5 + FXAA_SUBPIX_SHIFT))), text, rcpFrame),1.0);
  }
  else{
    return texture(text, coord);
  }
}

void main() 
{
  if(vignette==1){
    vec2 position = (gl_FragCoord.xy / screenResolution.xy) - vec2(0.5);
    float len = length(position);
    FragColor = texture_fxaa(deferred, TexCoords);
    FragColor=vec4(FragColor.xyz*(1-len),1.0);
  }
  else if(kernel==1){
    vec3 color = vec3(0.0f);
    for(int i = 0; i < 9; i++){
      color += vec3(texture_fxaa(deferred, TexCoords.st + offsets[i])) * kernel_arr[i];
    }
    FragColor = vec4(color, 1.0f);
  }
  else if(wave==1){
    vec2 wavecoord = TexCoords;
    wavecoord.x += sin(wavecoord.y * 25.13272 + 100) / 100;
    FragColor = texture_fxaa(deferred, wavecoord);
  }
  else if(inverse==1){
    FragColor = vec4(1.0f) - texture_fxaa(deferred, TexCoords);
  }
  else{
    FragColor = texture_fxaa(deferred, TexCoords);
  }
}  