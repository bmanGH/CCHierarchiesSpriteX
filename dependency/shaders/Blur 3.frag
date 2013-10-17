// precision mediump float;
// uniform mat4 CC_PMatrix;
// uniform mat4 CC_MVMatrix;
// uniform mat4 CC_MVPMatrix;
// uniform vec4 CC_Time;
// uniform vec4 CC_SinTime;
// uniform vec4 CC_CosTime;
// uniform vec4 CC_Random01;
// //CC INCLUDES END

#ifdef GL_ES                                                                                
precision highp float;                                                                       
#endif                                                                                      

uniform sampler2D CC_Texture0;
// uniform ivec2 u_blurSize;
// uniform vec2 u_texelSize;

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

void main()                                                                                 
{
	ivec2 u_blurSize = ivec2(5, 5);
	vec2 u_texelSize = vec2(1.0 / 550.0, 1.0 / 400.0);

	float deviation = 0.0;
	vec4 color = vec4(0.0);

	// Horizontal blur
	color += texture2D(CC_Texture0, vec2(v_texCoord.x, v_texCoord.y - 4.0 * u_texelSize.y)) * 0.05;
	color += texture2D(CC_Texture0, vec2(v_texCoord.x, v_texCoord.y - 3.0 * u_texelSize.y)) * 0.09;
	color += texture2D(CC_Texture0, vec2(v_texCoord.x, v_texCoord.y - 2.0 * u_texelSize.y)) * 0.12;
	color += texture2D(CC_Texture0, vec2(v_texCoord.x, v_texCoord.y - u_texelSize.y)) * 0.15;
	color += texture2D(CC_Texture0, vec2(v_texCoord.x, v_texCoord.y)) * 0.16;
	color += texture2D(CC_Texture0, vec2(v_texCoord.x, v_texCoord.y + u_texelSize.y)) * 0.15;
	color += texture2D(CC_Texture0, vec2(v_texCoord.x, v_texCoord.y + 2.0 * u_texelSize.y)) * 0.12;
	color += texture2D(CC_Texture0, vec2(v_texCoord.x, v_texCoord.y + 3.0 * u_texelSize.y)) * 0.09;
	color += texture2D(CC_Texture0, vec2(v_texCoord.x, v_texCoord.y + 4.0 * u_texelSize.y)) * 0.05;

	// Vertical blur 
	color += texture2D(CC_Texture0, vec2(v_texCoord.x - 4.0 * u_texelSize.x, v_texCoord.y)) * 0.05;
	color += texture2D(CC_Texture0, vec2(v_texCoord.x - 3.0 * u_texelSize.x, v_texCoord.y)) * 0.09;
	color += texture2D(CC_Texture0, vec2(v_texCoord.x - 2.0 * u_texelSize.x, v_texCoord.y)) * 0.12;
	color += texture2D(CC_Texture0, vec2(v_texCoord.x - u_texelSize.x, v_texCoord.y)) * 0.15;
	color += texture2D(CC_Texture0, vec2(v_texCoord.x, v_texCoord.y)) * 0.16;
	color += texture2D(CC_Texture0, vec2(v_texCoord.x + u_texelSize.x, v_texCoord.y)) * 0.15;
	color += texture2D(CC_Texture0, vec2(v_texCoord.x + 2.0 * u_texelSize.x, v_texCoord.y)) * 0.12;
	color += texture2D(CC_Texture0, vec2(v_texCoord.x + 3.0 * u_texelSize.x, v_texCoord.y)) * 0.09;
	color += texture2D(CC_Texture0, vec2(v_texCoord.x + 4.0 * u_texelSize.x, v_texCoord.y)) * 0.05;

	//gl_FragColor = clamp(color, 0.0, 1.0); 
	gl_FragColor = color;
	gl_FragColor.a = 1.0;
}
