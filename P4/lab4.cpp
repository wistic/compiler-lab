#include <iostream>
#include <iomanip>
#include <vector>
#include <set>
#include <map>

#define EPSILON ' '

using namespace std;

typedef vector<char> Rule;
typedef set<char> Set;
typedef pair<char, char> Index;
typedef map<Index, int> ParsingTable;

vector<Rule> grammar = {
	{'S', 'T', 'B'},
	{'B', '+', 'T', 'B'},
	{'B'},
	{'T', 'F', 'C'},
	{'C', '*', 'F', 'C'},
	{'C'},
	{'F', '(', 'S', ')'},
	{'F', 'i'}};

map<char, Set> getFirstSet(vector<Rule> productions);
map<char, Set> getFollowSet(vector<Rule> productions, map<char, Set> first_set);
void checkValidity(string input, ParsingTable parsing_table, vector<Rule> productions);
ParsingTable getParsingTable(vector<Rule> productions, map<char, Set> first_set, map<char, Set> follow_set);

map<char, Set> getFirstSet(vector<Rule> productions)
{
	map<char, Set> first_set;

	for (auto &vec : productions)
	{
		for (auto &ch : vec)
		{
			if (isupper(ch))
			{
				first_set.insert({ch, {}});
			}
			else
			{
				first_set.insert({ch, {ch}});
			}
		}
	}

	bool change = false;
	do
	{
		change = false;
		for (auto &rule : productions)
		{
			char non_terminal = rule.front();
			bool is_nullable = true;
			auto it = rule.begin() + 1;
			for (; it < rule.end() && is_nullable; ++it)
			{
				is_nullable = false;
				for (auto ch : first_set.at(*it))
				{
					if (ch != EPSILON)
					{
						// cout << (non_terminal) << ch << endl;
						if (!change)
						{
							change = first_set.at(non_terminal).insert(ch).second;
						}
						else
						{
							first_set.at(non_terminal).insert(ch);
						}
					}
					else
					{
						is_nullable = true;
					}
				}
			}
			if (is_nullable && it == rule.end())
			{
				first_set.at(non_terminal).insert(EPSILON);
			}
		}
	} while (change);

	return first_set;
}

map<char, Set> getFollowSet(vector<Rule> productions, map<char, Set> first_set)
{
	map<char, Set> follow_set;
	for (auto &it : productions)
	{
		for (auto &ch : it)
		{
			follow_set.insert({ch, {}});
		}
	}
	follow_set.at(productions.begin()->front()) = {'$'};

	bool change = false;
	do
	{
		change = false;
		for (auto &rule : productions)
		{
			for (auto &symbol : follow_set.at(rule.front()))
			{
				change |= follow_set.at(rule.back()).insert(symbol).second;
			}
			auto rest = follow_set.at(rule.front());
			for (size_t idx = rule.size() - 1; idx >= 2; --idx)
			{
				for (auto &it : first_set.at(rule[idx]))
				{
					if (it != EPSILON)
					{
						change |= follow_set.at(rule[idx - 1]).insert(it).second;
					}
					else
					{
						for (auto &v : rest)
						{
							change |= follow_set.at(rule[idx - 1]).insert(v).second;
						}
					}
				}
				rest = follow_set.at(rule[idx - 1]);
			}
		}
	} while (change);

	return follow_set;
}

ParsingTable getParsingTable(vector<Rule> productions,
							 map<char, Set> first_set, map<char, Set> follow_set)
{
	ParsingTable parsing_table;
	for (size_t i = 0; i < productions.size(); ++i)
	{
		bool is_nullable = true;
		size_t idx = 1;
		for (; idx < productions[i].size() && is_nullable; ++idx)
		{
			is_nullable = false;
			for (auto &ch : first_set.at(productions[i][idx]))
			{
				if (ch != EPSILON)
				{
					parsing_table.insert({{productions[i].front(), ch}, i});
				}
				else
				{
					is_nullable = true;
				}
			}
		}
		if (is_nullable && idx == productions[i].size())
		{
			for (auto &it : follow_set.at(productions[i].front()))
			{
				parsing_table.insert({{productions[i].front(), it}, i});
			}
		}
	}
	return parsing_table;
}

void checkValidity(string input, ParsingTable parsing_table, vector<Rule> productions)
{
	vector<char> stack = {'$', productions[0].front()};
	size_t inp_length = input.length();
	auto inp_it = input.begin();

	const size_t STACK_MAX_DISPLAY = 30;
	cout << left << setw(STACK_MAX_DISPLAY)
		 << "Stack"
		 << "INPUT" << endl
		 << endl;

	while (!stack.empty() || inp_it < input.end())
	{
		if (isspace(*inp_it))
		{
			++inp_it;
		}

		// Display
		size_t i = 0;
		for (auto it : stack)
		{
			cout << it;
			++i;
		}
		for (; i < STACK_MAX_DISPLAY; ++i)
		{
			cout << ' ';
		}

		cout << setw(inp_length + 5) << input << endl;

		// Logic
		char top = stack.back();
		char inp = input.front();
		if (isupper(top))
		{
			if (parsing_table.find({top, inp}) != parsing_table.end())
			{
				stack.pop_back();
				int rule_idx = parsing_table.at({top, inp});
				for (auto it = productions[rule_idx].end() - 1; it > productions[rule_idx].begin(); --it)
				{
					stack.push_back(*it);
				}
			}
			else
			{
				cerr << "Error: Input string doesn't fit in the given grammar." << endl;
				exit(1);
			}
		}
		else
		{
			if (top == inp)
			{
				stack.pop_back();
				input = input.substr(1, string::npos);
			}
			else
			{
				cerr << "Error: Input string doesn't fit in the given grammar." << endl;
				exit(1);
			}
		}
	}
	if (stack.empty() && inp_it == input.end())
	{
		cout << endl
			 << "Input string successfully validated." << endl;
	}
}

int main(void)
{
	map<char, Set> first_set = getFirstSet(grammar);
	map<char, Set> follow_set = getFollowSet(grammar, first_set);

	ParsingTable parsing_table = getParsingTable(grammar, first_set, follow_set);

	string input;
	getline(cin, input);

	input.push_back('$');
	checkValidity(input, parsing_table, grammar);

	return 0;
}