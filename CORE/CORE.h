// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 CORE_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// CORE_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#ifdef CORE_EXPORTS
#define CORE_API __declspec(dllexport)
#else
#define CORE_API __declspec(dllimport)
#endif

#include <utility>
#include <cmath>
#include <vector>
#include <ostream>
// 此类是从 dll 导出的
class CORE_API CCORE {
public:
	CCORE(void);
	// TODO: 在此处添加方法。
};

extern CORE_API int nCORE;

CORE_API int fnCORE(void);

using std::pair;
using std::vector;
using std::ostream;

#define EPS 1E-10

inline bool double_eq(double a, double b)
{
	return abs(a - b) <= EPS;
}

/****************from Zhang******************/
struct CORE_API Point
{
	double x;
	double y;
	
	Point(double x, double y)
	{
		this->x = x;
		this->y = y;
	}

	bool operator>(const Point& ano) const
	{
		return x > ano.x || (double_eq(x, ano.x) && y > ano.y);
	}
	bool operator<(const Point& ano) const
	{
		return x < ano.x || (double_eq(x, ano.x) && y < ano.y);
	}
	bool operator==(const Point& ano) const
	{
		return double_eq(x, ano.x) && double_eq(y, ano.y);
	}

	CORE_API friend ostream& operator<<(ostream& os, const Point& ano);
};

struct CORE_API RetPoints
{
	int count;
	Point p1;
	Point p2;
};

enum class CORE_API ShapeType
{
	/****************from Pan******************/
	NONE, POINT, LINE, CIRCLE, SEGMENT, RADIAL
};

struct CORE_API Shape {
	ShapeType type = ShapeType::NONE;
	virtual RetPoints intersect(Shape* ano) = 0;
};

struct CORE_API Line : public Shape {
	int x_coeff;
	int y_coeff;
	int c_coeff;
	Line(int x1, int y1, int x2, int y2);
	Line(int x_coeff, int y_coeff, int c_coeff);

	bool operator==(const Line& ano) const;

	RetPoints intersect(Shape* ano);
};

struct CORE_API Circle : public Shape {
	int x_coeff;
	int y_coeff;
	int r_coeff;
	Circle(int x, int y, int r);
	RetPoints intersect(Shape* ano);
};

/****************from Pan******************/
struct CORE_API Segment : public Shape {
	int x_coeff;
	int y_coeff;
	int c_coeff;
	int x1;
	int y1;
	int x2;
	int y2;
	Segment(int x1, int y1, int x2, int y2);
	RetPoints intersect(Shape* ano);
};
/****************from Pan******************/
struct CORE_API Radial : public Shape {
	int x_coeff;
	int y_coeff;
	int c_coeff;
	int x1;
	int y1;
	int x2;
	int y2;
	Radial(int x1, int y1, int x2, int y2);
	RetPoints intersect(Shape* ano);
};

struct CORE_API ShapeList {
	vector<Shape*> shapelist;
	void add_shape(char* line);
	void delete_shape(char* line);
	vector<Shape*> get_shapelist();
};

/****************from Pan******************/
inline char* decide_char(char* s, char* c);
inline char* decide_int(char* s);
inline void decide_more(char* s);
inline bool on_radial(Point p, Shape* s);
inline bool on_segment(Point p, Shape* s);
/****************from Zhang******************/
inline RetPoints line_inter_line(Shape* s1, Shape* s2);
inline RetPoints line_inter_circle(Shape* s1, Shape* s2);
inline RetPoints circle_inter_circle(Shape* s1, Shape* s2);
inline RetPoints segment_constraint(Shape* s, RetPoints orig);
inline RetPoints radial_constraint(Shape* s, RetPoints orig);