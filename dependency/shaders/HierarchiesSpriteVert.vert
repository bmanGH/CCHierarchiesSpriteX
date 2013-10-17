attribute vec4 a_position;
attribute vec2 a_texCoord;
attribute vec4 a_color;

//uniform highp vec2 u_texelSize;

varying lowp vec4 v_fragmentColor;
varying mediump vec2 v_texCoord;
varying mediump vec2 v_texCoord_L;
varying mediump vec2 v_texCoord_R;
varying mediump vec2 v_texCoord_T;
varying mediump vec2 v_texCoord_B;

void main()
{
  gl_Position = CC_MVPMatrix * a_position;
  v_fragmentColor = a_color;

  vec2 u_texelSize = vec2(1.0 / 1136.0, 1.0 / 768.0);
  vec2 widthStep = vec2(u_texelSize.x, 0.0);
  vec2 heightStep = vec2(0.0, u_texelSize.y);
  //vec2 widthHeightStep = vec2(u_texelSize.x, u_texelSize.y);
  //vec2 widthNegativeHeightStep = vec2(u_texelSize.x, -u_texelSize.y);

  v_texCoord = a_texCoord.xy;
  v_texCoord_L = a_texCoord.xy - widthStep;
  v_texCoord_R = a_texCoord.xy + widthStep;
  v_texCoord_T = a_texCoord.xy - heightStep;
  v_texCoord_B = a_texCoord.xy + heightStep;
}
