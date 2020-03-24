// CORE.cpp : 定义 DLL 的导出函数。

#include "CORE.h"
#include <string>
#include <iostream>
using namespace std;

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
	if (ano->type == ShapeType::SEGMENT) {
		orig = line_inter_line(this, ano);
		if (orig.count == 0) {
			Segment* s = (Segment*)ano;
			if ((this->x1 == s->x1) && (this->y1 == s->y1)) {
				orig.count = 1;
				Point p = *(new Point(s->x1, s->y1));
				orig.p1 = p;
			}
			else if ((this->x1 == s->x2) && (this->y1 == s->y2)) {
				orig.count = 1;
				Point p = *(new Point(s->x2, s->y2));
				orig.p1 = p;
			}
			else if ((this->x2 == s->x1) && (this->y2 == s->y1)) {
				orig.count = 1;
				Point p = *(new Point(s->x1, s->y1));
				orig.p1 = p;
			}
			else if ((this->x2 == s->x2) && (this->y2 == s->y2)) {
				orig.count = 1;
				Point p = *(new Point(s->x2, s->y2));
				orig.p1 = p;
			}
		}
		orig = segment_constraint(ano, orig);
	}
	if (ano->type == ShapeType::RADIAL) {
		orig = line_inter_line(this, ano);
		if (orig.count == 0) {
			Radial* r = (Radial*)ano;
			orig.count = 1;
			Point p = *(new Point(r->x1, r->y1));
			orig.p1 = p;
		}
		orig = radial_constraint(ano, orig);
	}
	if (ano->type == ShapeType::CIRCLE) orig = line_inter_circle(this, ano);
	return segment_constraint(this, orig);
}
/****************from Pan******************/
RetPoints Radial::intersect(Shape* ano) {
	RetPoints orig = RetPoints{ 0, ERRORP, ERRORP };
	if (ano->type == ShapeType::LINE) orig = line_inter_line(this, ano);
	if (ano->type == ShapeType::SEGMENT) {
		orig = line_inter_line(this, ano);
		if (orig.count == 0) {
			Radial* r = (Radial*)this;
			orig.count = 1;
			Point p = *(new Point(r->x1, r->y1));
			orig.p1 = p;
		}
		orig = segment_constraint(ano, orig);
	}
	if (ano->type == ShapeType::RADIAL) {
		orig = line_inter_line(this, ano);
		if (orig.count == 0) {
			Radial* r = (Radial*)this;
			orig.count = 1;
			Point p = *(new Point(r->x1, r->y1));
			orig.p1 = p;
		}
		orig = radial_constraint(ano, orig);
	}
	if (ano->type == ShapeType::CIRCLE) orig = line_inter_circle(this, ano);
	return radial_constraint(this, orig);
}

inline char* decide_char(char* s, char* c) {
	for (; *s == ' '; s++);
	if(*s=='\0') {
		throw("1:Input char does not conform to four formats");
	}
	if ((*s != 'L') && (*s != 'C') && (*s != 'S') && (*s != 'R')) {
		throw("1:Input char does not conform to four formats");
	}
	else {
		s++;
		*c = *(s - 1);
		return s;
	}
	s++;
	if (*s != ' ') {
		throw("1:Input char does not conform to four formats");
	}
}

inline char* decide_int(char* s) {
	for (; *s == ' '; s++);
	if (*s == '\0') {
		throw("2:Fewer inputs");
	}
	if (*s == '-') s++;
	if (*s == '0') {
		if ((*(s + 1) != ' ') && (*(s + 1) != '\0')) {
			throw("3:Leading 0");
		}
	}
	while (true) {
		if (*s == ' ' || *s == '\0') {
			break;
		}
		else if ((*s > '9') || (*s < '0')) {
			throw("4:The input number does not meet the requirements");
			s++;
			break;
		}
		else s++;
	}
	return s;
}

inline void decide_more(char* s) {
	for (; *s == ' '; s++);
	if (*s != '\0') {
		throw("5:More inputs");
		return;
	}
}

void ShapeList::add_shape(char* line) {
	char* decide = line;
	Shape* shape;
	char c1;
	int a1, a2, a3, a4;
	decide = decide_char(decide, &c1);
	decide = decide_int(decide);
	decide = decide_int(decide);
	decide = decide_int(decide);
	if (c1 != 'C') {
		decide = decide_int(decide);
	}
	decide_more(decide);
	if (c1 == 'C') {
		sscanf_s(line, "%c %d %d %d", &c1, 1, &a1, &a2, &a3);
	}
	else {
		sscanf_s(line, "%c %d %d %d %d", &c1, 1, &a1, &a2, &a3, &a4);
	}
	
	if ((a1 >= 100000) || (a1 <= -100000) || (a2 >= 100000) || (a2 <= -100000) || (a3 >= 100000) || (a3 <= -100000)) {
		throw( "4:The input number does not meet the requirements");
	}
	if (c1 != 'C') {
		if ((a4 >= 100000) || (a4 <= -100000)) {
			throw("4:The input number does not meet the requirements");
		}
	}
	if (c1 == 'L')
	{
		if ((a1 == a3) && (a2 == a4)) {
			throw("6:Two point superposition");
		}
		shape = new Line(a1, a2, a3, a4);
	}
	else if (c1 == 'C')
	{
		if (a3 == 0) {
			throw("7:Circle radius is 0");
		}
		shape = new Circle(a1, a2, a3);
	}
	else if (c1 == 'S')
	{
		if ((a1 == a3) && (a2 == a4)) {
			throw("6:Two point superposition");
		}
		shape = new Segment(a1, a2, a3, a4);
	}
	else if (c1 == 'R')
	{
		if ((a1 == a3) && (a2 == a4)) {
			throw("6:Two point superposition");
		}
		shape = new Radial(a1, a2, a3, a4);
	}
	else {
		shape = new Radial(a1, a2, a3, a4);
	}
	if (c1 == 'C') {
		for (Shape* sh : this->shapelist) {
			if (sh->type == ShapeType::CIRCLE) {
				Circle* c = (Circle*)sh;
				if ((a1 == c->x_coeff) && (a2 == c->y_coeff) && (a3 == c->r_coeff)) {
					throw("8:Infinite intersections");
				}
			}
		}
	}
	else {
		int A, B, C;
		Point p1 = *(new Point(a1, a2));
		Point p2 = *(new Point(a3, a4));
		for (Shape* sh : this->shapelist) {
			if (sh->type == ShapeType::LINE) {
				Line* l = (Line*)sh;
				A = l->x_coeff;
				B = l->y_coeff;
				C = l->c_coeff;
			}
			else if (sh->type == ShapeType::SEGMENT) {
				Segment* s = (Segment*)sh;
				Point p3 = *(new Point(s->x1, s->y1));
				Point p4 = *(new Point(s->x2, s->y2));
				bool flag1, flag2, flag3, flag4;
				if (c1 == 'S') {
					flag1 = on_segment(p1, s);
					flag2 = on_segment(p2, s);
					flag3 = on_segment(p3, shape);
					flag4 = on_segment(p4, shape);
					if ((flag1 == false) && (flag2 == false) && (flag3 == false) && (flag4 == false)) {
						continue;
					}
					else {
						if ((flag1 == true) && (flag2 == false)) {
							if ((flag3 == true) && (flag4 == false)) {
								if (p1 == p3) continue;
							}
							else if((flag4 == true) && (flag3 == false)) {
								if (p1 == p4) continue;
							}
						}
						if ((flag2 == true) && (flag1 == false)) {
							if ((flag3 == true) && (flag4 == false)) {
								if (p2 == p3) continue;
							}
							else if ((flag4 == true) && (flag3 == false)) {
								if (p2 == p4) continue;
							}
						}
					}
				}
				else if (c1 == 'R') {
					flag1 = on_radial(p3, shape);
					flag2 = on_radial(p4, shape);
					if ((flag1 == false) && (flag2 == false)) {
						continue;
					}
					else if ((flag1 == true) && (flag2 == true));
					else {
						if (flag1 == true) {
							if ((p3 == p1)) continue;
						}
						else {
							if ((p4 == p1)) continue;
						}
					}
				}
				A = s->x_coeff;
				B = s->y_coeff;
				C = s->c_coeff;
			}
			else if (sh->type == ShapeType::RADIAL) {
				Radial* r = (Radial*)sh;
				Point p3 = *(new Point(r->x1, r->y1));
				bool flag1, flag2;
				if (c1 == 'S') {
					flag1 = on_radial(p1, r);
					flag2 = on_radial(p2, r);
					if ((flag1 == false) && (flag2 == false)) {
						continue;
					}
					else if ((flag1 == true) && (flag2 == true));
					else {
						if (flag1 == true) {
							if ((p3 == p1)) continue;
						}
						else {
							if ((p3 == p2)) continue;
						}
					}
				}
				else if (c1 == 'R') {
					if (p1 == p3) {
						if (on_radial(p2, r) == false) {
							continue;
						}
					}
				}
				A = r->x_coeff;
				B = r->y_coeff;
				C = r->c_coeff;
			}
			else {
				continue;
			}
			if ((A * a1 + B * a2 + C == 0) && (A * a3 + B * a4 + C == 0)) {
				throw("8:Infinite intersections");
			}
		}
	}
	this->shapelist.push_back(shape);
}

void ShapeList::delete_shape(char* line) {
	vector<Shape*>::iterator sh;
	char c1;
	int a1, a2, a3, a4;
	bool flag;
	sscanf_s(line, "%c %d %d %d %d", &c1, 1, &a1, &a2, &a3, &a4);
	if (c1 == 'C') {
		for (sh = this->shapelist.begin(); sh != this->shapelist.end(); sh++) {
			if ((*sh)->type == ShapeType::CIRCLE) {
				Circle* c = (Circle*)(*sh);
				if ((a1 == c->x_coeff) && (a2 == c->y_coeff) && (a3 == c->r_coeff)) {
					break;
				}
			}
		}
	}
	else if (c1 == 'L') {
		for (sh = this->shapelist.begin(); sh != this->shapelist.end(); sh++) {
			if ((*sh)->type == ShapeType::LINE) {
				Line* c = (Line*)(*sh);
				int A, B, C;
				A = c->x_coeff;
				B = c->y_coeff;
				C = c->c_coeff;
				if ((A * a1 + B * a2 + C == 0) && (A * a3 + B * a4 + C == 0)) {
					break;
				}
			}
		}
	}
	else if (c1 == 'S') {
		for (sh = this->shapelist.begin(); sh != this->shapelist.end(); sh++) {
			if ((*sh)->type == ShapeType::SEGMENT) {
				Segment* c = (Segment*)(*sh);
				int A, B, C;
				A = c->x_coeff;
				B = c->y_coeff;
				C = c->c_coeff;
				if ((A * a1 + B * a2 + C == 0) && (A * a3 + B * a4 + C == 0)) {
					break;
				}
			}
		}
	}
	else if (c1 == 'R') {
		for (sh = this->shapelist.begin(); sh != this->shapelist.end(); sh++) {
			if ((*sh)->type == ShapeType::RADIAL) {
				Radial* c = (Radial*)(*sh);
				int A, B, C;
				A = c->x_coeff;
				B = c->y_coeff;
				C = c->c_coeff;
				if ((A * a1 + B * a2 + C == 0) && (A * a3 + B * a4 + C == 0)) {
					break;
				}
			}
		}
	}
	if (sh == this->shapelist.end()) {
		throw("9:Deleted element is empty");
	}
	else {
		this->shapelist.erase(sh);
	}
}

vector<Shape*> ShapeList::get_shapelist() {
	return this->shapelist;
}