#version 430 core

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


// SDF Blend Flags
#define GROUP           1
#define SUBTRACT        2


out vec4 FragColor;

const float _PI = 3.14159;


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
    
    return  add(d, shape); 
}

// -------------------------
// OPTIONS
// -------------------------

bool outerGlow = false;
bool innerGlow = false;



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

void main() 
{

    float rmin = min(uFrameRes.x, uFrameRes.y);
    float u_blur = 2.0;
//    float w = u_blur*5.0/r;


    vec2 Position = (2.0*gl_FragCoord.xy-uFrameRes.xy)/rmin;
    vec2 p = Position;

    // Background color
//    vec4 col = vec4(0);
    vec4 col = vec4(0.0);

    int loop = 0;

    float d  = 1;

    for(int i = 0; i < uPrimSize; i++)
    {

        UIPrimitive e = primitives[i];

        transformPixelSpace(e);

        if((e.blendMode & GROUP) == GROUP)
            loop ^= 1; //switch group mode

        p = (e.rotation > 0.0) ? rotate(p, e.rotation): p;

        if(e.shapeId == SDCIRCLE     ){d = composeSdf(e.blendMode, d, sdCircle(p- e.pos, e.size.r));}
        if(e.shapeId == SDORIENTEDBOX){d = composeSdf(e.blendMode, d, sdOrientedBox  (p, e.pos, e.pos2, e.size.r));}
        if(e.shapeId == SDROUNDEDDBOX){d = composeSdf(e.blendMode, d, sdRoundedBox  (p- e.pos, e.size, e.pos2, e.pos3));}
        if(e.shapeId == SDSEGMENT    ){d = composeSdf(e.blendMode, d, sdSegment      (p, e.pos, e.pos2, e.size.r));}
        if(e.shapeId == SDPIE        ){d = composeSdf(e.blendMode, d, sdPie(p, e.progress, e.rotation, e.size.r));}
        if(e.shapeId == SDRING       ){d = composeSdf(e.blendMode, d, sdParamRing(p, e.progress, e.rotation, e.size));}
        if(e.shapeId == SDARC        ){d = composeSdf(e.blendMode, d, sdArc(p, uProgress, e.rotation, e.size));}
        if(e.shapeId == SDTRIANGLE   ){d = composeSdf(e.blendMode, d, sdTriangle(p, e.pos, e.pos2, e.pos3));}
        if(e.shapeId == SDROUNEDX    ){d = composeSdf(e.blendMode, d, sdRoundedX(p, e.size.r, e.cornerRadius));}
        if(e.shapeId == SDCROSS      ){d = composeSdf(e.blendMode, d, sdCross(p, e.size, e.cornerRadius));}
        if(e.shapeId == SDPENTAGON   ){d = composeSdf(e.blendMode, d,sdPentagon(p, e.size.r));}
        if(e.shapeId == SDHEXAGON    ){d = composeSdf(e.blendMode, d,sdHexagon(p, e.size.r));}


        if(loop!=0) continue;

        d = (e.cornerRadius > 0) ?  d - e.cornerRadius : d;
        d = (e.innerRadius> 0) ? d = abs(d) - e.innerRadius : d;

        // Anti Aliasing
        float w = u_blur*fwidth(d);
        float h = smoothstep(-w/2.0, w/2.0, -d); // smooth centered blur AA
        

        // Alpha blending
        float alpha = e.color.a * h;
        vec4 src = vec4(e.color.xyz, alpha);
        col = over(col, src);
//        col = vec4(vec3(h), 1.0);

//        if(innerGlow)
//            col *= (d>0.0) ? 1.0 : 1.0 - exp(-6.0*abs(d));
//        if(outerGlow)
//            col *= (d>0.0) ? 1.0 - exp(-6.0*abs(d)): 1.0;

        // Border with AA
        if(e.borderWidth > 0) {
            d = borderCoverage(d, e.borderWidth);
            col = mix( vec4(e.border.xyz, 1.0), col, d);
        }

        d  = 1;
    }

    FragColor = col;
}