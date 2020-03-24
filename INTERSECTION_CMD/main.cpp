#include <iostream>
#include "../CORE/CORE.h"
#include <cstdio>
#include <set>
#include <cstdlib>
#include <ctime>

using namespace std;

FILE* input;
FILE* output;

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
}

inline size_t count(vector<Shape*>& shapes)
{
	set<Point> points;
	auto n = shapes.size();
	// n ^ 2 遍历
	for (size_t i = 0; i < n - 1; i++)
	{
		for (size_t j = i + 1; j < n; j++)
		{
			RetPoints ret = shapes[i]->intersect(shapes[j]);
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

#define RD(a, b) (rand()%((b)-(a)+1))+(a)
void performance(void)
{
	srand((unsigned int)time(NULL));
	const int n = 1000;
	vector<Shape*> vec;
	for (int i = 0; i < n; i++)
	{
		int a1 = RD(-100000, +100000);
		int a2 = RD(-100000, +100000);
		int a3 = RD(-100000, +100000);
		int a4 = RD(-100000, +100000);
		int r = RD(1, 100000);
		if (RD(0, 1) == 0)
		{
			vec.push_back(new Line(a1, a2, a3, a4));
		}
		else
		{
			vec.push_back(new Circle(a1, a2, r));
		}
	}
	printf("%zd", count(vec));
}

int main(int argc, char* argv[])
{
	vector<Shape*> shapes;
	int n;
	int a1, a2, a3, a4;
	char line[50] = "\0";
	new Circle(1, 1, 2);
	arg_parse(argc, argv);
	//performance();
	gets_s(line);
	sscanf_s(line, "%d", &n);
	for (int i = 0; i < n; i++)
	{
		gets_s(line);
		char c1;
		sscanf_s(line, "%c %d %d %d %d", &c1, 1, &a1, &a2, &a3, &a4);
		if (c1 == 'L')
		{
			shapes.push_back(new Line(a1, a2, a3, a4));
		}
		if (c1 == 'C')
		{
			shapes.push_back(new Circle(a1, a2, a3));
		}
		if (c1 == 'S')
		{
			shapes.push_back(new Segment(a1, a2, a3, a4));
		}
		if (c1 == 'R')
		{
			shapes.push_back(new Radial(a1, a2, a3, a4));
		}
	}
	printf("%zd", count(shapes));
	fclose(stdin);
	fclose(stdout);
}