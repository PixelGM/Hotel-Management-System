#pragma once
#include <string>
#include <vector>

int levenshtein_distance(const std::string& s1, const std::string& s2);
std::string search_keyword(const std::string& keyword, const std::vector<std::string>& keywords);
