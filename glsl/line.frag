//  CC BY-NC-SA 4.0

#define PI 3.14159265359

// A new type of algorithm for computing the distance to a cubic bezier curve
// insight:
// use complex numbers, then
// the quintic is the real part of the factored quintic
// (a*x^3 + b*x^2 + c*x + d)*(3*a*x^2 + 2*b*x + c) = 0
// This is zero if a*x^3 + b*x^2 + c*x + d = 0
// or 3*a*x^2 + 2*b*x + c = 0 (which corresponds to cusps)
// a*x^3 + b*x^2 + c*x + d = 0 always has 3 solutions and
// if the point in on the bezier, then one of the solutions
// for the curve parameter x is real.
// NOTE: the algorithm for determining the side is a bit unstable at cusps.

// iteratively improve the result using the newton method
// use 0 for the pure form of the approximation (and be surprised by how good it is!)
#define ITERATIONS 2

// implement all sorts of complex functions to compose the final analytic function,
// which is by it's very nature a conformal map. Meaning it will be a good approximation.
// complex exponential
vec2 cexp(vec2 c) {
    return exp(c.x)*vec2(cos(c.y), sin(c.y));
}
// complex logarithm
vec2 cln(vec2 c) {
    return vec2(log(dot(c,c))*.5, atan(c.y, c.x));
}
// complex multiplication
vec2 cmul(vec2 a, vec2 b) {
    return vec2(a.x * b.x - a.y * b.y, a.x * b.y + a.y * b.x);
}
// complex conjugation
vec2 conj(vec2 c) { return vec2(c.x, -c.y); }
// complex division
vec2 cdiv(vec2 a, vec2 b) {
    return cmul(a, conj(b)) / dot(b, b);
}
// complex sqrt
vec2 csqrt(vec2 a) {
    float r = length(a);
    if ((a.y + a.x) - a.x == 0.0) {
        return a.x >= 0.0 ? vec2(sqrt(r), 0.0) : vec2(0.0, sqrt(r));
    }
    vec2 h = a / r + vec2(1.0, 0.0);
    return h * sqrt(r / dot(h, h));
}
// complex cuberoot
vec2 ccbrt(vec2 a) {
    return cexp(cln(a)/3.0);
}

void cubic_roots(vec2 a, vec2 b, vec2 c, vec2 d, out vec2 x0, out vec2 x1, out vec2 x2) {
    // Cardano's formula for complex coefficients
    // TODO handle a=0 correctly
    vec2 ac = cmul(a, c);
    vec2 bb = cmul(b, b);
    vec2 aa = cmul(a, a);
    vec2 d0 = bb - 3.0 * ac;
    vec2 d1 = 2.0 * cmul(b, bb) - 9.0 * cmul(ac, b) + 27.0 * cmul(aa, d);
    vec2 s = csqrt(cmul(d1, d1) - 4.0 * cmul(cmul(d0, d0), d0));
    vec2 opta = d1 - s;
    vec2 optb = d1 + s;
    vec2 opt = dot(opta,opta) < dot(optb,optb) ? optb : opta;
    vec2 cb = ccbrt(opt * 0.5);
    x0 = cdiv(b + cb + cdiv(d0, cb), -3.0 * a);
    vec2 root = vec2(-0.5, 0.866025403784439);
    cb = cmul(cb, root);
    x1 = cdiv(b + cb + cdiv(d0, cb), -3.0 * a);
    cb = cmul(cb, root);
    x2 = cdiv(b + cb + cdiv(d0, cb), -3.0 * a);
}
vec2 cubic(vec2 a, vec2 b, vec2 c, vec2 d, vec2 x) {
    return cmul(cmul(cmul(a, x) + b, x) + c, x) + d;
}
vec2 quadratic(vec2 a, vec2 b, vec2 c, vec2 x) {
    return cmul(cmul(a, x) + b, x) + c;
}
vec2 selectx(vec2 x0, vec2 x1) {
    return abs(x0.y) < abs(x1.y) ? x0 : x1;
}
float newton_quintic(float a, float b, float c, float d, float e, float f, float x0) {
    float v = ((((a * x0 + b) * x0 + c) * x0 + d) * x0 + e) * x0 + f;
    float dv = (((5.0 * a * x0 + 4.0 * b) * x0 + 3.0 * c) * x0 + 2.0 * d) * x0 + e;
    float ddv = ((20.0 * a * x0 + 12.0 * b) * x0 + 6.0 * c) * x0 + 2.0 * d;
    float p = dv / ddv;
    float q = v / ddv * 2.0;
    float dx = p - sqrt(max(p * p - q, 0.0)) * sign(p);
    return x0 - dx;
    //float vdv = v / dv;
    //return x0 - vdv * (1.0 + ddv * vdv / dv);
}
float newton_bezier(float a, float b, float c, float d, float e, float f, float x0) {
    x0 = clamp(x0, 0.0, 1.0);
    for (int i = 0; i < ITERATIONS; i++) {
        x0 = clamp(newton_quintic(a, b, c, d, e, f, x0), 0.0, 1.0);
    }
    return x0;
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 uv = (fragCoord - iResolution.xy*0.5)/iResolution.x * 2.0;
    vec2 mouse = (iMouse.xy - iResolution.xy*0.5)/iResolution.x * 2.0;
    // cubic bezier params from https://www.shadertoy.com/view/4sKyzW
	float t0 = mod(iTime*2.+1.5,24.*PI);
    float scale = 2.0;
	vec2 p0 = scale * vec2(-cos(t0 * 1./2.) * .2,sin(t0 * 1./3.) * .25);
	vec2 p1 = scale * vec2(-cos(t0 * 2./3.) * .2,sin(t0 * 1./4.) * .1);
	vec2 p2 = scale * vec2(cos(t0 * 1./4.) * .1,-sin(t0 * 2./3.) * .2);
	vec2 p3 = scale * vec2(cos(t0 * 1./3.) * .25,-sin(t0 * 1./2.) * .1);
    if (iMouse.z > 0.0) {
        p1 = mouse;
    }

    // test cubic roots
    vec2 a = p3 - p0 + 3.0 * (p1 - p2);
    vec2 b = 3.0 * p0 - 6.0 * p1 + 3.0 * p2;
    vec2 c = -3.0 * p0 + 3.0 * p1;
    vec2 d = p0 - uv;
    float qa = 3.0 * dot(a, a), qb = 5.0 * dot(a, b), qc = 2.0 * dot(b, b) + 4.0 * dot(a, c), qd = 3.0 * dot(c, b) + 3.0 * dot(a, d), qe = 2.0 * dot(b, d) + dot(c, c), qf = dot(c, d);
    vec2 x0, x1, x2;
    cubic_roots(a, b, c, d, x0, x1, x2);
    // limit to range 0-1
    vec2 x = vec2(0.0, length(p0 - uv));
    x0.x = newton_bezier(qa, qb, qc, qd, qe, qf, x0.x);
    x0.y = length(cubic(a, b, c, d, vec2(x0.x, 0.0)));
    x = selectx(x, x0);
    x1.x = newton_bezier(qa, qb, qc, qd, qe, qf, x1.x);
    x1.y = length(cubic(a, b, c, d, vec2(x1.x, 0.0)));
    x = selectx(x, x1);
    x2.x = newton_bezier(qa, qb, qc, qd, qe, qf, x2.x);
    x2.y = length(cubic(a, b, c, d, vec2(x2.x, 0.0)));
    x = selectx(x, x2);
    float dp3 = length(p3 - uv);
    if (x.y > dp3) {
        x.x = 1.0;
        x.y = dp3;
    }
    float t = x.x;
    vec2 pos = cubic(a, b, c, d, vec2(t, 0.0));
    vec2 tangent = quadratic(3.0 * a, 2.0 * b, c, vec2(t, 0.0));
    float dist = length(pos);
    // TODO improve stability of sign! The cancellaton of signs makes it unstable at cusps.
    float sgn0 = sign(cmul(conj(tangent), pos).y);
    // flip sign if the pos is in a self intersection loop
    vec2 p = -inverse(mat2(a, b)) * c;
    float q = p.y*p.y - p.x;
    float sgn = sgn0 * ((q > 0.0) && (q-p.y+1.0 > 0.0) ? sign(t*t - p.y * t + q) : 1.0);
    float d0 = abs(dist);
    
	//iq's sd color scheme
	vec3 col = vec3(0.0) - sgn*vec3(0.1,0.4,0.7);
	col = vec3(0.0) - sgn*vec3(0.1,0.4,0.7);
	col = vec3(.89, .4, 0.1);
    
    //Champ Ligne
	//col *= 1.0 - exp(-8.0 * abs(d0)*0.5);
    //Aspect bandes
	//col *= 0.8 + 0.2*cos(480.0*0.5*d0);
    vec4 clearColor = vec4(.0, .0, 0.0, 0.0);
    vec4 lineColor = vec4(t, t, t, 1.0);
	//col = mix(clearColor, lineColor, 1.0-smoothstep(0.04,0.05,abs(d0)*1.8));
    
    //fragColor = vec4(col, 1.0);
    
    // Change the line thickness
    float lineSize = clamp(t, 0.3, 1.0);
    lineSize = mix(0.6, 1.0, sqrt(smoothstep(0.0,1.0,t)));
    
    // Change the color along the line
    // TODO Add color gradient
    fragColor = mix(clearColor, lineColor, 1.0-smoothstep(0.046,0.05,abs(d0)*(1.f/lineSize)));
    
    bool showControlPoints = true;
    if(showControlPoints)// Colorisation des points de controles 
    {
        fragColor += vec4(vec3(0.0,0.0,1.0) * max(0.0, 3.0 - length(p0 - uv) / 0.01), 1.0);
        fragColor += vec4(vec3(0.0,1.0,0.0) * max(0.0, 3.0 - length(p1 - uv) / 0.01), 1.0);
        fragColor += vec4(vec3(1.0,0.0,0.0) * max(0.0, 3.0 - length(p2 - uv) / 0.01), 1.0);
        fragColor += vec4(vec3(0.0,0.0,1.0) * max(0.0, 3.0 - length(p3 - uv) / 0.01), 1.0);
    }

}