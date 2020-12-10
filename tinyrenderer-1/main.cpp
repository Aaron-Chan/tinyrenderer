#include <vector>
#include <cmath>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
Model *model = NULL;
const int width  = 800;
const int height = 800;

void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
    // 1首先确定最长的维度  并交换
    bool is_steep = false;
    if (std::abs(x0-x1) < std::abs(y1-y0)){
        std::swap(x0,y0);
        std::swap(x1,y1);
        is_steep = true;
    }   
    // 2确定起始点
    if(x0>x1){
        std::swap(x0,x1);
        std::swap(y0,y1);
    }
    // 3按照最长维度开始递进，如果dy超过了0.5就需要加1或者减1
    int dx = x1- x0;
    int dy = y1 - y0;
    float dy_changed = 0;
    // float step = std::abs(dy/(float)dx);
    int step = std::abs(dy)*2;
    int y = y0;
    // 优化 将float 换成int 提高运行效率

    for(int x=x0;x<=x1;x++){
        dy_changed +=step;
        if (is_steep){
            image.set(y, x, color);
        }
        else
        {
            image.set(x, y, color);
        }
        if (dy_changed>dx){
        
            y+=dy>0?1:-1;
            dy_changed -=dx*2;
        }
        // if (dy_changed>=0.5){
        
        //     y+=dy>0?1:-1;
        //     dy_changed -=1;
        // }

    }

}

int main(int argc, char** argv) {
    if (2==argc) {
        model = new Model(argv[1]);
    } else {
        model = new Model("obj/african_head.obj");
    }

    TGAImage image(width, height, TGAImage::RGB);
    for (int i=0; i<model->nfaces(); i++) {
        std::vector<int> face = model->face(i);
        for (int j=0; j<3; j++) {
            Vec3f v0 = model->vert(face[j]);
            Vec3f v1 = model->vert(face[(j+1)%3]);
            int x0 = (v0.x+1.)*width/2.;
            int y0 = (v0.y+1.)*height/2.;
            int x1 = (v1.x+1.)*width/2.;
            int y1 = (v1.y+1.)*height/2.;
            line(x0, y0, x1, y1, image, white);
        }
    }

    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");
    delete model;
    return 0;
}

