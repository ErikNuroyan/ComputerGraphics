attribute vec2 position;

uniform vec4 u_Color;
uniform mat2 u_Rotate;

void main(){
    gl_Position = vec4(u_Rotate * position,0.0,1.0);
}
