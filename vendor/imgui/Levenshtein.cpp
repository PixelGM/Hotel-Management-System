#include "Levenshtein.h"
#include <algorithm>
#include <climits>
#include <iostream>
#include "imgui.h"

using namespace std;

// Function to calculate the Levenshtein distance between two strings
int levenshtein_distance(const string& s1, const string& s2) {
    int m = s1.size(), n = s2.size();
    vector<vector<int>> dp(m + 1, vector<int>(n + 1, 0));

    for (int i = 1; i <= m; ++i) dp[i][0] = i;
    for (int j = 1; j <= n; ++j) dp[0][j] = j;

    for (int i = 1; i <= m; ++i) {
        for (int j = 1; j <= n; ++j) {
            int cost = (s1[i - 1] == s2[j - 1]) ? 0 : 1;
            dp[i][j] = min({ dp[i - 1][j] + 1, dp[i][j - 1] + 1, dp[i - 1][j - 1] + cost });
        }
    }
    return dp[m][n];
}

// Function to search for a keyword in a list of keywords
void search_keyword(const std::string& keyword, const std::vector<std::string>& keywords) {
    int min_distance = INT_MAX;
    std::string closest_match;
    int exact_matches = 0;

    for (const auto& k : keywords) {
        int distance = levenshtein_distance(keyword, k);
        if (distance == 0) {
            ++exact_matches;
            closest_match = k;
        }
        if (distance < min_distance) {
            min_distance = distance;
            closest_match = k;
        }
    }

    if (exact_matches == 1) {
        ImGui::Text("Exact match found: %s", closest_match.c_str());
    }
    else {
        ImGui::Text("Did you mean: %s?", closest_match.c_str());
    }
}


