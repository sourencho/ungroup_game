uniform vec2 u_resolution;
uniform vec2 u_position;
uniform float u_radius;

void main()
{
    vec2 st = gl_FragCoord.xy/u_resolution;
    vec2 coord_trans = vec2(gl_FragCoord.x, u_resolution.y - gl_FragCoord.y);
    vec2 st_trans = coord_trans/u_resolution;

    float dst = 0.0;
    dst = distance(coord_trans, u_position+u_radius);

    float dst_ratio = 0.0;
    dst_ratio = dst/u_radius;

    vec3 color = vec3(dst_ratio);
    gl_FragColor = vec4(color, 1.0);
}
