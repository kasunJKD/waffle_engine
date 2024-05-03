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
     float displacement = sin(5.0 * p.x) * sin(5.0 * p.y) * sin(5.0 * p.z) * 0.25;
    //float dCube = sdBox(p - vec3(0.0, 0.0, 0.0), vec3(0.5));  // Cube centered at origin
    float dSphere = sdSphere(p, vec3(0.0, 0.0, 0.0), 1.0);     // Sphere shifted right on the x-axis
   // return min(dCube, dSphere);  // Union operation for SDFs
    return dSphere + displacement;
}

vec3 calculateNormals(vec3 p)
{
    const vec3 small_step = vec3(0.001, 0.0, 0.0);

    float gradient_x = sceneSDF(p + small_step.xyy) - sceneSDF(p - small_step.xyy);
    float gradient_y = sceneSDF(p + small_step.yxy) - sceneSDF(p - small_step.yxy);
    float gradient_z = sceneSDF(p + small_step.yyx) - sceneSDF(p - small_step.yyx);

    vec3 normal = vec3(gradient_x, gradient_y, gradient_z);

    return normalize(normal);
}

// Raymarching function
vec3 rayMarch(vec3 ro, vec3 rd) {
    float marchDistance = 0.0;
    const float maxDistance = 1000.0;
    const int maxSteps = 100;
    const float minDistance = 0.001;
    for (int i = 0; i < maxSteps; i++) {
        vec3 p = ro + marchDistance * rd;
        float dist = sceneSDF(p);
        if (dist < minDistance) {
            vec3 normal = calculateNormals(p);
            //light position
            vec3 light_position = vec3(2.0, -5.0, 3.0);
            vec3 direction_to_light = normalize(p - light_position);
            float diffuse_intensity = max(0.0, dot(normal, direction_to_light));

            return vec3(1.0, 1.0, 1.0) * diffuse_intensity;
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
