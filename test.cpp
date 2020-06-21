#include "sparse-table.h"
#include <algorithm>
#include <iostream>
#include <random>
#include <numeric>
#ifndef NDEBUG
#   define ASSERT(condition, message) \
    do { \
        if (! (condition)) { \
            std::cerr << "Assertion `" #condition "` failed in " << __FILE__ \
                      << " line " << __LINE__ << ": " << message << std::endl; \
            std::terminate(); \
        } \
    } while (false)
#else
#   define ASSERT(condition, message) do { } while (false)
#endif
using namespace std;
using namespace jsc723;

const int N = 1000;
default_random_engine eng;

void testMin(const vector<int>& v)
{
	uniform_int_distribution randint(0U, (unsigned)v.size() - 1);
	MinSparseTable<int> minst = createMinSparseTable(v);
	for (int i = 0; i < N; i++) {
		int low = randint(eng);
		int high = randint(eng);
		if (low == high)
			continue;
		if (low > high)
			swap(low, high);
		int ridx = minst.queryIndex(low, high);
		int r = minst.query(low, high);
		ASSERT(v[ridx] == r, "index not matched");
		int r0 = *std::min_element(v.cbegin() + low, v.cbegin() + high);
		ASSERT(r0 == r, "result not correct");
	}
}
void testMax(const vector<int>& v)
{
	uniform_int_distribution randint(0U, (unsigned)v.size() - 1);
	MaxSparseTable<int> maxst = createMaxSparseTable(v);
	for (int i = 0; i < N; i++) {
		int low = randint(eng);
		int high = randint(eng);
		if (low == high)
			continue;
		if (low > high)
			swap(low, high);
		int ridx = maxst.queryIndex(low, high);
		int r = maxst.query(low, high);
		ASSERT(v[ridx] == r, "index not matched");
		int r0 = *std::max_element(v.cbegin() + low, v.cbegin() + high);
		ASSERT(r0 == r, "result not correct");
	}
}
void testSum(const vector<int>& v)
{
	uniform_int_distribution randint(0U, (unsigned)v.size() - 1);
	SparseTable<int> st = SparseTable<int>(v, [](const int& a, const int& b) { return a + b; });
	for (int i = 0; i < 100; i++) {
		int low = randint(eng);
		int high = randint(eng);
		if (low == high)
			continue;
		if (low > high)
			swap(low, high);
		int r = st.query(low, high);
		int r0 = std::accumulate(v.cbegin() + low, v.cbegin() + high, 0);
		ASSERT(r0 == r, "result not correct");
	}
}

int main()
{
	vector<int> v;
	uniform_int_distribution<int> randint(-10000, 10000);
	for (int i = 0; i < N; i++)
		v.push_back(randint(eng));
	testMin(v);
	testMax(v);
	testSum(v);
	return 0;
}