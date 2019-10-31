uniform vec2 u_resolution;
uniform vec2 u_position;
uniform float u_radius;
uniform float u_time;

float speed = 15.0;

float random(vec2 st) {
    return fract(sin(dot(st.xy * floor(u_time * speed), vec2(12.9898, 78.233))) * 43758.5453123);
}

void main() {
    vec2 st = gl_FragCoord.xy / u_resolution.xy;

    st *= 100.0;           // Scale the coordinate system by 10
    vec2 ipos = floor(st); // get the integer coords
    vec2 fpos = fract(st); // get the fractional coords

    // Assign a random value based on the integer coord
    float rand = random(ipos);

    gl_FragColor = vec4(gl_Color.rgb, rand);
}
