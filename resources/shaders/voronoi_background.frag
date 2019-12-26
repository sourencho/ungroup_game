// Modified version of @patriciogv "4 cells voronoi"
// https://thebookofshaders.com/12/

#ifdef GL_ES
precision mediump float;
#endif

uniform vec2 u_resolution;
uniform vec2 u_position;
uniform float u_radius;
uniform float u_time;

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
    const int COLOR_COUNT = 4;
    vec3 color_pool[COLOR_COUNT];

    // Green palette
    // color_pool[0] = vec3(66. / 255., 118. / 255., 118. / 255.);
    // color_pool[1] = vec3(63. / 255., 157. / 255., 130. / 255.);
    // color_pool[2] = vec3(161. / 255., 205. / 255., 115. / 255.);
    // color_pool[3] = vec3(236. / 255., 219. / 255., 96. / 255.);

    // Soft palette
    color_pool[0] = vec3(10. / 255., 10. / 255., 10. / 255.);
    color_pool[1] = vec3(20. / 255., 20. / 255., 20. / 255.);
    color_pool[2] = vec3(30. / 255., 30. / 255., 30. / 255.);
    color_pool[3] = vec3(40. / 255., 40. / 255., 40. / 255.);

    // Cell count
    const int MAX_CELL_COUNT = 100;
    const int CELL_COUNT = 4;

    // Cell positions
    vec2 point[MAX_CELL_COUNT];
    for (int i = 0; i < CELL_COUNT; i++) {
        point[i] = random2(vec2(float(i), 1.0));
    }

    // Cell colors
    vec3 point_color[MAX_CELL_COUNT];
    for (int i = 0; i < CELL_COUNT; i++) {
        int color_index = int(mod(float(i), float(COLOR_COUNT)));
        point_color[i] = color_pool[color_index];
    }

    float m_dist = 1.; // minimun distance
    vec2 m_point;      // minimum position
    vec3 m_point_color;

    // Iterate through the points positions
    for (int i = 0; i < CELL_COUNT; i++) {
        point[i] = 0.5 + 0.5 * sin(u_time * 0.05 + 6.2831 * point[i]);
        float dist = distance(st, point[i]);
        if (dist < m_dist) {
            // Keep the closer distance
            m_dist = dist;

            // Keep the position of the closer point
            m_point = point[i];
            m_point_color = point_color[i];
        }
    }

    // set color to closest point color
    color = m_point_color;

    gl_FragColor = vec4(color, 1.0);
}
