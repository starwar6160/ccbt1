#include "stdafx.h"

#ifdef _DEBUG
#pragma comment(lib,"gtestd.lib")
#pragma comment(lib,"gtest_main-mdd.lib")
#else
#pragma comment(lib,"gtest.lib")
#pragma comment(lib,"gtest_main-md.lib")
#endif // _DEBUG

int Foo(int a, int b)
{
	if (a == 0 || b == 0)
	{
		throw "don't do that";
	}
	int c = a % b;
	if (c == 0)
		return b;
	return Foo(b, c);
}

#include <gtest/gtest.h>

TEST(FooTest, HandleNoneZeroInput)
{
	EXPECT_EQ(2, Foo(4, 10));
	EXPECT_EQ(6, Foo(30, 18));
}

