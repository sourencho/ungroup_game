// Modified version of @patriciogv "4 cells voronoi" (https://thebookofshaders.com/12/)
// Colors voronoi cells according to resource colors and counts passed in.

#ifdef GL_ES
precision mediump float;
#endif

const int MAX_CELL_COUNT = 30;
const int COLOR_COUNT = 4;

uniform vec2 u_resolution; // resolution of window
uniform vec2 u_position;   // position of circle
uniform float u_radius;    // radius of circle
uniform float u_time;
uniform int u_maxResources; // Needs to be set the most the sum of u_resourceCounts will be.
uniform float u_resourceCounts[COLOR_COUNT]; // Order of resources needs to correspond to order of
                                             // colors in  color_pool.

// Green palette
const vec4 green_a = vec4(66. / 255., 118. / 255., 118. / 255., 1.);
const vec4 green_b = vec4(63. / 255., 157. / 255., 130. / 255., 1.);
const vec4 green_c = vec4(161. / 255., 205. / 255., 115. / 255., 1.);
const vec4 green_d = vec4(236. / 255., 219. / 255., 96. / 255., 1.);

// Soft palette
const vec4 soft_a = vec4(159. / 255., 224. / 255., 246. / 255., 1.);
const vec4 soft_b = vec4(243. / 255., 229. / 255., 154. / 255., 1.);
const vec4 soft_c = vec4(195. / 255., 235. / 255., 141. / 255., 1.);
const vec4 soft_d = vec4(243. / 255., 181. / 255., 155. / 255., 1.);

vec2 random2(vec2 p) {
    return fract(sin(vec2(dot(p, vec2(127.1, 311.7)), dot(p, vec2(269.5, 183.3)))) * 43758.5453);
}

void main() {
    vec2 frag_coord = gl_FragCoord.xy;

    // flip y axis
    vec2 coord = vec2(frag_coord.x, u_resolution.y - frag_coord.y);

    // move view in opposite direction to align with top left position of object
    coord -= u_position;

    // normalize coord space
    vec2 st = coord / u_resolution;

    // make tiles with size of object
    st *= (u_resolution / (u_radius * 2.));
    st = fract(st);

    vec4 color = vec4(1., 1., 1., 0.);

    // Cell colors
    vec4 color_pool[COLOR_COUNT];
    color_pool[0] = soft_a;
    color_pool[1] = soft_b;
    color_pool[2] = soft_c;
    color_pool[3] = soft_d;

    int CELL_COUNT = int(min(float(MAX_CELL_COUNT), float(u_maxResources)));

    /* Cell colors */
    vec4 point_color[MAX_CELL_COUNT];
    for (int i = 0; i < CELL_COUNT; i++) {
        point_color[i] = vec4(1., 1., 1., .05); // Set all to transparent
    }

    // Copy color counts
    float color_counts[COLOR_COUNT];
    for (int i = 0; i < COLOR_COUNT; i++) {
        color_counts[i] = u_resourceCounts[i];
    }

    // Fill in colors evenly
    int counter = 0;
    for (int i = 0; i < MAX_CELL_COUNT; i++) {
        for (int c = 0; c < COLOR_COUNT; c++) {
            int color_index = int(mod(float(i + c), float(COLOR_COUNT)));
            if (color_counts[color_index] != 0.) {
                point_color[counter] = color_pool[color_index];
                counter += 1;
                color_counts[color_index] -= 1.;
            }
        }
    }

    /* Cell positions */
    vec2 point[MAX_CELL_COUNT];
    for (int i = 0; i < MAX_CELL_COUNT; i++) {
        point[i] = random2(vec2(float(i), 1.0));
    }

    float m_dist = 20.; // minimun distance
    vec2 closest_point; // minimum position
    vec4 closest_point_color = vec4(1., 1., 1., 0.05);

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

    gl_FragColor = color;
}
