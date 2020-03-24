#include <iostream>
#include <string>
#include <set>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include "../CORE/CORE.h"
#include "iup.h"
#include "iupdraw.h"
#include "cd_plus.h"
#include "cd.h"
#include "cdprint.h"
#include "cdiup.h"

using std::string;
using std::cout;
using std::cerr;
using std::endl;
using std::set;

static char* text_str;
static ShapeList shape_list;
static set<Point> points;
static double mx = 0., my = 0.;

FILE* input;
FILE* output;

inline size_t count(vector<Shape*>& shapes)
{
	auto n = shapes.size();
	// n ^ 2 遍历
	for (size_t i = 0; i < n - 1; i++)
	{
		for (size_t j = i + 1; j < n; j++)
		{
			auto ret = shapes[i]->intersect(shapes[j]);
			if (ret.count == 1) {
				points.insert(ret.p1);
			}
			else if (ret.count == 2) {
				points.insert(ret.p1);
				points.insert(ret.p2);
			}
		}
	}
	return points.size();
}

/// 重定向输入输出
void arg_parse_one_item(char* arg, char* val)
{
	string argument(arg);
	string value(val);
	//printf("arg: %s; val: %s\n", arg, val);
	if (argument == "-i")
	{
		freopen_s(&input, val, "r", stdin);
	}
	if (argument == "-o")
	{
		freopen_s(&output, val, "w", stdout);
	}
}

/// 参数解析
void arg_parse(int argc, char* argv[])
{
	argv += 1;
	while (argc >= 3)
	{
		argc -= 2;
		arg_parse_one_item(argv[0], argv[1]);
		argv += 2;
	}
	int n;
	char line[50] = "\0";
	gets_s(line);
	sscanf_s(line, "%d", &n);
	for (int i = 0; i < n; i++)
	{
		gets_s(line);
		try
		{
			shape_list.add_shape(line);
		}
		catch (const char* msg)
		{
			cerr << msg << endl;
		}
	}
	printf("%d\n", count(shape_list.shapelist));
}

void flush_shape(void)
{
	count(shape_list.shapelist);
	mx = 0., my = 0.;
	for (auto&& p : points)
	{
		if (abs(p.x) > mx) mx = abs(p.x);
		if (abs(p.y) > my) my = abs(p.y);
	}
}

#define R 10.0

inline double unit(int w, int h, double mx, double my)
{
	double xu = (w / 2.0) / (mx + 5);
	double yu = (h / 2.0) / (my + 5);
	return xu < yu ? xu : yu;
}

inline double trans(double x, int w, double un)
{
	return w / 2.0 + x * un;
}

int update_canvas(Ihandle* canvas)
{
	int w, h;
	cdCanvas* cd_canvas = cdCreateCanvas(CD_NATIVEWINDOW, IupGetAttribute(canvas, "WID"));

	cdCanvasActivate(cd_canvas);
	cdCanvasClear(cd_canvas);

	IupGetIntInt(canvas, "DRAWSIZE", &w, &h);
	std::cout << w << ", " << h << std::endl;
	cdCanvasForeground(cd_canvas, CD_BLACK);
	cdCanvasLineWidth(cd_canvas, 1);
	cdCanvasLineStyle(cd_canvas, CD_CONTINUOUS);
	cdfCanvasLine(cd_canvas, 0, h / 2, w, h / 2);
	cdfCanvasLine(cd_canvas, w / 2, 0, w / 2, h);

	flush_shape();
	auto un = unit(w, h, mx, my);
	for (auto&& shape : shape_list.shapelist)
	{
		Line* l = (Line*)shape;
		Circle* c = (Circle*)shape;
		Segment* s = (Segment*)shape;
		Radial* r = (Radial*)shape;
		double x_left, y_left, x_right, y_right;
		switch (shape->type)
		{
		case ShapeType::LINE:
			if (l->y_coeff != 0)
			{
				x_left = -(w / 2) / un - 100, x_right = (w / 2) / un + 100;
				y_left = -(l->x_coeff * x_left + l->c_coeff) / l->y_coeff;
				y_right = -(l->x_coeff * x_right + l->c_coeff) / l->y_coeff;
			}
			else
			{
				y_left = -(h / 2) / un - 100, y_right = (h / 2) / un + 100;
				x_left = -(l->y_coeff * y_left + l->c_coeff) / l->x_coeff;
				x_right = -(l->y_coeff * y_right + l->c_coeff) / l->x_coeff;
			}
			cdfCanvasLine(cd_canvas, trans(x_left, w, un), trans(y_left, h, un), trans(x_right, w, un), trans(y_right, h, un));
			break;
		case ShapeType::CIRCLE:
			cdfCanvasArc(cd_canvas, trans(c->x_coeff, w, un), trans(c->y_coeff, h, un), 2 * c->r_coeff * un, 2 * c->r_coeff * un, 0., 360.);
			break;
		case ShapeType::SEGMENT:
			cdfCanvasLine(cd_canvas, trans(s->x1, w, un), trans(s->y1, h, un), trans(s->x2, w, un), trans(s->y2, h, un));
			break;
		case ShapeType::RADIAL:
			if (l->y_coeff != 0)
			{
				x_left = -(w / 2) / un - 100, x_right = (w / 2) / un + 100;
				y_left = -(l->x_coeff * x_left + l->c_coeff) / l->y_coeff;
				y_right = -(l->x_coeff * x_right + l->c_coeff) / l->y_coeff;
			}
			else
			{
				y_left = -(h / 2) / un - 100, y_right = (h / 2) / un + 100;
				x_left = -(l->y_coeff * y_left + l->c_coeff) / l->x_coeff;
				x_right = -(l->y_coeff * y_right + l->c_coeff) / l->x_coeff;
			}
			if (s->x2 > s->x1)
				cdfCanvasLine(cd_canvas, trans(s->x1, w, un), trans(s->y1, h, un), trans(x_right, w, un), trans(y_right, h, un));
			else
				cdfCanvasLine(cd_canvas, trans(s->x1, w, un), trans(s->y1, h, un), trans(x_left, w, un), trans(y_left, h, un));
			break;
		default:
			break;
		}
	}
	for (auto&& p : points)
	{
		auto _x = trans(p.x, w, un);
		auto _y = trans(p.y, h, un);
		cout << p << endl;
		cout << "(" << _x << ", " << _y << ")" << endl;
		cdfCanvasSector(cd_canvas, _x, _y, 7, 7, 0., 360.);
	}

	cdCanvasFlush(cd_canvas);

	return IUP_DEFAULT;
}

int text_cb(Ihandle* self)
{
	text_str = IupGetAttribute(self, "VALUE");
	//std::cout << text_str << std::endl;
	return IUP_DEFAULT;
}

int bt_insert_cb(Ihandle* self)
{
	if (text_str == NULL) return IUP_DEFAULT;
	try
	{
		shape_list.add_shape(text_str);
	}
	catch (const char* msg)
	{
		cerr << msg << endl;
	}
	Ihandle* canvas = IupGetDialogChild(self, "CANVAS");
	update_canvas(canvas);
	return IUP_DEFAULT;
}

int bt_delete_cb(Ihandle* self)
{
	if (text_str == NULL) return IUP_DEFAULT;
	try
	{
		shape_list.delete_shape(text_str);
	}
	catch (const char* msg)
	{
		cerr << msg << endl;
	}
	Ihandle* canvas = IupGetDialogChild(self, "CANVAS");
	update_canvas(canvas);
	return IUP_DEFAULT;
}


int main(int argc, char** argv)
{
	arg_parse(argc, argv);
	Ihandle* dlg, * button_insert, * button_delete, * text, * canvas, * hbox, * vbox;
	IupOpen(&argc, &argv);

	button_insert = IupButton("Insert", NULL);
	button_delete = IupButton("Delete", NULL);
	IupSetAttribute(button_insert, "SIZE", "60");
	IupSetAttribute(button_delete, "SIZE", "60");

	text = IupText(NULL);
	IupSetAttribute(text, "SIZE", "300");
	IupSetAttribute(text, "NAME", "TEXT");
	IupSetAttribute(text, "NC", "1024");

	hbox = IupHbox(IupFill(), text, button_insert, button_delete, IupFill(), NULL);
	IupSetAttribute(hbox, "MARGIN", "10x10");
	IupSetAttribute(hbox, "GAP", "10");

	canvas = IupCanvas(NULL);
	IupSetAttribute(canvas, "NAME", "CANVAS");
	IupSetAttribute(canvas, "EXPAND", "YES");

	vbox = IupVbox(hbox, canvas, NULL);
	IupSetAttribute(vbox, "MARGIN", "10x10");
	IupSetAttribute(vbox, "GAP", "10");

	dlg = IupDialog(vbox);
	IupSetAttribute(dlg, "TITLE", "Intersection_GUI");
	IupSetAttribute(dlg, "SIZE", "1000x360");

	/* Registers callbacks */
	IupSetCallback(text, "VALUECHANGED_CB", (Icallback)text_cb);
	IupSetCallback(canvas, "ACTION", (Icallback)update_canvas);
	IupSetCallback(button_insert, "ACTION", (Icallback)bt_insert_cb);
	IupSetCallback(button_delete, "ACTION", (Icallback)bt_delete_cb);


	IupShowXY(dlg, IUP_CENTER, IUP_CENTER);

	IupMainLoop();

	IupClose();
	return EXIT_SUCCESS;
}
