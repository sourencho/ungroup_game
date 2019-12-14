// Modified version of @patriciogv "4 cells voronoi" (https://thebookofshaders.com/12/)
// Colors voronoi cells according to resource colors and counts passed in.

#ifdef GL_ES
precision mediump float;
#endif

const int MAX_CELL_COUNT = 100;
const int RESOURCES_PER_CELL = 2;
const int COLOR_COUNT = 4;

uniform vec2 u_resolution; // resolution of window
uniform vec2 u_position;   // position of circle
uniform float u_radius;    // radius of circle
uniform float u_time;
uniform int u_maxResources; // Needs to be set the most the sum of u_resourceCounts will be.
uniform float u_resourceCounts[COLOR_COUNT]; // Order of resources needs to correspond to order of
                                             // colors in  color_pool.

// Green palette
const vec3 green_a = vec3(66. / 255., 118. / 255., 118. / 255.);
const vec3 green_b = vec3(63. / 255., 157. / 255., 130. / 255.);
const vec3 green_c = vec3(161. / 255., 205. / 255., 115. / 255.);
const vec3 green_d = vec3(236. / 255., 219. / 255., 96. / 255.);

// Soft palette
const vec3 soft_a = vec3(159. / 255., 224. / 255., 246. / 255.);
const vec3 soft_b = vec3(243. / 255., 229. / 255., 154. / 255.);
const vec3 soft_c = vec3(243. / 255., 181. / 255., 155. / 255.);
const vec3 soft_d = vec3(243. / 255., 156. / 255., 156. / 255.);

vec2 random2(vec2 p) {
    return fract(sin(vec2(dot(p, vec2(127.1, 311.7)), dot(p, vec2(269.5, 183.3)))) * 43758.5453);
}

void main() {
    vec2 frag_coord = gl_FragCoord.xy;
    vec2 coord_trans = vec2(frag_coord.x, frag_coord.y - u_resolution.y);
    coord_trans += vec2(-u_position.x, u_position.y);
    vec2 st = coord_trans / u_resolution;

    st *= u_resolution / (u_radius * 2.);
    st = fract(st);

    vec3 color = vec3(.0);

    // Cell colors
    vec3 color_pool[COLOR_COUNT];
    color_pool[0] = soft_a;
    color_pool[1] = soft_b;
    color_pool[2] = soft_c;
    color_pool[3] = soft_d;

    int CELL_COUNT = u_maxResources / RESOURCES_PER_CELL;

    /* Cell colors */
    vec3 point_color[MAX_CELL_COUNT];

    // Set all to white
    for (int i = 0; i < u_maxResources; i++) {
        point_color[i] = vec3(1.);
    }

    // Populate resources
    int color_index = 0;
    int point_color_index = 0;
    while (color_index < COLOR_COUNT) {
        for (int i = 0; i < int(u_resourceCounts[color_index]); i += RESOURCES_PER_CELL) {
            point_color[point_color_index] = color_pool[color_index];
            point_color_index += 1;
        }
        color_index += 1;
    }

    /* Cell positions */
    vec2 point[MAX_CELL_COUNT];
    for (int i = 0; i < MAX_CELL_COUNT; i++) {
        point[i] = random2(vec2(float(i), 1.0));
    }

    float m_dist = 1.;  // minimun distance
    vec2 closest_point; // minimum position
    vec3 closest_point_color;

    // Iterate through the points positions
    for (int i = 0; i < CELL_COUNT; i++) {
        point[i] = 0.5 + 0.5 * sin(u_time + 6.2831 * point[i]);
        float dist = distance(st, point[i]);
        if (dist < m_dist) {
            // Keep the closer distance
            m_dist = dist;

            // Keep the position of the closer point
            closest_point = point[i];
            closest_point_color = point_color[i];
        }
    }

    // Set color to closest point color
    color = closest_point_color;

    gl_FragColor = vec4(color, 1.0);
}