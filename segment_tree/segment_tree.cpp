#include <concepts>

template <typename T>
concept INT = std::is_integral_v<T>;

template <typename INT> class SegmentTree
{
    INT* arr_ = nullptr;
    size_t size_;

  public:
    SegmentTree() = default;
    ~SegmentTree();

    void build(INT* data, size_t size);
    INT query(int start, int end);
    void replace(INT data, int index);
    void remove(int index);

  private:
    INT build(INT* data, int start, int end, int idx);
    INT query(int start, int end, int cur_start, int cur_end, int idx);
    INT replace(INT data, int idx, int start, int end, int i);
};

template <typename INT> SegmentTree<INT>::~SegmentTree()
{
    if (arr_ != nullptr)
        delete[] arr_;
}

template <typename INT> void SegmentTree<INT>::build(INT* data, size_t size)
{
    if (arr_ != nullptr)
        return;
    arr_ = new INT[size * 4];
    size_ = size;
    build(data, 0, size - 1, 1);
}

template <typename INT>
INT SegmentTree<INT>::build(INT* data, int start, int end, int idx)
{
    int mid = (start + end) / 2;
    if (start == end)
        arr_[idx] = data[start];
    else [[likely]]
    {
        INT lhs = build(data, start, mid, 2 * idx);
        INT rhs = build(data, mid + 1, end, 2 * idx + 1);
        arr_[idx] = lhs + rhs;
    }

    return arr_[idx];
}

template <typename INT> INT SegmentTree<INT>::query(int start, int end)
{
    return query(start, end, 0, size_ - 1, 1);
}

template <typename INT>
INT SegmentTree<INT>::query(int start, int end, int cur_start, int cur_end,
                            int idx)
{
    int mid = (cur_start + cur_end) / 2;

    if (cur_end < start || cur_start > end)
    {
        return 0;
    }
    else if (start <= cur_start && cur_end <= end)
    {
        return arr_[idx];
    }
    else
    {
        return query(start, end, cur_start, mid, 2 * idx) +
               query(start, end, mid + 1, cur_end, 2 * idx + 1);
    }
}

template <typename INT> void SegmentTree<INT>::replace(INT data, int idx)
{
    replace(data, idx, 0, size_ - 1, 1);
}

template <typename INT>
INT SegmentTree<INT>::replace(INT data, int idx, int start, int end, int i)
{
    if (idx == start && idx == end)
    {
        INT delta = data - arr_[i];
        arr_[i] = data;
        return delta;
    }

    int mid = (start + end) / 2;

    if (start <= idx && idx <= mid)
    {
        INT delta = replace(data, idx, start, mid, i * 2);
        arr_[i] += delta;
        return delta;
    }
    else if (mid < idx && idx <= end)
    {
        INT delta = replace(data, idx, mid + 1, end, i * 2 + 1);
        arr_[i] += delta;
        return delta;
    }
    return 0;
}

template <typename INT> void SegmentTree<INT>::remove(int idx)
{
    replace(0, idx);
}