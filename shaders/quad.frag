#version 460 core
out vec4 FragColor;

uniform vec2 iResolution;   // viewport resolution (width, height)
uniform float iTime;        // shader playback time (in seconds)

// Signed Distance Function for a cube
float sdBox(vec3 p, vec3 b) {
    vec3 d = abs(p) - b;
    return length(max(d, 0.0)) + min(max(d.x, max(d.y, d.z)), 0.0);
}

// Signed Distance Function for a sphere
float sdSphere(vec3 p, vec3 center, float r) {
    return length(p - center) - r;
}

// Scene definition combining both cube and sphere
float sceneSDF(vec3 p) {
    float dCube = sdBox(p - vec3(0.0, 0.0, 0.0), vec3(0.5));  // Cube centered at origin
    float dSphere = sdSphere(p, vec3(0.5, 0.0, 0.0), 0.3);     // Sphere shifted right on the x-axis
    return min(dCube, dSphere);  // Union operation for SDFs
}

// Raymarching function
vec3 rayMarch(vec3 ro, vec3 rd) {
    float marchDistance = 0.0;
    const float maxDistance = 100.0;
    const int maxSteps = 100;
    for (int i = 0; i < maxSteps; i++) {
        vec3 p = ro + marchDistance * rd;
        float dist = sceneSDF(p);
        if (dist < 0.001) {
            return vec3(0.8, 0.2, 0.1);  // Red color if hit
        }
        marchDistance += dist;
        if (marchDistance >= maxDistance) break;
    }
    return vec3(0.0, 0.0, 0.0);  // Background color if missed
}

// Main rendering function
void main() {
    vec2 uv = (gl_FragCoord.xy - 0.5 * iResolution.xy) / iResolution.y;
    vec3 ro = vec3(0.0, 0.0, -3.0); // Camera position, back 3 units from the origin
    vec3 rd = normalize(vec3(uv, 1.0)); // Ray direction from camera through the pixel

    vec3 color = rayMarch(ro, rd);
    FragColor = vec4(color, 1.0);
}
