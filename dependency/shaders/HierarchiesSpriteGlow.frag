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
varying mediump vec2 v_texCoord_B;

void main()                                                                                 
{                             
	//mediump ivec2 u_glowSize = ivec2(10, 10);
	mediump vec2 u_texelSize = vec2(1.0 / 128.0, 1.0 / 128.0);
	lowp vec4 origColor = texture2D(CC_Texture0, vec2(v_texCoord.x, v_texCoord.y));
	lowp vec4 colorH = vec4(0.0);
	lowp vec4 colorV = vec4(0.0);
	lowp vec3 glowColor = vec3(1.0, 0.0, 0.0);

	// box filter
	//     Horizontal
   	// colorH += texture2D(CC_Texture0, vec2(v_texCoord.x, v_texCoord.y - 4.0 * u_texelSize.y));
   	// colorH += texture2D(CC_Texture0, vec2(v_texCoord.x, v_texCoord.y - 3.0 * u_texelSize.y));
   	// colorH += texture2D(CC_Texture0, vec2(v_texCoord.x, v_texCoord.y - 2.0 * u_texelSize.y));
   	// colorH += texture2D(CC_Texture0, vec2(v_texCoord.x, v_texCoord.y - u_texelSize.y));
   	// colorH += origColor;
   	// colorH += texture2D(CC_Texture0, vec2(v_texCoord.x, v_texCoord.y + u_texelSize.y));
   	// colorH += texture2D(CC_Texture0, vec2(v_texCoord.x, v_texCoord.y + 2.0 * u_texelSize.y));
   	// colorH += texture2D(CC_Texture0, vec2(v_texCoord.x, v_texCoord.y + 3.0 * u_texelSize.y));
   	// colorH += texture2D(CC_Texture0, vec2(v_texCoord.x, v_texCoord.y + 4.0 * u_texelSize.y));
   	// colorH /= 9.0;

   	colorH += texture2D(CC_Texture0, v_texCoord_T);
   	colorH += origColor;
   	colorH += texture2D(CC_Texture0, v_texCoord_B);

   	//     Vertical
	// colorV += texture2D(CC_Texture0, vec2(v_texCoord.x - 4.0 * u_texelSize.x, v_texCoord.y));
	// colorV += texture2D(CC_Texture0, vec2(v_texCoord.x - 3.0 * u_texelSize.x, v_texCoord.y));
	// colorV += texture2D(CC_Texture0, vec2(v_texCoord.x - 2.0 * u_texelSize.x, v_texCoord.y));
	// colorV += texture2D(CC_Texture0, vec2(v_texCoord.x - u_texelSize.x, v_texCoord.y));
	// colorV += origColor;
	// colorV += texture2D(CC_Texture0, vec2(v_texCoord.x + u_texelSize.x, v_texCoord.y));
	// colorV += texture2D(CC_Texture0, vec2(v_texCoord.x + 2.0 * u_texelSize.x, v_texCoord.y));
	// colorV += texture2D(CC_Texture0, vec2(v_texCoord.x + 3.0 * u_texelSize.x, v_texCoord.y));
	// colorV += texture2D(CC_Texture0, vec2(v_texCoord.x + 4.0 * u_texelSize.x, v_texCoord.y));
	// colorV /= 9.0;

	colorV += texture2D(CC_Texture0, v_texCoord_L);
	colorV += origColor;
	colorV += texture2D(CC_Texture0, v_texCoord_R);

	gl_FragColor = colorH * 0.5 + colorV * 0.5;
	gl_FragColor.rgb = glowColor.rgb * (1.0 - origColor.a) + origColor.rgb * origColor.a;
}
