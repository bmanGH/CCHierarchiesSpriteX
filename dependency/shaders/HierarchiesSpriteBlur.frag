#ifdef GL_ES                                                                                
precision mediump float;                                                                       
#endif                                                                                      

uniform lowp sampler2D CC_Texture0;
// uniform mediump ivec2 u_blurSize;
// uniform mediump vec2 u_texelSize;

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

void main()                                                                                 
{
	//mediump ivec2 u_blurSize = ivec2(10, 10);
	mediump vec2 u_texelSize = vec2(1.0 / 128.0, 1.0 / 128.0);
	lowp vec4 origColor = texture2D(CC_Texture0, vec2(v_texCoord.x, v_texCoord.y));
	lowp vec4 colorH = vec4(0.0);
	lowp vec4 colorV = vec4(0.0);

	// Horizontal blur
   	colorH += texture2D(CC_Texture0, vec2(v_texCoord.x, v_texCoord.y - 4.0 * u_texelSize.y)) * 0.05;
   	colorH += texture2D(CC_Texture0, vec2(v_texCoord.x, v_texCoord.y - 3.0 * u_texelSize.y)) * 0.09;
   	colorH += texture2D(CC_Texture0, vec2(v_texCoord.x, v_texCoord.y - 2.0 * u_texelSize.y)) * 0.12;
   	colorH += texture2D(CC_Texture0, vec2(v_texCoord.x, v_texCoord.y - u_texelSize.y)) * 0.15;
   	colorH += origColor * 0.16;
   	colorH += texture2D(CC_Texture0, vec2(v_texCoord.x, v_texCoord.y + u_texelSize.y)) * 0.15;
   	colorH += texture2D(CC_Texture0, vec2(v_texCoord.x, v_texCoord.y + 2.0 * u_texelSize.y)) * 0.12;
   	colorH += texture2D(CC_Texture0, vec2(v_texCoord.x, v_texCoord.y + 3.0 * u_texelSize.y)) * 0.09;
   	colorH += texture2D(CC_Texture0, vec2(v_texCoord.x, v_texCoord.y + 4.0 * u_texelSize.y)) * 0.05;
   	// colorH /= 9.0;

   	// Vertical blur 
	colorV += texture2D(CC_Texture0, vec2(v_texCoord.x - 4.0 * u_texelSize.x, v_texCoord.y)) * 0.05;
	colorV += texture2D(CC_Texture0, vec2(v_texCoord.x - 3.0 * u_texelSize.x, v_texCoord.y)) * 0.09;
	colorV += texture2D(CC_Texture0, vec2(v_texCoord.x - 2.0 * u_texelSize.x, v_texCoord.y)) * 0.12;
	colorV += texture2D(CC_Texture0, vec2(v_texCoord.x - u_texelSize.x, v_texCoord.y)) * 0.15;
	colorV += origColor * 0.16;
	colorV += texture2D(CC_Texture0, vec2(v_texCoord.x + u_texelSize.x, v_texCoord.y)) * 0.15;
	colorV += texture2D(CC_Texture0, vec2(v_texCoord.x + 2.0 * u_texelSize.x, v_texCoord.y)) * 0.12;
	colorV += texture2D(CC_Texture0, vec2(v_texCoord.x + 3.0 * u_texelSize.x, v_texCoord.y)) * 0.09;
	colorV += texture2D(CC_Texture0, vec2(v_texCoord.x + 4.0 * u_texelSize.x, v_texCoord.y)) * 0.05;
	// colorV /= 9.0;

	gl_FragColor = colorH * 0.5 + colorV * 0.5;
}
