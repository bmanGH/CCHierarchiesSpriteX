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

float gaussian (float x, float deviation) { 
	return (1.0 / sqrt(2.0 * 3.141592 * deviation)) * exp(-((x * x) / (2.0 * deviation)));
}

void main()                                                                                 
{
	ivec2 u_blurSize = ivec2(10, 10);
	vec2 u_texelSize = vec2(1.0 / 550.0, 1.0 / 400.0);

	float deviation = 0.0;
	vec4 color = vec4(0.0);
	vec4 texColor = vec4(0.0);

	// Horizontal blur
	float halfBlurX = float(u_blurSize.x) * 0.5;
	deviation =  halfBlurX * 0.35;
	deviation *= deviation;
	for (int i = 0; i < u_blurSize.x; ++i) {
		float offset = float(i) - halfBlurX;
		vec2 texCoord = v_texCoord + vec2(offset * u_texelSize.x, 0.0);
		texColor = texture2D(CC_Texture0, texCoord) * gaussian(offset, deviation);
		color += texColor;
	}

	// Vertical blur 
	float halfBlurY = float(u_blurSize.y) * 0.5;
	deviation =  halfBlurY * 0.35;
	deviation *= deviation;
	for (int j = 0; j < u_blurSize.y; ++j) {
		float offset = float(j) - halfBlurY;
		vec2 texCoord = v_texCoord + vec2(0.0, offset * u_texelSize.y);
		texColor = texture2D(CC_Texture0, texCoord) * gaussian(offset, deviation);
		color += texColor;
	}

	//gl_FragColor = clamp(color, 0.0, 1.0); 
	gl_FragColor = color;
	gl_FragColor.a = 1.0;
}