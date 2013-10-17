// Sobel Edge Detection

#ifdef GL_ES                                                                                
precision mediump float;                                                                       
#endif                  

uniform lowp sampler2D CC_Texture0;                                                             
// uniform mediump ivec2 u_glowSize;
// uniform mediump vec2 u_texelSize; 
// uniform lowp vec3 u_glowColor;

varying lowp vec4 v_fragmentColor;    

varying mediump vec2 v_texCoord;
varying mediump vec2 v_texCoord_L;
varying mediump vec2 v_texCoord_R;

varying mediump vec2 v_texCoord_T;
varying mediump vec2 v_texCoord_TL;
varying mediump vec2 v_texCoord_TR;

varying mediump vec2 v_texCoord_B;
varying mediump vec2 v_texCoord_BL;
varying mediump vec2 v_texCoord_BR;                                                                     

const  vec3 rgb2lum =  vec3(0.30, 0.59, 0.11);

// averaged pixel intensity from 3 color channels
float avg_intensity(in  vec4 pix) {
  return dot(pix.rgb, rgb2lum);
}

 vec4 get_pixel(in vec2 coords, in vec2 offset) {
  return texture2D(CC_Texture0, coords + offset);
}

// returns pixel color
float IsEdge(in vec2 coords){
    // float dxtex = 1.0 / 1136.0 /*image width*/;
    // float dytex = 1.0 / 768.0 /*image height*/;
    float pix[9];
    
    // int k = -1;
    // for (int i=-1; i<2; i++) {
    //   for(int j=-1; j<2; j++) {
    //     k++;
    //     pix[k] = avg_intensity(get_pixel(coords, vec2(float(i) * dxtex, float(j) * dytex)));
    //   }
    // }

    pix[0] = avg_intensity(texture2D(CC_Texture0, v_texCoord_BL));
    pix[1] = avg_intensity(texture2D(CC_Texture0, v_texCoord_L));
    pix[2] = avg_intensity(texture2D(CC_Texture0, v_texCoord_TL));
    pix[3] = avg_intensity(texture2D(CC_Texture0, v_texCoord_B));
    pix[4] = avg_intensity(texture2D(CC_Texture0, v_texCoord));
    pix[5] = avg_intensity(texture2D(CC_Texture0, v_texCoord_T));
    pix[6] = avg_intensity(texture2D(CC_Texture0, v_texCoord_BR));
    pix[7] = avg_intensity(texture2D(CC_Texture0, v_texCoord_R));
    pix[8] = avg_intensity(texture2D(CC_Texture0, v_texCoord_TR));

    float x = pix[2] +  pix[8] + 2.0 * pix[5] - pix[0] - 2.0 * pix[3] - pix[6];
    float y = pix[6] + 2.0 * pix[7] +  pix[8] - pix[0] - 2.0 * pix[1] - pix[2];

    return (x * x + y * y < 0.09) ? 1.0 : 0.0;
}

void main()
{
    lowp vec3 glowColor = vec3(1.0, 0.0, 0.0);
  	gl_FragColor = texture2D(CC_Texture0, vec2(v_texCoord.x, v_texCoord.y));
  	float d = IsEdge(v_texCoord.xy);
  	gl_FragColor.rgb = gl_FragColor.rgb * d;
}
