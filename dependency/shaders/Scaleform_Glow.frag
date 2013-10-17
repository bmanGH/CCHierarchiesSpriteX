// 外发光
precision mediump float;
uniform vec4 fsize;
uniform vec4 offset;
uniform vec4 scolor;
uniform sampler2D srctex;
uniform vec4 srctexscale;
uniform sampler2D tex;
uniform vec4 texscale;
varying vec4 fucxadd;
varying vec4 fucxmul;
varying mediump vec2 tc;
void main() {
lowp vec4 fcolor;
fcolor       = vec4(0.0, 0.0, 0.0, 0.0);
vec4 color = vec4(0.0, 0.0, 0.0, 0.0);
vec2 i = vec2(0.0, 0.0);
for (i.x = -fsize.x; i.x <= fsize.x; i.x++)
{
for (i.y = -fsize.y; i.y <= fsize.y; i.y++)
{
color += texture2D(tex,  tc + (offset.xy + i) * texscale.xy);
}
} // EndBox2.
fcolor = color * fsize.w;
lowp vec4 base = texture2D(srctex,  tc * srctexscale.xy);
fcolor = scolor * clamp(fcolor.a * (1.0-base.a) * fsize.z, 0.0, 1.0) + base;
lowp vec4 knockout = base;
fcolor = (fcolor * vec4(fucxmul.rgb,1.0)) * fucxmul.a;
fcolor += fucxadd * fcolor.a;
fcolor = fcolor;
gl_FragColor = fcolor;
}
