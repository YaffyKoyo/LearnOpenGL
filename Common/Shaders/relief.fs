#version 420 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D depthMap;

//uniform bool parallax;
uniform float height_scale;

bool parallax = true;
//float height_scale = 0.1f;

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{ 
    // number of depth layers
    const float minLayers = 20;
    const float maxLayers = 40;
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));  
    // calculate the size of each layer
    float layerDepth = 1.0 / numLayers;
    // depth of current layer
    float currentLayerDepth = 0.0;
    // the amount to shift the texture coordinates per layer (from vector P)
    vec2 P = viewDir.xy / viewDir.z * height_scale; 
    vec2 deltaTexCoords = P / numLayers;
  
    // get initial values
    vec2  currentTexCoords     = texCoords;
    float currentDepthMapValue = texture(depthMap, currentTexCoords).r;
      
    while(currentLayerDepth < currentDepthMapValue)
    {
        // shift texture coordinates along direction of P
        currentTexCoords -= deltaTexCoords;
        // get depthmap value at current texture coordinates
        currentDepthMapValue = texture(depthMap, currentTexCoords).r;  
        // get depth of next layer
        currentLayerDepth += layerDepth;  
    }
    
    // -- parallax occlusion mapping interpolation from here on
    // get texture coordinates before collision (reverse operations)
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

    // get depth after and before collision for linear interpolation
    float afterDepth  = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = texture(depthMap, prevTexCoords).r - currentLayerDepth + layerDepth;
 
    // interpolation of texture coordinates
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;
}

float ParallaxSoftShadowMultiplier(vec3 L, vec2 T, float initialHeight ){
    float shadowMultiplier = 1;

   const float minLayers = 15;
   const float maxLayers = 30;

   // calculate lighting only for surface oriented to the light source
   if(dot(vec3(0, 0, 1), L) > 0)
   {
      // calculate initial parameters
      float numSamplesUnderSurface	= 0;
      shadowMultiplier	= 0;
      float numLayers	= mix(maxLayers, minLayers, abs(dot(vec3(0, 0, 1), L)));
      float layerHeight	= initialHeight / numLayers;
      vec2 texStep	= height_scale * L.xy / L.z / numLayers;

      // current parameters
      float currentLayerHeight	= initialHeight - layerHeight;
      vec2 currentTextureCoords	= T + texStep;
      float heightFromTexture	= texture(depthMap, currentTextureCoords).r;
      int stepIndex	= 1;

      // while point is below depth 0.0 )
      while(currentLayerHeight > 0)
      {
         // if point is under the surface
         if(heightFromTexture < currentLayerHeight)
         {
            // calculate partial shadowing factor
            numSamplesUnderSurface	+= 1;
            float newShadowMultiplier	= (currentLayerHeight - heightFromTexture) *
                                             (1.0 - stepIndex / numLayers);
            shadowMultiplier	= max(shadowMultiplier, newShadowMultiplier);
         }

         // offset to the next layer
         stepIndex	+= 1;
         currentLayerHeight	-= layerHeight;
         currentTextureCoords	+= texStep;
         heightFromTexture	= texture(depthMap, currentTextureCoords).r;
      }

      // Shadowing factor should be 1 if there were no points under the surface
      if(numSamplesUnderSurface < 1)
      {
         shadowMultiplier = 1;
      }
      else
      {
         shadowMultiplier = 1.0 - shadowMultiplier;
      }
   }
   return shadowMultiplier;
}

void main()
{           
    // Offset texture coordinates with Parallax Mapping
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    vec2 texCoords = fs_in.TexCoords;
    if(parallax)
        texCoords = ParallaxMapping(fs_in.TexCoords,  viewDir);
        
    // discards a fragment when sampling outside default texture region (fixes border artifacts)
    if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
        discard;

    // Obtain normal from normal map
    vec3 normal = texture(normalMap, texCoords).rgb;
    normal = normalize(normal * 2.0 - 1.0);   
   
    // Get diffuse color
    vec3 color = texture(diffuseMap, texCoords).rgb;
    // Ambient
    vec3 ambient = 0.1 * color;
    // Diffuse
    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;
    // Specular    
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

    vec3 specular = vec3(0.2) * spec;
    
    //float initialHeight = texture(depthMap,fs_in.TexCoords).r;
    
    //float shadowMultiplier = ParallaxSoftShadowMultiplier(normalize(fs_in.TangentLightPos),texCoords,initialHeight-0.05);
    
    //FragColor.rgb = color*(ambient+(diffuse+specular)*pow(shadowMultiplier,4));
    //FragColor.a = 1;
    FragColor = vec4(ambient + diffuse + specular, 1.0f);
}