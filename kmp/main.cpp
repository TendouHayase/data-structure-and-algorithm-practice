#include <bits/stdc++.h>
using namespace std;

vector<int> getFailure(const string& pattern)
{
    int m = pattern.size();
    vector<int> fail(m, 0);
    int j = 0;
    for (int i = 1; i < m; i++)
    {
        while (j > 0 && pattern[i] != pattern[j])
        {
            j = fail[j - 1];
        }
        if (pattern[i] == pattern[j])
        {
            fail[i] = ++j;
        }
    }
    return fail;
}

vector<int> kmpSearch(const string& text, const string& pattern)
{
    vector<int> result;
    int n = text.size(), m = pattern.size();
    if (m == 0)
        return result;

    vector<int> fail = getFailure(pattern);
    int j = 0;
    for (int i = 0; i < n; i++)
    {
        while (j > 0 && text[i] != pattern[j])
        {
            j = fail[j - 1];
        }
        if (text[i] == pattern[j])
        {
            j++;
        }
        if (j == m)
        {
            result.push_back(i - m + 1);
            j = fail[j - 1];
        }
    }
    return result;
}

int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    string text, pattern;
    cin >> text >> pattern;

    vector<int> matches = kmpSearch(text, pattern);

    cout << matches.size() << '\n';
    for (int idx : matches)
    {
        cout << idx + 1 << ' ';
    }
    cout << '\n';

    return 0;
}