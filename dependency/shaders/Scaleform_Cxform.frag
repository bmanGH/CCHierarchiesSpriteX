// cxform
precision mediump float;
varying lowp vec4 color;
varying lowp vec4 factor;
void main() {
lowp vec4 fcolor;
fcolor = color;
fcolor.a *= factor.a;
fcolor = fcolor;
gl_FragColor = fcolor;
}
