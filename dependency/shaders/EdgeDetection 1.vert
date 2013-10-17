attribute vec4 a_position;
attribute vec2 a_texCoord;
attribute vec4 a_color;

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

  float texelWidth = 1.0 / 1136.0;
  float texelHeight = 1.0 / 768.0;
  vec2 widthStep = vec2(texelWidth, 0.0);
  vec2 heightStep = vec2(0.0, texelHeight);
  vec2 widthHeightStep = vec2(texelWidth, texelHeight);
  vec2 widthNegativeHeightStep = vec2(texelWidth, -texelHeight);

  v_texCoord = a_texCoord.xy;
  v_texCoord_L = a_texCoord.xy - widthStep;
  v_texCoord_R = a_texCoord.xy + widthStep;
  v_texCoord_T = a_texCoord.xy - heightStep;
  v_texCoord_B = a_texCoord.xy + heightStep;
}
