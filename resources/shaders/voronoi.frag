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

    // Cell positions
    vec2 point[4];
    point[0] = random2(vec2(1.0, 1.0));
    point[1] = random2(vec2(2.0, 1.0));
    point[2] = random2(vec2(3.0, 1.0));
    point[3] = random2(vec2(4.0, 1.0));

    vec3 point_color[4];

    // #2 color palette
    /*
    point_color[0] = vec3(251. / 255., 174. / 255., 89. / 255.); // yellow
    point_color[1] = vec3(153. / 255., 138. / 255., 47. / 255.); // green
    point_color[2] = vec3(44. / 255., 89. / 255., 79. / 255.);   // blue
    point_color[3] = vec3(230. / 255., 70. / 255., 97. / 255.);  // red
    */
    // #3 color palette
    point_color[0] = vec3(66. / 255., 118. / 255., 118. / 255.);  // darkets
    point_color[1] = vec3(63. / 255., 157. / 255., 130. / 255.);  // dark
    point_color[2] = vec3(161. / 255., 205. / 255., 115. / 255.); // light
    point_color[3] = vec3(236. / 255., 219. / 255., 96. / 255.);  // lightest

    float m_dist = 1.; // minimun distance
    vec2 m_point;      // minimum position
    vec3 m_point_color;

    // Iterate through the points positions
    for (int i = 0; i < 4; i++) {
        point[i] = 0.5 + 0.5 * sin(u_time + 6.2831 * point[i]);
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
