#include "CppUnitTest.h"
#include "../CORE/CORE.h"
#include <iostream>
using namespace std;

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Microsoft
{
	namespace VisualStudio
	{
		namespace CppUnitTestFramework
		{
			template<> static std::wstring ToString<Point>(const Point& p)
			{
				std::wstringstream wss;
				wss << "(" << p.x << ", " << p.y << ")";
				return wss.str();
			}
		}
	}
}

namespace CORETEST
{
	TEST_CLASS(CORETEST)
	{
	public:
		TEST_METHOD(TestMethod1)
		{
			Shape* l1 = new Line(3, 0, 3, 2);
			Shape* l2 = new Line(1, 4, 5, 4);
			Shape* l3 = new Line(1, 1, 2, 3);
			Shape* l4 = new Line(2, 4, 2, 5);
			Assert::AreEqual(l1->intersect(l2).count, 1);
			Assert::AreEqual(l1->intersect(l2).p1, Point(3.0, 4.0));
			Assert::AreEqual(l1->intersect(l3).p1, Point(3.0, 5.0));
			Assert::AreEqual(l2->intersect(l3).p1, Point(2.5, 4.0));
			Assert::AreEqual(l1->intersect(l4).count, 0);
		}

		TEST_METHOD(TestMethod2)
		{
			Shape* l1 = new Line(1, 3, 3, 4);
			Shape* l2 = new Line(-1, 2, 4, 0);
			Shape* l3 = new Line(7, 5, 5, 7);
			Shape* l4 = new Line(-1, 1, -1, 6);
			Shape* c = new Circle(3, 2, 4);
			Assert::AreEqual(l1->intersect(c).count, 2);
			Assert::IsTrue(l1->intersect(c).p1 == Point(-1.0, 2.0)
				|| l1->intersect(c).p1 == Point(5.4, 5.2));
			Assert::AreEqual(l1->intersect(c).p2, l2->intersect(c).p2);
			Assert::AreEqual(l4->intersect(c).p1, Point(-1.0, 2.0));
			Assert::AreEqual(l3->intersect(c).count, 0);
		}

		TEST_METHOD(TestMethod3)
		{
			Shape* c1 = new Circle(3, 2, 4);
			Shape* c2 = new Circle(-1, 6, 4);
			Shape* c3 = new Circle(-5, 2, 4);
			Shape* c4 = new Circle(-8, 7, 3);

			Assert::AreEqual(c1->intersect(c2).count, 2);
			Assert::AreEqual(c1->intersect(c2).p1, Point(3.0, 6.0));
			Assert::AreEqual(c1->intersect(c2).p2, Point(-1.0, 2.0));

			Assert::AreEqual(c1->intersect(c3).count, 1);
			Assert::AreEqual(c1->intersect(c3).p1, Point(-1.0, 2.0));

			Assert::AreEqual(c1->intersect(c4).count, 0);
		}

		TEST_METHOD(TestMethod4)
		{
			Shape* c1 = new Circle(0, 0, 2);
			Shape* c2 = new Circle(3, 0, 1);
			Shape* s1 = new Segment(2, 0, 2, -4);
			Shape* s2 = new Segment(-1, 1, 1, 1);
			Shape* s3 = new Segment(-1, 1, -1, -2);
			Shape* r1 = new Radial(0, 0, 1, 0);
			Shape* r2 = new Radial(3, -1, 4, -1);
			Shape* r3 = new Radial(1, 1, 1, 4);

			Assert::AreEqual(c1->intersect(c2).count, 1);
			Assert::AreEqual(c1->intersect(c2).p1, Point(2.0, 0.0));

			Assert::AreEqual(c1->intersect(s1).count, 1);
			Assert::AreEqual(c1->intersect(s1).p1, Point(2.0, 0.0));
			
			Assert::AreEqual(c1->intersect(r1).count, 1);
			Assert::AreEqual(c1->intersect(r1).p1, Point(2.0, 0.0));

			Assert::AreEqual(c2->intersect(s1).count, 1);
			Assert::AreEqual(c2->intersect(s1).p1, Point(2.0, 0.0));

			Assert::AreEqual(c2->intersect(r1).count, 2);
			Assert::AreEqual(c2->intersect(r1).p1, Point(4.0, 0.0));
			Assert::AreEqual(c2->intersect(r1).p2, Point(2.0, 0.0));

			Assert::AreEqual(r1->intersect(s1).count, 1);
			Assert::AreEqual(r1->intersect(s1).p1, Point(2.0, 0.0));
			
			Assert::AreEqual(c1->intersect(s2).count, 0);

			Assert::AreEqual(c1->intersect(r2).count, 0);

			Assert::AreEqual(c1->intersect(s3).count, 1);

			Assert::AreEqual(c1->intersect(r3).count, 1);

			Assert::AreEqual(c1->intersect(s3).count, 1);

			Assert::AreEqual(c1->intersect(r3).count, 1);
		}

		TEST_METHOD(TestMethod5)
		{
			Shape* s1 = new Segment(-5, -1, -3, -7);
			Shape* s2 = new Segment(4, 2, 4, -7);
			Shape* r1 = new Radial(-11, -4, 2, -4);
			Shape* r2 = new Radial(0, 0, -1, -1);

			Assert::AreEqual(s1->intersect(s2).count, 0);

			Assert::AreEqual(s1->intersect(r1).count, 1);
			Assert::AreEqual(s1->intersect(r1).p1, Point(-4.0, -4.0));

			Assert::AreEqual(s1->intersect(r2).count, 1);
			Assert::AreEqual(s1->intersect(r2).p1, Point(-4.0, -4.0));

			Assert::AreEqual(s2->intersect(r1).count, 1);
			Assert::AreEqual(s2->intersect(r1).p1, Point(4.0, -4.0));

			Assert::AreEqual(s2->intersect(r2).count, 0);

			Assert::AreEqual(r1->intersect(r2).count, 1);
			Assert::AreEqual(r1->intersect(r2).p1, Point(-4.0, -4.0));
		}

		TEST_METHOD(TestMethod6)
		{
			Shape* l1 = new Line(0, 0, 2, 2);
			Shape* s1 = new Segment(0, -2, 2, 0);
			Shape* r1 = new Radial(2, 10, 2, 0);
			Shape* r2 = new Radial(0, 0, 2, -2);

			Assert::AreEqual(l1->intersect(s1).count, 0);

			Assert::AreEqual(l1->intersect(r1).count, 1);
			Assert::AreEqual(l1->intersect(r1).p1, Point(2.0, 2.0));

			Assert::AreEqual(l1->intersect(r2).count, 1);
			Assert::AreEqual(l1->intersect(r2).p1, Point(0.0, 0.0));

			Assert::AreEqual(s1->intersect(r1).count, 1);
			Assert::AreEqual(s1->intersect(r1).p1, Point(2.0, 0.0));

			Assert::AreEqual(s1->intersect(r2).count, 1);
			Assert::AreEqual(s1->intersect(r2).p1, Point(1.0, -1.0));

			Assert::AreEqual(r1->intersect(r2).count, 1);
			Assert::AreEqual(r1->intersect(r2).p1, Point(2.0, -2.0));
		}

		TEST_METHOD(TestMethod7)
		{
			Shape* c1 = new Circle(0, 0, 2);
			Shape* l1 = new Line(1, 0, -1, 1);
			Shape* s1 = new Segment(0, 3, 1, 1);
			Shape* r1 = new Radial(1, 0, 0, 1);
			Shape* r2 = new Radial(0, -3, 1, -3);
			Shape* none = new Line(1, 0, 1, 1);
			none->type = ShapeType::NONE;

			Assert::AreEqual(c1->intersect(s1).count, 1);

			Assert::AreEqual(c1->intersect(r1).count, 1);

			Assert::AreEqual(c1->intersect(r2).count, 0);

			Assert::AreEqual(c1->intersect(none).count, 0);

			Assert::AreEqual(l1->intersect(none).count, 0);

			Assert::AreEqual(s1->intersect(none).count, 0);

			Assert::AreEqual(r1->intersect(none).count, 0);
		}
	};
}
