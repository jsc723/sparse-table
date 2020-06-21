#pragma once
#include <algorithm>
#include <vector>
#include <cmath>
#include <functional>

namespace jsc723
{
	using std::cbegin;
	using std::cend;
	using std::vector;
	using std::function;

	template<typename T>
	class SparseTable
	{
	public:
		using Func = function<T(const T&, const T&)>;

		SparseTable(const vector<T>& data, const Func& op) :
			op{ op }
		{
			const int p = (int)log2(data.size()) + 1;
			const uint64_t n = data.size();
			dp.resize(p);
			std::copy(cbegin(data), cend(data), std::back_inserter(dp[0]));
			for (int i = 1; i < p; i++)
			{
				dp[i].resize(n);
				for (int j = 0; j + (1 << i) - 1 < n; j++)
				{
					dp[i][j] = op(dp[i - 1][j], dp[i - 1][j + (1 << (i - 1))]);
				}
			}
		}

		virtual T query(int beginIdx, int endIdx)
		{
			if (beginIdx >= endIdx)
				throw std::runtime_error("invalid range for SparseTable::query()");
			int u = log2(endIdx - beginIdx);
			T result = dp[u][beginIdx];
			int v = 1 << u; //2^u
			beginIdx += v;
			while (endIdx - beginIdx > 0)
			{
				u = log2(endIdx - beginIdx);
				result = op(result, dp[u][beginIdx]);
				v = 1 << u;
				beginIdx += v;
			}
			return result;
		}
	protected:
		vector<vector<T>> dp;
		Func op;
	};

	template<typename T>
	class SparseTableOverlapable : virtual public SparseTable<T>
	{
	public:
		SparseTableOverlapable(const vector<T>& data, const Func& op) :
			SparseTable(data, op)
		{
		}
		virtual T query(int beginIdx, int endIdx) override
		{
			if (beginIdx >= endIdx)
				throw std::runtime_error("invalid range for SparseTableOverlapable::query()");
			int u = log2(endIdx - beginIdx);
			int v = 1 << u;
			int rightIdx = endIdx - v;
			return op(dp[u][beginIdx], dp[u][rightIdx]);
		}
	};

	template<typename T>
	class SparseTableIndexable : virtual public SparseTable<T>
	{
	public:
		SparseTableIndexable(const vector<T>& data, const Func& op) :
			SparseTable(data, op)
		{
			const int p = dp.size();
			const int n = dp[0].size();
			indices.resize(p);
			for (int j = 0; j < n; j++)
			{
				indices[0].push_back(j);
			}
			for (int i = 1; i < p; i++)
			{
				indices[i].resize(n);
				for (int j = 0; j + (1 << i) - 1 < n; j++)
				{
					int r = j + (1 << (i - 1));
					T t = op(dp[i - 1][j], dp[i - 1][r]);
					indices[i][j] = (t == dp[i - 1][j]) ? indices[i - 1][j] : indices[i - 1][r];
				}
			}
		}
		virtual int queryIndex(int beginIdx, int endIdx)
		{
			if (beginIdx >= endIdx)
				throw std::runtime_error("invalid range for SparseTableIndexable::queryIndex()");
			int u = log2(endIdx - beginIdx);
			T t = dp[u][beginIdx];
			int result = indices[u][beginIdx];
			int v = 1 << u; //2^u
			beginIdx += v;
			while (endIdx - beginIdx > 0)
			{
				u = log2(endIdx - beginIdx);
				T tt = op(t, dp[u][beginIdx]);
				result = (t == tt) ? result : indices[u][beginIdx];
				t = tt;
				v = 1 << u;
				beginIdx += v;
			}
			return result;
		}
	protected:
		vector<vector<int>> indices;
	};

	template<typename T>
	class SparseTableOverlapIndexable : public SparseTableOverlapable<T>, public SparseTableIndexable<T>
	{
	public:
		SparseTableOverlapIndexable(const vector<T>& data, const Func& op) :
			SparseTable(data, op),
			SparseTableOverlapable(data, op),
			SparseTableIndexable(data, op)
		{
		}
		virtual T query(int beginIdx, int endIdx) override
		{
			return this->SparseTableOverlapable::query(beginIdx, endIdx);
		}
		virtual int queryIndex(int beginIdx, int endIdx) override
		{
			if (beginIdx >= endIdx)
				throw std::runtime_error("invalid range for SparseTableOverlapIndexable::queryIndex()");
			int u = log2(endIdx - beginIdx);
			int v = 1 << u;
			int rightIdx = endIdx - v;
			int t = op(dp[u][beginIdx], dp[u][rightIdx]);
			return (t == dp[u][beginIdx]) ? indices[u][beginIdx] : indices[u][rightIdx];
		}
	};

	template<typename T>
	using MinSparseTable = SparseTableOverlapIndexable<T>;
	template<typename T>
	using MaxSparseTable = SparseTableOverlapIndexable<T>;

	template<typename T>
	MinSparseTable<T> createMinSparseTable(const vector<T>& data) {
		return MinSparseTable<T>(data, [](const T& a, const T& b) { return std::min(a, b);  });
	}

	template<typename T>
	MaxSparseTable<T> createMaxSparseTable(const vector<T>& data) {
		return MinSparseTable<T>(data, [](const T& a, const T& b) { return std::max(a, b);  });
	}

}