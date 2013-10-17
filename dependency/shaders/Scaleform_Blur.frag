// 模糊
precision mediump float;
uniform vec4 fsize;
uniform sampler2D tex;
uniform vec4 texscale;
varying vec4 fucxadd;
varying vec4 fucxmul;
varying vec2 tc;
void main() {
lowp vec4 fcolor;
fcolor       = vec4(0.0, 0.0, 0.0, 0.0);
vec4 color = vec4(0.0, 0.0, 0.0, 0.0);
vec2 i = vec2(0.0, 0.0);
for (i.x = -fsize.x; i.x <= fsize.x; i.x++)
{
for (i.y = -fsize.y; i.y <= fsize.y; i.y++)
{
color += texture2D(tex,  tc + i * texscale.xy);
}} // EndBox1/2.
fcolor = color * fsize.w;
fcolor = (fcolor * vec4(fucxmul.rgb,1.0)) * fucxmul.a;
fcolor += fucxadd * fcolor.a;
fcolor = fcolor;
gl_FragColor = fcolor;
}
