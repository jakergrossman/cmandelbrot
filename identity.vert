#version 400
in vec2 position; void main() { gl_Position = vec4( position.x, position.y, 0, 1 ); }
