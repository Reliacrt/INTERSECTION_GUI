// CORE.cpp : 定义 DLL 的导出函数。

#include "CORE.h"
#include <string>
#include <iostream>
using namespace std;

// 这是导出变量的一个示例
CORE_API int nCORE=0;

// 这是导出函数的一个示例。
CORE_API int fnCORE(void)
{
    return 0;
}

// 这是已导出类的构造函数。
CCORE::CCORE()
{
    return;
}

#define SQUA(x) (x)*(x)
#define SSUM(x, y) SQUA(x)+SQUA(y)
#define ERRORP Point(-100000000.0, -100000000.0)

inline RetPoints line_inter_line(Shape* s1, Shape* s2)
{
	double x, y;
	Line* l1 = (Line*)s1;
	Line* l2 = (Line*)s2;
	auto a1 = l1->x_coeff, a2 = l2->x_coeff;
	auto b1 = l1->y_coeff, b2 = l2->y_coeff;
	auto c1 = l1->c_coeff, c2 = l2->c_coeff;
	if (a1 * b2 - a2 * b1 != 0)
	{
		x = ((double)c2 * b1 - (double)c1 * b2) / ((double)a1 * b2 - (double)a2 * b1);
		y = ((double)c1 * a2 - (double)c2 * a1) / ((double)a1 * b2 - (double)a2 * b1);
		return RetPoints{ 1, Point(x, y), ERRORP};
	}
	return RetPoints{ 0, ERRORP, ERRORP};
}

inline RetPoints line_inter_circle(Shape* s1, Shape* s2)
{
	Line* l = (Line*)s1;
	Circle* c = (Circle*)s2;
	auto A = l->x_coeff, B = l->y_coeff, C = l->c_coeff;
	auto x0 = c->x_coeff, y0 = c->y_coeff, r = c->r_coeff;
	const int ssum_a_b = SSUM(A, B);
	const double squa_ssum = sqrt(ssum_a_b);
	double d = abs(A * x0 + B * y0 + C) / squa_ssum;
	if (d > r) return RetPoints{ 0, ERRORP, ERRORP};
	double x_d = ((double)-A * C + (double)B * B * x0 - (double)A * B * y0) / ssum_a_b;
	double y_d = ((double)-B * C - (double)A * B * x0 + (double)A * A * y0) / ssum_a_b;
	if (r > d) // 相交于两点
	{
		double ano_d = sqrt((double)r * r - d * d);
		double vec_x = 1.0 * (-B) / squa_ssum;
		double vec_y = 1.0 * (A) / squa_ssum;
		return RetPoints{
			2,
			Point(x_d + vec_x * ano_d, y_d + vec_y * ano_d),
			Point(x_d - vec_x * ano_d, y_d - vec_y * ano_d)
		};
	}
	if (r == d) // 相切于一点
	{
		return RetPoints{ 1, Point(x_d, y_d), ERRORP };
	}
	return RetPoints{ 0, ERRORP, ERRORP};
}

inline RetPoints circle_inter_circle(Shape* s1, Shape* s2)
{
	Circle* c1 = (Circle*)s1;
	Circle* c2 = (Circle*)s2;
	auto x1 = c1->x_coeff, y1 = c1->y_coeff, r1 = c1->r_coeff;
	auto x2 = c2->x_coeff, y2 = c2->y_coeff, r2 = c2->r_coeff;
	auto d_2 = SSUM(x2 - x1, y2 - y1);
	auto r_r_2 = SQUA(r1 + r2);
	if (r_r_2 >= d_2)
	{
		Shape* line = new Line(
			2 * (x2 - x1), 
			2 * (y2 - y1), 
			x1 * x1 - x2 * x2 + y1 * y1 - y2 * y2 - r1 * r1 + r2 * r2);
		return line_inter_circle(line, s1);
	}
	return RetPoints{ 0, ERRORP, ERRORP};
}

Line::Line(int x1, int y1, int x2, int y2)
{
	this->type = ShapeType::LINE;
	this->x_coeff = y2 - y1;
	this->y_coeff = x1 - x2;
	this->c_coeff = x2 * y1 - x1 * y2;
}

RetPoints Line::intersect(Shape* ano)
{
	if (ano->type == ShapeType::LINE) return line_inter_line(this, ano);
	if (ano->type == ShapeType::CIRCLE) return line_inter_circle(this, ano);
	/****************from Pan******************/
	if (ano->type == ShapeType::SEGMENT) return segment_constraint(ano, line_inter_line(this, ano));
	if (ano->type == ShapeType::RADIAL) return radial_constraint(ano, line_inter_line(this, ano));
	return RetPoints{ 0, ERRORP, ERRORP};
}

Circle::Circle(int x, int y, int r)
{
	this->type = ShapeType::CIRCLE;
	this->x_coeff = x;
	this->y_coeff = y;
	this->r_coeff = r;
}

RetPoints Circle::intersect(Shape* ano)
{
	if (ano->type == ShapeType::LINE) return line_inter_circle(ano, this);
	if (ano->type == ShapeType::CIRCLE) return circle_inter_circle(ano, this);
	/****************from Pan******************/
	if (ano->type == ShapeType::SEGMENT) return segment_constraint(ano, line_inter_circle(ano, this));
	if (ano->type == ShapeType::RADIAL) return radial_constraint(ano, line_inter_circle(ano, this));
	return RetPoints{ 0, ERRORP, ERRORP};
}

Line::Line(int x_coeff, int y_coeff, int c_coeff)
{
	this->type = ShapeType::LINE;
	this->x_coeff = x_coeff;
	this->y_coeff = y_coeff;
	this->c_coeff = c_coeff;
}

CORE_API ostream& operator<<(ostream& os, const Point& ano)
{
	os << "(" << ano.x << ", " << ano.y << ")";
	return os;
}
/****************from Pan******************/
Radial::Radial(int x1, int y1, int x2, int y2) {
	this->type = ShapeType::RADIAL;
	this->x_coeff = y2 - y1;
	this->y_coeff = x1 - x2;
	this->c_coeff = x2 * y1 - x1 * y2;
	this->x1 = x1;
	this->y1 = y1;
	this->x2 = x2;
	this->y2 = y2;
}
/****************from Pan******************/
Segment::Segment(int x1, int y1, int x2, int y2) {
	this->type = ShapeType::SEGMENT;
	this->x_coeff = y2 - y1;
	this->y_coeff = x1 - x2;
	this->c_coeff = x2 * y1 - x1 * y2;
	this->x1 = x1;
	this->y1 = y1;
	this->x2 = x2;
	this->y2 = y2;
}
/****************from Pan******************/
inline bool on_radial(Point p, Shape* s) {
	Radial* r = (Radial*)s;
	if (((r->x1) <= (r->x2)) && ((r->y1) <= (r->y2)) && ((r->x1) <= (p.x)) && ((r->y1) <= (p.y))) return true;
	else if(((r->x1) <= (r->x2)) && ((r->y1) >= (r->y2)) && ((r->x1) <= (p.x)) && ((r->y1) >= (p.y))) return true;
	else if(((r->x1) >= (r->x2)) && ((r->y1) <= (r->y2)) && ((r->x1) >= (p.x)) && ((r->y1) <= (p.y))) return true;
	else if(((r->x1) >= (r->x2)) && ((r->y1) >= (r->y2)) && ((r->x1) >= (p.x)) && ((r->y1) >= (p.y))) return true;
	else return false;
}
/****************from Pan******************/
inline bool on_segment(Point p, Shape* s) {
	Segment* seg = (Segment*)s;
	int x_max, x_min;
	int y_max, y_min;
	if ((seg->x1) < (seg->x2)) {
		x_max = seg->x2;
		x_min = seg->x1;
	}
	else {
		x_max = seg->x1;
		x_min = seg->x2;
	}
	if ((seg->y1) < (seg->y2)) {
		y_max = seg->y2;
		y_min = seg->y1;
	}
	else {
		y_max = seg->y1;
		y_min = seg->y2;
	}
	if ((p.x <= x_max) && (p.x >= x_min) && (p.y <= y_max) && (p.y >= y_min)) {
		return true;
	}
	else {
		return false;
	}
}
/****************from Pan******************/
inline RetPoints segment_constraint(Shape* s, RetPoints orig)
{
	if (orig.count == 0) {
		return orig;
	}
	else if (orig.count == 1) {
		if (on_segment(orig.p1,s) == false) {
			orig.count = 0;
			orig.p1 = ERRORP;
		}
		return orig;
	}
	else {
		if (on_segment(orig.p1, s) == false) {
			if (on_segment(orig.p2, s) == false) {
				orig.count = 0;
				orig.p1 = ERRORP;
				orig.p2 = ERRORP;
			}
			else {
				orig.count = 1;
				orig.p1 = orig.p2;
				orig.p2 = ERRORP;
			}
			return orig;
		}
		else {
			if (on_segment(orig.p2, s) == false) {
				orig.count = 1;
				orig.p2 = ERRORP;
			}
			return orig;
		}
	}
}
/****************from Pan******************/
inline RetPoints radial_constraint(Shape* s, RetPoints orig)
{
	if (orig.count == 0) {
		return orig;
	}
	else if (orig.count == 1) {
		if (on_radial(orig.p1, s) == false) {
			orig.count = 0;
			orig.p1 = ERRORP;
		}
		return orig;
	}
	else {
		if (on_radial(orig.p1, s) == false) {
			if (on_radial(orig.p2, s) == false) {
				orig.count = 0;
				orig.p1 = ERRORP;
				orig.p2 = ERRORP;
			}
			/*
			else {
				orig.count = 1;
				orig.p1 = orig.p2;
				orig.p2 = ERRORP;
			}*/
			//不可能出现
			return orig;
		}
		else {
			if (on_radial(orig.p2, s) == false) {
				orig.count = 1;
				orig.p2 = ERRORP;
			}
			return orig;
		}
	}
}

RetPoints Segment::intersect(Shape* ano) {
	RetPoints orig = RetPoints{ 0, ERRORP, ERRORP};
	if (ano->type == ShapeType::LINE) orig = line_inter_line(this, ano);
	if (ano->type == ShapeType::SEGMENT) orig = segment_constraint(ano, line_inter_line(this, ano));
	if (ano->type == ShapeType::RADIAL) orig = radial_constraint(ano, line_inter_line(this, ano));
	if (ano->type == ShapeType::CIRCLE) orig = line_inter_circle(this, ano);
	return segment_constraint(this, orig);
}
/****************from Pan******************/
RetPoints Radial::intersect(Shape* ano) {
	RetPoints orig = RetPoints{ 0, ERRORP, ERRORP };
	if (ano->type == ShapeType::LINE) orig = line_inter_line(this, ano);
	if (ano->type == ShapeType::SEGMENT) orig = segment_constraint(ano, line_inter_line(this, ano));
	if (ano->type == ShapeType::RADIAL) orig = radial_constraint(ano, line_inter_line(this, ano));
	if (ano->type == ShapeType::CIRCLE) orig = line_inter_circle(this, ano);
	return radial_constraint(this, orig);
}

