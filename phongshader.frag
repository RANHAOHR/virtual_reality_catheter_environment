// gouraud shading with Phong illumination

uniform float ns_;
uniform vec4 K_a;
uniform vec4 K_d;
uniform vec4 K_s;
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
	vec4 Iamb = K_a * gl_FrontLightProduct[0].ambient; 

	//calculate Diffuse Term:  
	vec4 Idiff = K_d * gl_FrontLightProduct[0].diffuse * max(dot(N_, L_), 0.0);  

	// calculate Specular Term:
	vec4 Ispec = K_s * gl_FrontLightProduct[0].specular * pow(max(dot(H_,V_),0.0), ns_);
    Ispec = clamp(Ispec, 0.0, 1.0);  // between 0-1

    gl_FragColor = gl_FrontLightModelProduct.sceneColor + Iamb + Idiff + Ispec;
   // gl_FragColor = vec4(0.0,1.0,1.0,1.0); //debug

}
