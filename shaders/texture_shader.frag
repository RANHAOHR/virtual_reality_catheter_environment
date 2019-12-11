//Shader for Phong Illuminations and Phong shading

uniform sampler2D blendMap;
uniform sampler2D texAort;
uniform sampler2D texPulm;
uniform sampler2D texMuscle;
uniform sampler2D texBackground;
uniform float ns_;
varying vec3 vertex_;
varying vec3 normal_;

void main()
{
   vec3 N_ = normalize(normal_);

   //compute L, V:
   vec3 L_ = normalize(gl_LightSource[0].position.xyz - vertex_);  
   vec3 V_ = normalize(-vertex_); // reverse because its from vertex to camera
   //compute H:
   vec3 H_ = (L_ + V_);
   H_ = normalize(H_);

   // ambient:
   vec4 Iamb = gl_FrontLightProduct[0].ambient; 

   // Output color = color of the texture at the specified UV
  vec4 blendColor = texture2D( blendMap, gl_TexCoord[0].st );
  vec4 redColor = texture2D( texAort, gl_TexCoord[0].st ) * blendColor.x;
  vec4 blueColor = texture2D( texPulm, gl_TexCoord[0].st ) * blendColor.z;  
  vec4 muscleColor = texture2D( texMuscle, gl_TexCoord[0].st ) * blendColor.y; 

   float background_ = 1.0 - (blendColor.x + blendColor.y + blendColor.z); 
  vec4 backgroundColor = texture2D( texBackground, gl_TexCoord[0].st ) * background_;
  vec4 totalColor =  redColor + blueColor + muscleColor + backgroundColor; 

  vec4 Idiff = totalColor * max(dot(N_, L_), 0.0);

   // calculate Specular Term:
  vec4 Ispec = gl_FrontLightProduct[0].specular * pow(max(dot(H_,V_), 0.0), ns_);
  Ispec = clamp(Ispec, 0.0, 1.0);  // between 0-1

  gl_FragColor = Iamb + Idiff + Ispec;
 // gl_FragColor = vec4(0.0,1.0,1.0,1.0); //for debug

}