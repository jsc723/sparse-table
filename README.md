# sparse-table
A sparse table implementation in c++ (std=c++14).
This is a header-only library.

## Usage for min (or max) sparse table:
```cpp
vector<int> v {1,-3,4,2,4};
MinSparseTable<int> minst = createMinSparseTable(v);
cout << minst.queryIndex(0, 5) << endl;  //print 1 since v[1]==-3 is the min in range v[0,5)
```

## Usage for a sum sparse table:
```cpp
SparseTable<int> st = SparseTable<int>(v, [](const int& a, const int& b) { return a + b; });
cout << minst.query(0, 5) << endl;  //print 12
```

You can explore more by reading the code itself.
