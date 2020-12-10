#include <vector>
#include <cmath>
#include <iostream>
#include "tgaimage.h"
#include "geometry.h"
#include <algorithm>
#include "model.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const TGAColor green = TGAColor(0,   255, 0,   255);
const int width  = 400;
const int height = 400;

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


vec3 barycentric(vec3 *pts, vec3& v){
    // ab ac  pa
    vec3 ab = pts[1] - pts[0];
    vec3 ac = pts[2] - pts[0];
    vec3 pa = pts[0] - v;
    vec3 result = cross(vec3(ab.x,ac.x,pa.x),vec3(ab.y,ac.y,pa.y));
    if (std::abs(result.z) < 1)
    {
        return vec3(-1,1,1);
    }
    result = vec3(1.f-(result.x+result.y)/result.z,result.x/result.z,result.y/result.z);
    return result;
}



void triangle(vec3 t0, vec3 t1, vec3 t2, TGAImage &image, TGAColor color, float* z_buffer) {
    //    计算boundingbox 
    // 遍历范围内的x 和y  判断是否在三角形范围内
    vec3 pts[3] = {t0,t1,t2};
    vec2 max_bouding = vec2(0,0);
    vec2 min_bouding = vec2(image.get_width()-1,image.get_height()-1);
    vec2 clamp(image.get_width()-1, image.get_height()-1); 
    for(int i=0;i<3;i++){
        for(int j=0;j<2;j++){
            min_bouding[j] = std::max(0., std::min(min_bouding[j], pts[i][j]));
            max_bouding[j] = std::min(clamp[j], std::max(max_bouding[j], pts[i][j]));
        }
    }
    std::cout << "max_bouding " <<max_bouding<<"min_bouding"<<min_bouding<<std::endl;
    vec3 p;
    for (p.x=min_bouding.x;p.x<=max_bouding.x;p.x++){
        for (p.y=min_bouding.y;p.y<=max_bouding.y;p.y++){
            vec3 barycentric_coord = barycentric(pts, p);
            if (barycentric_coord.x <0 || barycentric_coord.y<0 || barycentric_coord.z < 0){
                continue;
            }
            p.z = 0;
            p.z = barycentric_coord.x * pts[0][2] + barycentric_coord.y * pts[1][2]  +barycentric_coord.z * pts[2][2] ;
            int idx = p.x + p.y * image.get_width();
            if(z_buffer[idx] < p.z){
                z_buffer[idx] = p.z;
                image.set(p.x, p.y, color);
            }
        }   
    }


}

vec3 worldNDCToScreenCoord(vec3 world_ndc, int sc_width, int sc_height){
    int x = (world_ndc.x + 1.) * sc_width/2.;
    int y = (world_ndc.y + 1.) * sc_height/2.;
    world_ndc.x = x;
    world_ndc.y = y;
    return world_ndc;
}


void drawModelWithLight(int argc, char** argv){
    Model *model = NULL;    
    if (2==argc) {
        model = new Model(argv[1]);
    } else {
        model = new Model("obj/african_head.obj");
    }
    TGAImage image(width, height, TGAImage::RGB);
    vec3 light_dir = vec3(0,0,-1.);
    int z_buffer_size = height * width;
    
    float *z_buffer  = new float[z_buffer_size];
    for(int i=0;i<z_buffer_size;i++){
        z_buffer[i] = -std::numeric_limits<float>::max();
    }

    for (int i=0; i<model->nfaces(); i++) {
        std::vector<int> face = model->face(i);
        std::vector<vec3> screen_coords = {};
        std::vector<vec3> world_coords_list = {};
        for (int j=0; j<3; j++) {
            vec3 world_coords  = model->vert(face[j]);
            vec3 screen_coord = worldNDCToScreenCoord(world_coords, width, height);
            screen_coords.push_back(screen_coord);
            world_coords_list.push_back(world_coords);
            
        }
        vec3 normal = cross(world_coords_list[2] - world_coords_list[0],world_coords_list[1]-world_coords_list[0]);
        normal = normal.normalize();
        double n = normal * light_dir;
        if (n>0)
        {
            TGAColor color = TGAColor(n * 255, n * 255,   n * 255,   255);
            triangle(screen_coords[0], screen_coords[1], screen_coords[2], image, color, z_buffer);
        }        

    }

    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output_model_with_light.tga");
    delete z_buffer;
    delete model;
}




int main(int argc, char** argv) {
    drawModelWithLight(argc, argv);
    return 0;
}

