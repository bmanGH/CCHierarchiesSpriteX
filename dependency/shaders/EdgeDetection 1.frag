#ifdef GL_ES                                                                                
precision mediump float;                                                                       
#endif                  

uniform lowp sampler2D CC_Texture0;                                                             
// uniform mediump ivec2 u_glowSize;
// uniform mediump vec2 u_texelSize; 
// uniform lowp vec3 u_glowColor;

varying vec4 v_fragmentColor;                                                               
varying vec2 v_texCoord;     
varying mediump vec2 v_texCoord_L;
varying mediump vec2 v_texCoord_R;
varying mediump vec2 v_texCoord_T;
varying mediump vec2 v_texCoord_B;                                                                  


float threshold(in float thr1, in float thr2 , in float val) {
 if (val < thr1) {return 0.0;}
 if (val > thr2) {return 1.0;}
 return val;
}

// averaged pixel intensity from 3 color channels
float avg_intensity(in vec4 pix) {
 return (pix.r + pix.g + pix.b)/3.;
}

lowp vec4 get_pixel(in vec2 coords, in float dx, in float dy) {
 return texture2D(CC_Texture0, coords + vec2(dx, dy));
}

// returns pixel color
float IsEdge(in vec2 coords){
  // float dxtex = 1.0 / 1136.0 /*image width*/;
  // float dytex = 1.0 / 768.0 /*image height*/;
  float pix[9];
  // int k = -1;
  float delta;

  // read neighboring pixel intensities
  // for (int i=-1; i<2; i++) {
  //  for(int j=-1; j<2; j++) {
  //   k++;
  //   pix[k] = avg_intensity(get_pixel(coords,float(i)*dxtex,
  //                                         float(j)*dytex));
  //  }
  // }
  	pix[1] = avg_intensity(texture2D(CC_Texture0, v_texCoord_L));
  	pix[7] = avg_intensity(texture2D(CC_Texture0, v_texCoord_R));
  	pix[5] = avg_intensity(texture2D(CC_Texture0, v_texCoord_T));
  	pix[3] = avg_intensity(texture2D(CC_Texture0, v_texCoord_B));

  // average color differences around neighboring pixels
  // delta = (abs(pix[1]-pix[7])+
  //         abs(pix[5]-pix[3]) +
  //         abs(pix[0]-pix[8])+
  //         abs(pix[2]-pix[6])
  //          )/4.;
    delta = (abs(pix[1]-pix[7])+
          abs(pix[5]-pix[3])
           )/2.;

  // return threshold(0.25,0.4,clamp(1.8*delta,0.0,1.0));
  return clamp(1.8*delta,0.0,1.0);
}

void main()
{
  // lowp vec4 color = vec4(0.0,0.0,0.0,1.0);
  // color.g = IsEdge(v_texCoord.xy);
  // gl_FragColor = color;

	lowp vec3 glowColor = vec3(1.0, 0.0, 0.0);
  	gl_FragColor = texture2D(CC_Texture0, vec2(v_texCoord.x, v_texCoord.y));
  	float d = IsEdge(v_texCoord.xy);
  	gl_FragColor.rgb = glowColor.rgb * d + gl_FragColor.rgb * (1.0 - d);
}
