#version 330

uniform float time;
uniform ivec2 iResolution;
smooth in vec3 fragNorm;    // Interpolated model-space normal
out vec3 outCol;    // Final pixel color


struct int_desc {
    int type;   /* Type 0: Sky Type 1: Plane, Type 2: Sphere */
    vec3 pos;
    vec3 normal;

    /* Materials */
    vec3  color;
    float diffuse_str;
    float specular_str;
    float spec_exp;
};

/*
 * Global Variables
 * */
vec4  sphere       = vec4(vec3(0.0f), 1.0f);
vec3  light        = vec3(100.0f);
vec3  light_color  = vec3(1.0);
float ambient      = 0.7;
vec3  plane_color  = vec3(0.9f);
vec3  sphere_color = vec3(0.7f);

float sphere_diffuse_str  = 0.5;
float sphere_specular_str = 1.0;
float sphere_spec_exp    = 100.0;


float ground_diffuse_str  = 0.6;
float ground_specular_str = 1.0;
float ground_spec_exp    = 20.0;

/*
 * Animate the scene
 **/
void animation() {
    /* Light moving */
    float t   = time * 0.001f;
    float dis = 100.0f;
    light = vec3(cos(t) * dis,100.0f, sin(t) * dis);

    sphere.w = (0.5 + sphere.w * abs(sin(t)) * 0.5);
}

/*
 * Ray-Sphere intersection
 **/
float sphere_intersect(vec3 ro, vec3 rd, vec4 sphere ) {
    vec3 oc = ro - sphere.xyz;
    float b = dot( oc, rd );
    float c = dot( oc, oc ) - sphere.w*sphere.w;
    float h = b*b - c;
    if( h < 0.0 ) return -1.0;
    return -b - sqrt( h );
}


/*
 * Ray-Plane Intersection
**/
float plane_intersect(vec3 ro, vec3 rd ) {
    return (-1.0 - ro.y)/rd.y;
}


vec3 reflection(vec3 vec_in, vec3 normal) {
    // TODO 4, find the formula to calculate the reflected direction
    return vec_in - 2 * (normal * vec_in) * normal;
}


vec3 ground_color(vec3 ro, vec3 rd, int_desc intersect) {
    vec2 p = intersect.pos.xz * 3.0;

    // filter kernel
    vec2 w = vec2(0.001);
    // analytical integral (box filter)
    vec2 i = 2.0*(abs(fract((p-0.5*w)*0.5)-0.5)-abs(fract((p+0.5*w)*0.5)-0.5))/w;
    // xor pattern
    float pattern = 0.5 - 0.5*i.x*i.y;
    return 0.7 + pattern*vec3(0.05);
}

vec3 sky_color(vec3 ro, vec3 rd) {
    return vec3(0.6, 0.71, 0.75) - (rd.y) * 0.2 * vec3(1.0, 0.5, 1.0) + 0.15 * 1.0;
}

/*
* Ray-Scene Intersection.
* Return the t.
* */
int_desc ray_scene_intersect(vec3 ro, vec3 rd) {
    int_desc ret;

    float tmin=1e10;

    ret.type = 0;
    ret.normal = vec3(0.0);
    ret.color = sky_color(ro, rd);

    float t1 = plane_intersect( ro, rd );
    if (t1 > 0.0) {
        tmin = t1;

        ret.type = 1;
        ret.pos = ro + rd * tmin;
        ret.normal = vec3(0.0, 1.0, 0.0);

        /* grid pattern */
        ret.color        = ground_color(ro, rd, ret);
        ret.diffuse_str  = ground_diffuse_str;
        ret.specular_str = ground_specular_str;
        ret.spec_exp     = ground_spec_exp;
    }

    float t2 = sphere_intersect( ro, rd, sphere );
    if( t2 > 0.0 && t2 < tmin ) {
        tmin = t2;

        float t = t2;
        vec3 pos = ro + t*rd;

        // TODO 2, replace the following line with the correct normal.
        //         How to compute the normal for the point pos on the sphere?
        vec3 sphere_center = vec3(sphere);
        vec3 len = pos - sphere_center;
        float mag = sqrt(len.x * len.x + len.y * len.y + len.z * len.z);
        vec3 norm = len / mag;
        //vec3 norm = vec3(0.7);

        ret.type = 2;
        ret.pos = ro + rd * tmin;
        ret.normal = norm;

        ret.color        = sphere_color;
        ret.diffuse_str  = sphere_diffuse_str;
        ret.specular_str = sphere_specular_str;
        ret.spec_exp     = sphere_spec_exp;
    }

    return ret;
}

vec3 lerp(vec3 a, vec3 b, float aa) {
    return a * aa + b * (1.0-aa);
}

/*
 * Shading the scene
 **/
vec3 shading(vec3 ro, vec3 rd, int_desc intersect) {
    vec3 ret    = vec3(0.0f);

    int   type         = intersect.type;
    vec3  pos          = intersect.pos;
    vec3  normal       = intersect.normal;
    vec3  color        = intersect.color;
    float diffuse_str  = intersect.diffuse_str;
    float specular_str = intersect.specular_str;
    float spec_exp     = intersect.spec_exp;

    vec3 sky    = sky_color(ro, rd);

    if (type == 0) {
        return sky;
    }

    ret = normal;
    vec3 light_color = lerp(sky, light_color, 0.5);

    // TODO 3, Phong shading here. Read details in the handout, Note, you do not need to finish TODO 5, 6 to see the result
    // Phong shading model ret = (ambient + diffuse + specular) * color * light_color
    // Steps:
    //      1. Read the formula for diffuse term, ignore specular term first to test the result
    //          1.1 Remember to use diffuse_str as a scaling term
    //      2. Read the formula for specular term
    //          2.1 Go to TODO 4. Implement reflection function.
    //          2.2 Use the reflection function to compute the specular term.
    //          2.3 Use the specular_str, spec_exp in the specular calculation.
    float diff = (normal.x * light.x) + (normal.y * light.y) + (normal.z * light.z);
    float mag_norm = sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
    float mag_light = sqrt(light.x * light.x + light.y * light.y + light.z * light.z);
    diff = diff / (mag_norm * mag_light);
    diff = acos(diff);
    ret = (ambient + (diffuse_str * diff)) * color * light_color;

    // TODO 5, reflection effects. Note, you do not need to finish TODO 6 to see the result
    // Steps:
    //      1. What is the ro, rd (ray origin and ray direction) when you compute reflection?
    //      2. Call the ray_scene_intersect(ro, rd) to get the ray-scene intersection results.
    //      3. Replace the light_color term with the intersection result color.


    // TODO 6 (Bonus), shadow effect
    // Steps:
    //      1. What is the ro, rd (ray origin and ray direction) when you compute shadow?
    //      2. Call the int_desc shadow_ret = ray_scene_intersect(ro, rd) to get the ray-scene intersection results.
    //      3. Use the new equation: ret = (ambient + diffuse + specular) * color * light_color * shadow
    //          3.1 If shadow_ret.type is not 0, then it is blocked by some object in the space, give 0.3 as the value for the shadow term
    //          3.2 If shadow_ret.type is 0, then it hits the sky, give 1.0 to shadow

    return ret;
}

void main() {
    animation();

    vec2 p  = (2.0 * gl_FragCoord.xy - iResolution.xy) / iResolution.y;
    vec3 ro = vec3(0.0, 0.0, 4.0);
    vec3 rd = normalize( vec3(p, -2.0) );

    int_desc intersect = ray_scene_intersect(ro, rd);
    outCol = shading(ro, rd, intersect);
}
