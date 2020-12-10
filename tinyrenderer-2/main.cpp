#include <vector>
#include <cmath>
#include <iostream>
#include "tgaimage.h"
#include "geometry.h"
#include <algorithm>

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const TGAColor green = TGAColor(0,   255, 0,   255);
const int width  = 200;
const int height = 200;

void line(vec2 p0, vec2 p1, TGAImage &image, TGAColor color) {
    bool steep = false;
    if (std::abs(p0.x-p1.x)<std::abs(p0.y-p1.y)) {
        std::swap(p0.x, p0.y);
        std::swap(p1.x, p1.y);
        steep = true;
    }
    if (p0.x>p1.x) {
        std::swap(p0, p1);
    }

    for (int x=p0.x; x<=p1.x; x++) {
        float t = (x-p0.x)/(float)(p1.x-p0.x);
        int y = p0.y*(1.-t) + p1.y*t;
        if (steep) {
            image.set(y, x, color);
        } else {
            image.set(x, y, color);
        }
    }
}

void triangle(vec2 t0, vec2 t1, vec2 t2, TGAImage &image, TGAColor color) {
    // line(t0, t1, image, color);
    // line(t1, t2, image, color);
    // line(t2, t0, image, color);
     // 按照y进行排序
    // 不断递增y并对三角形的左右两边进行划线

    if(t0.y>t1.y){
        std::swap(t0,t1);   
    }
    if(t0.y>t2.y){
        std::swap(t0,t2);   
    }
    if(t1.y>t2.y){
        std::swap(t1,t2);   
    }
    int total = t2.y-t0.y;
    int segment_1 = t1.y-t0.y;
    int segment_2 = t2.y-t1.y;
    for(int y=t0.y;y<=t2.y;y++){
        int segment = y>t1.y ? segment_2:segment_1;
        int base =  y>t1.y ? t1.y:t0.y;
        vec2 base_v =  y>t1.y ? t1:t0;
        vec2 top_v =  y>t1.y ? t2:t1;
        float a = (y-base)/(float)segment;
        float b = (y-t0.y)/(float)total;
        vec2 A = base_v+( top_v-base_v)*a;
        vec2 B = t0+( t2-t0)*b;

        if (A.x>B.x) std::swap(A, B); 
        for (int j=A.x; j<=B.x; j++) { 
            image.set(j, y, color); // attention, due to int casts t0.y+i != A.y 
        } 
    }
}

vec3 barycentric(vec2 *pts, vec2& v){
    // ab ac  pa
    vec2 ab = pts[1] - pts[0];
    vec2 ac = pts[2] - pts[0];
    vec2 pa = pts[0] - v;
    vec3 result = cross(vec3(ab.x,ac.x,pa.x),vec3(ab.y,ac.y,pa.y));
    if (std::abs(result.z) < 1)
    {
        return vec3(-1,1,1);
    }
    result = vec3(1.f-(result.x+result.y)/result.z,result.x/result.z,result.y/result.z);
    return result;
}


bool in_triangle(vec2 *pts, vec2& v){
    vec3 result = barycentric(pts, v);
    return result.x >0 && result.y > 0 && result.z >0;
}

void triangle_2(vec2 t0, vec2 t1, vec2 t2, TGAImage &image, TGAColor color) {
    //    计算boundingbox 
    // 遍历范围内的x 和y  判断是否在三角形范围内
    vec2 pts[3] = {t0,t1,t2};
    vec2 max_bouding = vec2(0,0);
    vec2 min_bouding = vec2(image.get_width()-1,image.get_height()-1);
    vec2 clamp(image.get_width()-1, image.get_height()-1); 
    for(int i=0;i<3;i++){
        for(int j=0;j<2;j++){
            max_bouding[j] = std::min(clamp[j], std::max(max_bouding[j], pts[i][j]));
            min_bouding[j] = std::max(0., std::min(min_bouding[j], pts[i][j]));
        }
    }
    std::cout << "max_bouding " <<max_bouding<<"min_bouding"<<min_bouding<<std::endl;
    vec2 p;
    for (p.x=min_bouding.x;p.x<max_bouding.x;p.x++){
        for (p.y=min_bouding.y;p.y<max_bouding.y;p.y++){
            if(in_triangle(pts, p)){
                image.set(p.x, p.y, color);
            }
        }   
    }



}

int main(int argc, char** argv) {
    TGAImage image(width, height, TGAImage::RGB);

    vec2 t0[3] = {vec2(10, 70),   vec2(50, 160),  vec2(70, 80)};
    vec2 t1[3] = {vec2(180, 50),  vec2(150, 1),   vec2(70, 180)};
    vec2 t2[3] = {vec2(180, 150), vec2(120, 160), vec2(130, 180)};

    triangle_2(t0[0], t0[1], t0[2], image, red);
    triangle_2(t1[0], t1[1], t1[2], image, white);
    triangle_2(t2[0], t2[1], t2[2], image, green);


    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");
    return 0;
}

