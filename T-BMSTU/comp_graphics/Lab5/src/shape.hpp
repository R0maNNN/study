#pragma once

#include <iostream>
#include <vector>

struct Point {

    int x, y, z;

    bool operator==(const Point &p) {

        return ((this->x == p.x) && (this->y == p.y) && (this->z == p.z));
    }

    bool operator!=(const Point &p) {

        return ((this->x != p.x) || (this->y != p.y) || (this->z != p.z));
    }
};

class Shape3D {

protected:
    int nel = 0;
    int ratio = 1;

public:
    int sectors = 10, sections = 5;
    std::vector<float> verts;

    float* getArray() {

        return &verts[0];
    }

    int getNel() {

        return nel;
    }

    virtual void render() = 0;
    virtual bool cross_border(const Point &p) const = 0;
    virtual void changeFragm(int sectors, int sections) = 0;
    virtual void calculateVert() {

        verts.clear();
    }

    virtual ~Shape3D() {
    }
};

class Shape2D : public Shape3D {

public:
    enum Plane {
        XY, YZ, XZ
    };

    Plane plane = Plane::XY;
    int pos = 0;
};

class Square : public Shape2D {

protected:
    Point p1, p2, p3, p4;

public:
    Square(Plane plane, int pos, int ratio) {

        this->ratio = ratio;
        this->pos = pos;
        this->plane = plane;
    }

    void render() override {

        glEnableClientState (GL_VERTEX_ARRAY);

        glVertexPointer(3, GL_FLOAT, 0, getArray());
        glDrawArrays(GL_QUADS, 0, getNel());

        glDisableClientState(GL_VERTEX_ARRAY);
    }

    bool cross_border(const Point &p) const {

        switch (plane) {
        case Plane::XY:
            return ((p.z == pos / ratio)
                    && (((p.x > p1.x) && (p.x < p2.x))
                            || ((p.x < p1.x) && (p.x > p2.x)))
                    && (((p.y > p2.y) && (p.y < p3.y))
                            || ((p.y < p2.y) && (p.y > p3.y))));
        case Plane::YZ:
            return ((p.x == pos / ratio)
                    && (((p.y > p1.y) && (p.y < p2.y))
                            || ((p.y < p1.y) && (p.y > p2.y)))
                    && (((p.z > p2.z) && (p.z < p3.z))
                            || ((p.z < p2.z) && (p.z > p3.z))));
        case Plane::XZ:
            return ((p.y == pos / ratio)
                    && (((p.x > p1.x) && (p.x < p2.x))
                            || ((p.x < p1.x) && (p.x > p2.x)))
                    && (((p.z > p2.z) && (p.z < p3.z))
                            || ((p.z < p2.z) && (p.z > p3.z))));
        }

        return false;
    }

    void changeFragm(int sectors, int sections) override {

        this->sections = sections;
        this->sectors = sectors;
    }

    void calculateVert() override {

        Shape3D::calculateVert();
        int a, b, c, *d = &a, *e = &b;
        switch (plane) {
        case Plane::XY:
            c = pos;
            d = &a;
            e = &b;
            break;
        case Plane::YZ:
            a = pos;
            d = &b;
            e = &c;
            break;
        case Plane::XZ:
            b = pos;
            d = &a;
            e = &c;
            break;
        }

        *d = 100;
        *e = 100;
        p1 = {a / ratio, b / ratio, c / ratio};
        verts.push_back(static_cast<float>(p1.x) / 100.0);
        verts.push_back(static_cast<float>(p1.y) / 100.0);
        verts.push_back(static_cast<float>(p1.z) / 100.0);

        *d = -100;
        p2 = {a / ratio, b / ratio, c / ratio};
        verts.push_back(static_cast<float>(p2.x) / 100.0);
        verts.push_back(static_cast<float>(p2.y) / 100.0);
        verts.push_back(static_cast<float>(p2.z) / 100.0);

        *e = -100;
        p3 = {a / ratio, b / ratio, c / ratio};
        verts.push_back(static_cast<float>(p3.x) / 100.0);
        verts.push_back(static_cast<float>(p3.y) / 100.0);
        verts.push_back(static_cast<float>(p3.z) / 100.0);

        *d = 100;
        p4 = {a / ratio, b / ratio, c / ratio};
        verts.push_back(static_cast<float>(p4.x) / 100.0);
        verts.push_back(static_cast<float>(p4.y) / 100.0);
        verts.push_back(static_cast<float>(p4.z) / 100.0);

        nel = verts.size() / 3;
    }
};

class Cube : public Shape3D {
public:
    Square squ_bot, squ_top, squ_left, squ_right, squ_front, squ_back;

    Cube(int ratio) :
            squ_bot(Shape2D::XZ, -100, ratio), squ_top(Shape2D::XZ, 100, ratio),
                    squ_left(Shape2D::YZ, -100, ratio),
                    squ_right(Shape2D::YZ, 100, ratio),
                    squ_front(Shape2D::XY, -100, ratio),
                    squ_back(Shape2D::XY, 100, ratio) {
        this->ratio = ratio;
    }

    void render() override {

        squ_bot.render();
        squ_top.render();
        squ_left.render();
        squ_right.render();
        squ_front.render();
        squ_back.render();
    }

    bool cross_border(const Point &p) const {

        return (squ_bot.cross_border(p) || squ_top.cross_border(p)
                || squ_left.cross_border(p) || squ_right.cross_border(p)
                || squ_front.cross_border(p) || squ_back.cross_border(p));
    }

    void changeFragm(int sectors, int sections) override {

        this->sections = sections;
        this->sectors = sectors;

        squ_bot.changeFragm(sectors, sections);
        squ_top.changeFragm(sectors, sections);
        squ_left.changeFragm(sectors, sections);
        squ_right.changeFragm(sectors, sections);
        squ_back.changeFragm(sectors, sections);
        squ_front.changeFragm(sectors, sections);
    }

    void calculateVert() override {

        Shape3D::calculateVert();
        squ_bot.calculateVert();
        squ_top.calculateVert();
        squ_left.calculateVert();
        squ_right.calculateVert();
        squ_back.calculateVert();
        squ_front.calculateVert();

        nel = verts.size() / 3;
    }

    unsigned char getMask(Point p) const {

        unsigned char mask = 0;

        if (p.y < squ_bot.pos / ratio) {
            mask |= 0x1;
        } else if (p.y > squ_top.pos / ratio) {
            mask |= 0x2;
        }
        if (p.x < squ_left.pos / ratio) {
            mask |= 0x4;
        } else if (p.x > squ_right.pos / ratio) {
            mask |= 0x8;
        }
        if (p.z < squ_front.pos / ratio) {
            mask |= 0x10;
        } else if (p.z > squ_back.pos / ratio) {
            mask |= 0x20;
        }

        return mask;
    }
};

class Line3D {

public:
    Point p1, p2;

    Line3D(const Point &p1, const Point &p2) :
            p1(p1), p2(p2) {
    }

    Line3D() {

        do {
            p1.x = 100 - rand() / (RAND_MAX / 200);
            p1.y = 100 - rand() / (RAND_MAX / 200);
            p1.z = 100 - rand() / (RAND_MAX / 200);
            p2.x = 100 - rand() / (RAND_MAX / 200);
            p2.y = 100 - rand() / (RAND_MAX / 200);
            p2.z = 100 - rand() / (RAND_MAX / 200);
        } while (std::sqrt(
                (p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y)
                        + (p1.z - p2.z) * (p1.z - p2.z)) < 200);
    }

    void render() const {

        glBegin (GL_LINES);
        glVertex3f(static_cast<float>(p1.x) / 100.0,
                static_cast<float>(p1.y) / 100.0,
                static_cast<float>(p1.z) / 100.0);
        glVertex3f(static_cast<float>(p2.x) / 100.0,
                static_cast<float>(p2.y) / 100.0,
                static_cast<float>(p2.z) / 100.0);
        glEnd();
    }

    Line3D clip(const Cube &cube) {

        unsigned char p1_mask = cube.getMask(p1);
        unsigned char p2_mask = cube.getMask(p2);

        unsigned char cross_count = 0;
        std::vector<Point> p_arr;

        if (p1_mask) {
            if (p2_mask) {
                if (p1_mask == p2_mask) {
                    return Line3D( { 0, 0, 0 }, { 0, 0, 0 });
                } else {
                    cross_count = 2;
                }
            } else {
                cross_count = 1;
            }
        } else {
            if (p2_mask) {
                cross_count = 1;
            } else {
                return *this;
            }
        }

        clip_aux(cube, p_arr, cross_count);

        if (cross_count == 2) {
            if (p_arr.size()) {
                return Line3D(p_arr[0], p_arr[1]);
            } else {
                return Line3D( { 0, 0, 0 }, { 0, 0, 0 });
            }
        } else {
            return Line3D(p1, p_arr[0]);
        }
    }

    void clip_aux(const Cube &cube, std::vector<Point> &p_arr,
            const unsigned char cross_count) {

        unsigned char l_mask = cube.getMask(p1);
        unsigned char r_mask = cube.getMask(p2);

        if ((p_arr.size() == cross_count)
                || ((static_cast<unsigned char>(p_arr.size()) == cross_count - 1)
                        && (l_mask == r_mask))) {
            return;
        }

        int x_mid = (p1.x + p2.x + 2 * 100) / 2;
        int y_mid = (p1.y + p2.y + 2 * 100) / 2;
        int z_mid = (p1.z + p2.z + 2 * 100) / 2;

        int x_mid_mod = (p1.x + p2.x + 2 * 100) % 2;
        int y_mid_mod = (p1.y + p2.y + 2 * 100) % 2;
        int z_mid_mod = (p1.z + p2.z + 2 * 100) % 2;

        Point p_mid { x_mid - 100, y_mid - 100, z_mid - 100 };
        Point p_mid_r { x_mid + x_mid_mod - 100, y_mid + y_mid_mod - 100, z_mid
                + z_mid_mod - 100 };

        if (p1.x > p2.x)
            std::swap(p_mid.x, p_mid_r.x);
        if (p1.y > p2.y)
            std::swap(p_mid.y, p_mid_r.y);
        if (p1.z > p2.z)
            std::swap(p_mid.z, p_mid_r.z);

        if (cube.cross_border(p_mid)) {
            p_arr.push_back(p_mid);
        }

        if (p_arr.size() < cross_count && (p1 != p_mid)) {
            Line3D line_1(*const_cast<const Point *>(&p1), p_mid);
            line_1.clip_aux(cube, p_arr, cross_count);
        }

        if (p_arr.size() < cross_count && (p2 != p_mid_r)) {
            Line3D line_2(p_mid_r, *const_cast<const Point *>(&p2));
            line_2.clip_aux(cube, p_arr, cross_count);
        }
    }

};
