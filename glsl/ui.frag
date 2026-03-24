#version 430 core

#define _PI 3.14159265359
//in vec2     iResolution;    // Screen dimensions
//in float    uProgress;           // Float uProgress 
uniform int     uPrimSize;
uniform vec2    uFrameRes;
uniform float   uProgress;

in mat4 proj;

// Enum SDF_Shapes copy declared in..
#define SDCIRCLE        0
#define SDORIENTEDBOX   1
#define SDROUNDEDDBOX   2
#define SDSEGMENT       3
#define SDPIE           4
#define SDRING          5
#define SDARC           6
#define SDTRIANGLE      7
#define SDROUNEDX       8
#define SDCROSS         9
#define SDPENTAGON      10
#define SDHEXAGON       11
#define SDBEZIER_CUBIC  12


// SDF Blend Flags
#define GROUP           1
#define SUBTRACT        2
#define INTERSECTION    4


out vec4 FragColor;



struct UIPrimitive {
    vec2 pos;
    vec2 size;
    vec4 color;
    vec4 border;
    float borderWidth;
    float cornerRadius;
    int   shapeId;
    int   blendMode;
    float innerRadius;  // 4 bytes
    float progress;     // 4 bytes
    vec2   pos2;
    vec2   pos3;        // 8 bytes
    vec2   pos4;        // 8 bytes

    float rotation;     // 4 bytes
    float scale;        // 4 bytes
    float _pad;         // 4 bytes
    float _pad1;        // 4 bytes
};

layout(binding = 0, std430) readonly buffer Primitives {
    UIPrimitive primitives[]; // unsized array
};


void transformPixelSpace(inout UIPrimitive prim)
{
    float rmin  = min(uFrameRes.x, uFrameRes.y);
    vec2 factor = uFrameRes / rmin;

    prim.pos            /= uFrameRes   * 0.5 / factor;
    prim.pos2           /= uFrameRes   * 0.5 / factor;
    prim.pos3           /= uFrameRes   * 0.5 / factor;
    prim.pos4           /= uFrameRes   * 0.5 / factor;
    prim.size           /= uFrameRes   * 0.5 / factor;
    prim.borderWidth    /= uFrameRes.x * 0.5;
    prim.cornerRadius   /= uFrameRes.x * 0.5;
    prim.innerRadius    /= uFrameRes.x * 0.5;
}

// Transforms
vec2 rotate(const in vec2 samplePosition, const in float rotation){
    float angle     = rotation * _PI  * -1/180;
    float sine      = sin(angle);
    float cosine    = cos(angle);
    return vec2(cosine * samplePosition.x + sine * samplePosition.y, cosine * samplePosition.y - sine * samplePosition.x);
}

vec2 scale(const vec2 samplePosition, float scale){
    return samplePosition / scale;
}

vec2 translate(vec2 samplePosition, vec2 offset){
    return samplePosition - offset;
}


// Bezier
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

// Signed Distance Functions 


float sdOrientedBox( const in vec2 p, const in vec2 a, const in vec2 b, const in float th )
{
    float l = length(b-a);
    vec2  d = (b-a)/l;
    vec2  q = (p-(a+b)*0.5);
          q = mat2(d.x,-d.y,d.y,d.x)*q;
          q = abs(q)-vec2(l,th)*0.5;
    return length(max(q,0.0)) + min(max(q.x,q.y),0.0);    
}

float sdRoundedBox( in vec2 p, in vec2 b, in vec2 upper, in vec2 lower )
{
    vec4 r = vec4(upper, lower);
    r.xy = (p.x<0.0)?r.xz : r.yw;
    r.x  = (p.y>0.0)?r.x  : r.y;
    vec2 q = abs(p)-b+r.x;
    return min(max(q.x,q.y),0.0) + length(max(q,0.0)) - r.x;
}

float sdCircle(const in vec2 p, const in float r )
{
    return length(p) - r;
}

float sdSegment(const in vec2 p, const in vec2 a, const in vec2 b, const in float th )
{
    vec2 pa = p-a, ba = b-a;
    float h = clamp( dot(pa,ba)/dot(ba,ba), 0.0, 1.0 );
    return length( pa - ba*h ) - th/2.0;
}

float sdPie(vec2 p, in float t, in float rot,  const in float r )
{
    // -0.5 full, 0.5 empty, mapping to aperture 0->1
    p = rotate(p, 360*t / 2);
    float o = ((1 - clamp(t, 0,1)) - 0.5) * _PI;
    vec2 c = vec2( cos(o), sin(o));

    p.x = abs(p.x);
    float l = length(p) - r;
    float m = length(p-c*clamp(dot(p,c),0.0,r)); // c=sin/cos of aperture
    return max(l,m*sign(c.y*p.x-c.x*p.y));
}

// Ring
// p  : pixel
// n  : normal
// r  : radius
// th : thickness
float sdRing( in vec2 p, in vec2 n, in float r, float th )
{
    p.x = abs(p.x);
    p = mat2x2(n.x,n.y,-n.y,n.x)*p;
    return max( abs(length(p)-r)-th*0.5,
                length(vec2(p.x,max(0.0,abs(r-p.y)-th*0.5)))*sign(p.x) );
}

float sdParamRing( in vec2 p, in float t, in float rot, in vec2 size )
{
    p = rotate(p, (360*t / 2));
    return sdRing(p, vec2(cos(t*_PI), sin(t*_PI)),  size.r, size.g );
}

float sdArc( vec2 p, float t, float rot,  in vec2 r )
{
    p = rotate(p, (360*t / 2));
    float o = ((1 - clamp(t, 0,1)) - 0.5) * _PI;
    vec2 sc = vec2( cos(o), sin(o));

    p.x = abs(p.x);
    return ((sc.y*p.x>sc.x*p.y) ? length(p-sc*r.x) : 
                                  abs(length(p)-r.x)) - r.y;
}

float sdRoundedX( in vec2 p, in float w, in float r )
{
    p = abs(p);
    return length(p-min(p.x+p.y,w)*0.5) - r;
}

// Ring
// p  : pixel
// b  : (branch length, branche thickness)
// r  : rounding factor
float sdCross( in vec2 p, in vec2 b, float r ) 
{
    p = abs(p); p = (p.y>p.x) ? p.yx : p.xy;
    vec2  q = p - b;
    float k = max(q.y,q.x);
    vec2  w = (k>0.0) ? q : vec2(b.y-p.x,-k);
    return sign(k)*length(max(w,0.0)) + r;
}

float sdPentagon( in vec2 p, in float r )
{
    const vec3 k = vec3(0.809016994,0.587785252,0.726542528);
    p.x = abs(p.x);
    p -= 2.0*min(dot(vec2(-k.x,k.y),p),0.0)*vec2(-k.x,k.y);
    p -= 2.0*min(dot(vec2( k.x,k.y),p),0.0)*vec2( k.x,k.y);
    p -= vec2(clamp(p.x,-r*k.z,r*k.z),r);    
    return length(p)*sign(p.y);
}

float sdHexagon( in vec2 p, in float r )
{
    const vec3 k = vec3(-0.866025404,0.5,0.577350269);
    p = abs(p);
    p -= 2.0*min(dot(k.xy,p),0.0)*k.xy;
    p -= vec2(clamp(p.x, -k.z*r, k.z*r), r);
    return length(p)*sign(p.y);
}

// Triangle
// p  : pixel
// p0, p1, p2 : Position triangle corners
float sdTriangle( in vec2 p, in vec2 p0, in vec2 p1, in vec2 p2 )
{
    vec2 e0 = p1-p0, e1 = p2-p1, e2 = p0-p2;
    vec2 v0 = p -p0, v1 = p -p1, v2 = p -p2;
    vec2 pq0 = v0 - e0*clamp( dot(v0,e0)/dot(e0,e0), 0.0, 1.0 );
    vec2 pq1 = v1 - e1*clamp( dot(v1,e1)/dot(e1,e1), 0.0, 1.0 );
    vec2 pq2 = v2 - e2*clamp( dot(v2,e2)/dot(e2,e2), 0.0, 1.0 );
    float s = sign( e0.x*e2.y - e0.y*e2.x );
    vec2 d = min(min(vec2(dot(pq0,pq0), s*(v0.x*e0.y-v0.y*e0.x)),
                     vec2(dot(pq1,pq1), s*(v1.x*e1.y-v1.y*e1.x))),
                     vec2(dot(pq2,pq2), s*(v2.x*e2.y-v2.y*e2.x)));
    return -sqrt(d.x)*sign(d.y);
}



float sdBezierCubic(in vec2 p, inout float t, inout float d0, UIPrimitive e)
{

    // test cubic roots
    vec2 a = e.pos4 - e.pos + 3.0 * (e.pos2 - e.pos3);
    vec2 b = 3.0 * e.pos - 6.0 * e.pos2 + 3.0 * e.pos3;
    vec2 c = -3.0 * e.pos + 3.0 * e.pos2;
    vec2 d = e.pos - p;
    float qa = 3.0 * dot(a, a), qb = 5.0 * dot(a, b), qc = 2.0 * dot(b, b) + 4.0 * dot(a, c), qd = 3.0 * dot(c, b) + 3.0 * dot(a, d), qe = 2.0 * dot(b, d) + dot(c, c), qf = dot(c, d);
    vec2 x0, x1, x2;
    cubic_roots(a, b, c, d, x0, x1, x2);
    // limit to range 0-1
    vec2 x = vec2(0.0, length(e.pos - p));
    x0.x = newton_bezier(qa, qb, qc, qd, qe, qf, x0.x);
    x0.y = length(cubic(a, b, c, d, vec2(x0.x, 0.0)));
    x = selectx(x, x0);
    x1.x = newton_bezier(qa, qb, qc, qd, qe, qf, x1.x);
    x1.y = length(cubic(a, b, c, d, vec2(x1.x, 0.0)));
    x = selectx(x, x1);
    x2.x = newton_bezier(qa, qb, qc, qd, qe, qf, x2.x);
    x2.y = length(cubic(a, b, c, d, vec2(x2.x, 0.0)));
    x = selectx(x, x2);
    float dp3 = length(e.pos4 - p);
    if (x.y > dp3) {
        x.x = 1.0;
        x.y = dp3;
    }

    t = x.x;
    vec2 pos = cubic(a, b, c, d, vec2(t, 0.0));
    vec2 tangent = quadratic(3.0 * a, 2.0 * b, c, vec2(t, 0.0));
    float dist = length(pos);
    // TODO improve stability of sign! The cancellaton of signs makes it unstable at cusps.
    float sgn0 = sign(cmul(conj(tangent), pos).y);
    // flip sign if the pos is in a self intersection loop
    vec2 pSign = -inverse(mat2(a, b)) * c;
    float q = p.y*p.y - p.x;
    float sgn = sgn0 * ((q > 0.0) && (q-pSign.y+1.0 > 0.0) ? sign(t*t - p.y * t + q) : 1.0);
    d0 = abs(dist);
    
    return d0;
}

// Blend modes
float intersect(const in float shape1, const in float shape2){
    return max(shape1, shape2);
}

float subtract(const in float base, const  in float subtraction){
    return intersect(base, -subtraction);
}

float add(in float base, in float shape){
    return min(base, shape);
}

float composeSdf(in int blend, const in float d, const in float shape)
{
    //Flag

    if((blend & SUBTRACT) == SUBTRACT)
        return subtract(d, shape);

    if((blend & INTERSECTION) == INTERSECTION)
        return intersect(d, shape);
    
    return  add(d, shape); 
}

// -------------------------
// OPTIONS
// -------------------------

bool outerGlow = false;
bool innerGlow = false;





vec4 over(vec4 dst, vec4 src) {
    float outA = src.a + dst.a * (1.0 - src.a);
    vec3 outRGB = (src.rgb * src.a + dst.rgb * dst.a * (1.0 - src.a)) / max(outA, 1e-5);
    return vec4(outRGB, outA);
}

float borderCoverage(float dist, float thickness)
{
    float halfT = thickness * 0.5;
    float w = max(0, fwidth(dist));               // AA width
    float inner = smoothstep(-halfT - w, -halfT + w, dist);
    float outer = smoothstep(halfT - w, halfT + w, dist);

    return -clamp((inner - outer), 0.0, 1.0) +1 ;
}

//void main() 
//{
//    float rmin = min(uFrameRes.x, uFrameRes.y);
//    float u_blur = 2.0;
//
//    vec2 Position = 2.0f*vec2(gl_FragCoord.x, uFrameRes.y-gl_FragCoord.y)/rmin;
////    vec2 Position = (2.0*gl_FragCoord.xy-uFrameRes.xy)/rmin;
//    vec2 p = Position;
//
//    // Background color
//    vec4 col = vec4(0.0);
//
//    int loop = 0;
//
//    float d  = 1;
//
//    for(int i = 0; i < uPrimSize; i++)
//    {
//
//        UIPrimitive e = primitives[i];
//
//        transformPixelSpace(e);
//
//        if((e.blendMode & GROUP) == GROUP)
//            loop ^= 1; //switch group mode
//
//        p = (e.rotation > 0.0) ? rotate(p, e.rotation): p;
//
//        if(e.shapeId == SDCIRCLE      ){d = composeSdf(e.blendMode, d, sdCircle(p- e.pos, e.size.r));}
//        if(e.shapeId == SDORIENTEDBOX ){d = composeSdf(e.blendMode, d, sdOrientedBox  (p, e.pos, e.pos2, e.size.r));}
//        if(e.shapeId == SDROUNDEDDBOX ){d = composeSdf(e.blendMode, d, sdRoundedBox  (p- e.pos, e.size, e.pos2, e.pos3));}
//        if(e.shapeId == SDSEGMENT     ){d = composeSdf(e.blendMode, d, sdSegment      (p, e.pos, e.pos2, e.size.r));}
//        if(e.shapeId == SDPIE         ){d = composeSdf(e.blendMode, d, sdPie(p, e.progress, e.rotation, e.size.r));}
//        if(e.shapeId == SDRING        ){d = composeSdf(e.blendMode, d, sdParamRing(p, e.progress, e.rotation, e.size));}
//        if(e.shapeId == SDARC         ){d = composeSdf(e.blendMode, d, sdArc(p, uProgress, e.rotation, e.size));}
//        if(e.shapeId == SDTRIANGLE    ){d = composeSdf(e.blendMode, d, sdTriangle(p, e.pos, e.pos2, e.pos3));}
//        if(e.shapeId == SDROUNEDX     ){d = composeSdf(e.blendMode, d, sdRoundedX(p, e.size.r, e.cornerRadius));}
//        if(e.shapeId == SDCROSS       ){d = composeSdf(e.blendMode, d, sdCross(p, e.size, e.cornerRadius));}
//        if(e.shapeId == SDPENTAGON    ){d = composeSdf(e.blendMode, d,sdPentagon(p, e.size.r));}
//        if(e.shapeId == SDHEXAGON     ){d = composeSdf(e.blendMode, d,sdHexagon(p, e.size.r));}
//        if(e.shapeId == SDBEZIER_CUBIC){d = composeSdf(e.blendMode, d,sdBezierCubic(p, e.pos, e.pos2, e.pos3, e.pos4));}
//
//
//        if(loop!=0) continue;
//
//        d = (e.cornerRadius > 0) ?  d - e.cornerRadius : d;
//        d = (e.innerRadius> 0) ? d = abs(d) - e.innerRadius : d;
//
//        // Anti Aliasing
//        float w = u_blur*fwidth(d);
//        float h = smoothstep(-w/2.0, w/2.0, -d); // smooth centered blur AA
//        
//
//        // Alpha blending
//        float alpha = e.color.a * h;
//        vec4 src = vec4(e.color.xyz, alpha);
//        col = over(col, src);
//
//        // Border with AA
//        if(e.borderWidth > 0) {
//            d = borderCoverage(d, e.borderWidth);
//            col = mix( vec4(e.border.xyz, 1.0), col, d);
//        }
//
//        d  = 1;
//    }
//
//    FragColor = col;
//}
//


void main()
{
    float rmin = min(uFrameRes.x, uFrameRes.y);
    float u_blur = 2.0;

    vec2 Position = 2.0f*vec2(gl_FragCoord.x, uFrameRes.y-gl_FragCoord.y)/rmin;
//    vec2 Position = (2.0*gl_FragCoord.xy-uFrameRes.xy)/rmin;
//    vec2 p = Position;
//    vec2 uv = (gl_FragCoord.xy - uFrameRes*0.5) / uFrameRes.x * 2.0;
//    vec2 mouse = (iMouse.xy - uFrameRes.xy*0.5)/uFrameRes.x * 2.0;
    // cubic bezier params from https://www.shadertoy.com/view/4sKyzW

    UIPrimitive e;

    e.pos  = vec2(100, 100);
    e.pos2  = vec2(400, 200);
    e.pos3  = vec2(100, 400);
    e.pos4 = vec2(600, 600);

    transformPixelSpace(e);

//	float t0 = mod(iTime*2.+1.5,24.*PI);
    float scale = 2.0;
//	vec2 p0 = vec2(-0.4,-0.4);
//	vec2 p1 = vec2(-0.4,0.4);
//	vec2 p2 = vec2(0.4,-0.4);
//	vec2 p3 = vec2(0.4,0.4);


    vec2 p0 = e.pos;
	vec2 p1 = e.pos2;
	vec2 p2 = e.pos3;
	vec2 p3 = e.pos4;

    float t = 0;
    float d0 = 0;

    sdBezierCubic(Position, t, d0, e);




    vec4 clearColor = vec4(0.0, .0, 0.0, 0.0);
    vec4 lineColor = vec4(1, 1, 0, 1.0);
    vec4 lineColor2 = vec4(0, 1, 1, 1.0);
    vec4 lineColorMix = mix(lineColor, lineColor2, t);
	//col = mix(clearColor, lineColor, 1.0-smoothstep(0.04,0.05,abs(d0)*1.8));
    
    //fragColor = vec4(col, 1.0);
    
    // Change the line thickness
    float lineSize = clamp(t, 1.0, 1.0);
//    lineSize = mix(0.6, 1.0, sqrt(smoothstep(0.0,1.0,t)));
    
    // Change the color along the line
    // TODO Add color gradient
    float alpha = 1.0 - smoothstep(0.046, 0.05, abs(d0)*(1.0/lineSize));

    FragColor = vec4(lineColorMix.rgb, alpha);
}