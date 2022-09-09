#version 400

uniform ivec2 u_resolution;

uniform int u_iterationLimit;

uniform double u_zoom;
uniform dvec2 u_center;

uniform bool u_crosshair;

out vec4 color;

dvec2 z = dvec2(0.0);
float mandelbrot(in dvec2 co) {
    for (int i = 0; i < u_iterationLimit; i++) {
        z = dvec2(
                z.x*z.x - z.y*z.y + co.x,
                2.0 * z.x * z.y + co.y);

        if (dot(z, z) > 4.0) {
            return float(i);
        }
    }

    return -1.0;
}

#define PI 3.14159265359
#define A  vec3(0.5)
#define B  vec3(0.5)
#define C  vec3(1.0, 0.7, 0.4)
#define D  vec3(0.0, 0.15, 0.2)
vec3 palette(in float t, in vec3 a, in vec3 b, in vec3 c, in vec3 d) {
    return a + b * cos(2*PI*(c * t + d));
}

void main() {
    dvec2 uv = gl_FragCoord.xy / u_resolution;

    dvec2 clip = 2.0 * uv - 1.0;
    double aspect = double(u_resolution.x) / double(u_resolution.y);
    clip *= dvec2(aspect, 1.0lf);
    clip /= u_zoom;
    clip += u_center;

    float numIterations = mandelbrot(clip);
    color = step(0, numIterations) * vec4(palette(numIterations / float(u_iterationLimit), A, B, C, D), 1.0);

    /* draw crosshair */
    if (u_crosshair) {
        color.g += float(1.0 - step(0.001, abs(uv.y-0.5)))
                + float(1.0 - step(0.001, abs(uv.x - 0.5)));
    }

}
