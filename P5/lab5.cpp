#include <iostream>
#include <iomanip>
#include <vector>
#include <set>
#include <map>
#include <list>

using namespace std;

#define EPSILON ' '

enum Actions
{
    ACTION_SHIFT,
    ACTION_REDUCE,
    ACTION_GOTO,
    ACTION_ACCEPT
};

typedef vector<char> Rule;
typedef set<char> Set;
typedef pair<int, char> Index;
typedef pair<Actions, int> Action;
typedef map<Index, Action> ParsingTable;
typedef set<pair<vector<char>, list<char>>> Item;

vector<Rule> grammar = {
    {'S', 'E', '$'},
    {'E', 'E', '+', 'T'},
    {'E', 'T'},
    {'T', 'T', '*', 'F'},
    {'T', 'F'},
    {'F', '(', 'E', ')'},
    {'F', 'i'}};

map<char, Set> getFirstSet(vector<Rule> &rules);
vector<Item> getItems(vector<Rule> &rules, map<pair<Item, char>, Item> &gotos);
void printTable(ParsingTable &tbl, vector<Rule> &rules, vector<Item> &items);
string actionString(Action &action);
Item getClosure(Item item, vector<Rule> &rules);
map<char, Set> getFollowSet(vector<Rule> &rules, map<char, Set> first);
void printLR0Items(vector<Item> &items);
void printCanonicalItem(Item item);
Item actionGOTO(Item item, char ch, map<pair<Item, char>, Item> &gotos, vector<Rule> &rules);

map<char, Set> getFirstSet(vector<Rule> &rules)
{
    map<char, Set> first;

    for (auto &vec : rules)
    {
        for (auto &ch : vec)
        {
            if (isupper(ch))
            {
                first.insert({ch, {}});
            }
            else
            {
                first.insert({ch, {ch}});
            }
        }
    }

    bool did_change = false;
    do
    {
        did_change = false;
        for (auto &rule : rules)
        {
            char non_terminal = rule.front();
            bool is_nullable = true;
            auto it = rule.begin() + 1;
            for (; it < rule.end() && is_nullable; ++it)
            {
                is_nullable = false;
                for (auto &ch : first.at(*it))
                {
                    if (ch != EPSILON)
                    {
                        // cout << (non_terminal) << ch << endl;
                        if (!did_change)
                        {
                            did_change = first.at(non_terminal).insert(ch).second;
                        }
                        else
                        {
                            first.at(non_terminal).insert(ch);
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
                first.at(non_terminal).insert(EPSILON);
            }
        }
    } while (did_change);

    return first;
}

vector<Item> getItems(vector<Rule> &rules, map<pair<Item, char>, Item> &gotos)
{
    set<Item> _items;
    vector<Item> items;
    items.push_back(getClosure({{{'S'}, {'E'}}}, rules));
    _items.insert(items.back());

    set<char> symbols;
    for (auto &rule : rules)
    {
        for (auto &ch : rule)
        {
            symbols.insert(ch);
        }
    }

    bool did_change = true;
    while (did_change)
    {
        did_change = false;
        for (auto &item : _items)
        {
            for (auto &symbol : symbols)
            {
                Item goto_res = actionGOTO(item, symbol, gotos, rules);
                if (goto_res.size() > 0 && _items.insert(goto_res).second)
                {
                    did_change = true;
                    items.push_back(goto_res);
                }
            }
        }
    }

    return items;
}

map<char, Set> getFollowSet(vector<Rule> &rules, map<char, Set> first)
{
    map<char, Set> follow;
    for (auto &it : rules)
    {
        for (auto &ch : it)
        {
            follow.insert({ch, {}});
        }
    }
    follow.at(rules.begin()->front()) = {'$'};

    bool did_change = false;
    do
    {
        did_change = false;
        for (auto &rule : rules)
        {
            for (auto &symbol : follow.at(rule.front()))
            {
                did_change |= follow.at(rule.back()).insert(symbol).second;
            }
            auto rest = follow.at(rule.front());
            for (size_t idx = rule.size() - 1; idx >= 2; --idx)
            {
                for (auto &it : first.at(rule[idx]))
                {
                    if (it != EPSILON)
                    {
                        did_change |= follow.at(rule[idx - 1]).insert(it).second;
                    }
                    else
                    {
                        for (auto &v : rest)
                        {
                            did_change |= follow.at(rule[idx - 1]).insert(v).second;
                        }
                    }
                }
                rest = follow.at(rule[idx - 1]);
            }
        }
    } while (did_change);

    return follow;
}

void printCanonicalItem(Item item)
{
    for (auto &it : item)
    {
        auto it1 = it.first.begin();
        cout << *it1 << " -> ";
        ++it1;
        for (; it1 < it.first.end(); ++it1)
        {
            cout << *it1;
        }
        cout << ".";
        for (auto &it2 : it.second)
        {
            cout << it2;
        }
        cout << endl;
    }
}

string actionString(Action &action)
{
    stringstream ret;
    switch (action.first)
    {
    case ACTION_SHIFT:
        ret << "S-" << action.second;
        break;
    case ACTION_REDUCE:
        ret << "R-" << action.second;
        break;
    case ACTION_GOTO:
        ret << action.second;
        break;
    case ACTION_ACCEPT:
        ret << "Accept";
        break;
    }
    return ret.str();
}

void printLR0Items(vector<Item> &items)
{
    size_t idx = 0;
    for (auto &item : items)
    {
        cout << "Item " << idx << endl;
        printCanonicalItem(item);
        cout << endl;
        ++idx;
    }
}

Item getClosure(Item item, vector<Rule> &rules)
{
    bool did_change = true;
    do
    {
        did_change = false;
        for (auto &it : item)
        {
            if (it.second.size() >= 1)
            {
                if (isupper(it.second.front()))
                {
                    char non_terminal = it.second.front();
                    for (auto &rule : rules)
                    {
                        if (rule.front() == non_terminal)
                        {
                            list<char> r_val;
                            for (auto symbol = rule.begin() + 1; symbol < rule.end(); ++symbol)
                            {
                                r_val.push_back(*symbol);
                            }
                            did_change |= item.insert({{non_terminal}, r_val}).second;
                        }
                    }
                }
            }
        }
    } while (did_change);

    return item;
}

void printTable(ParsingTable &tbl, vector<Rule> &rules, vector<Item> &items)
{
    list<char> symbols;
    set<char> _symbols;
    for (auto &rule : rules)
    {
        for (auto &ch : rule)
        {
            if (isupper(ch))
            {
                if (ch != 'S')
                {
                    if (_symbols.insert(ch).second)
                        symbols.push_back(ch);
                }
            }
            else
            {
                if (_symbols.insert(ch).second)
                    symbols.push_front(ch);
            }
        }
    }

    const size_t WIDTH_COL_1 = 8;
    const size_t WIDTH_OTHER = 10;

    cout << setw(WIDTH_COL_1) << "State";
    for (auto &ch : symbols)
    {
        cout << '|' << setw(WIDTH_OTHER) << ch;
    }
    cout << endl;
    for (size_t idx = 0; idx < WIDTH_COL_1 + (WIDTH_OTHER + 1) * symbols.size(); ++idx)
    {
        cout << '-';
    }
    cout << endl;

    for (size_t idx = 0; idx < items.size(); ++idx)
    {
        cout << setw(WIDTH_COL_1) << idx;
        for (auto &ch : symbols)
        {
            cout << '|' << setw(WIDTH_OTHER);
            if (tbl.find({idx, ch}) != tbl.end())
            {
                cout << actionString(tbl.at({idx, ch}));
            }
            else
            {
                cout << ' ';
            }
        }
        cout << endl;
    }
}

Item actionGOTO(Item item, char ch, map<pair<Item, char>, Item> &gotos, vector<Rule> &rules)
{
    Item ret_item;
    bool is_goto = false;
    for (auto &it : item)
    {
        if (it.second.size() >= 1 && ch == it.second.front())
        {
            is_goto = true;
            vector<char> l = it.first;
            list<char> r = it.second;
            l.push_back(r.front());
            r.pop_front();
            ret_item.insert({l, r});
        }
    }
    ret_item = getClosure(ret_item, rules);
    if (is_goto)
    {
        gotos.insert({{item, ch}, ret_item});
    }
    return ret_item;
}

int main(void)
{
    map<char, Set> first = getFirstSet(grammar);

    map<char, Set> follow = getFollowSet(grammar, first);

    map<pair<Item, char>, Item> gotos;
    vector<Item> items = getItems(grammar, gotos);
    cout << "Canonical List of LR(0) items:\n"
         << endl;
    printLR0Items(items);

    set<char> symbols;
    for (auto &rule : grammar)
    {
        for (auto &ch : rule)
        {
            symbols.insert(ch);
        }
    }

    ParsingTable tbl;
    for (size_t idx1 = 0; idx1 < items.size(); ++idx1)
    {
        for (size_t rule_idx = 0; rule_idx < grammar.size(); ++rule_idx)
        {
            for (auto &i : items[idx1])
            {
                if (i.first.front() == 'S' && i.first.back() == 'E')
                {
                    tbl.insert({{idx1, '$'}, {ACTION_ACCEPT, 0}});
                }
                else if (i.second.size() == 0 && i.first == grammar[rule_idx])
                {
                    for (auto &ch : follow.at(grammar[rule_idx].front()))
                    {
                        tbl.insert({{idx1, ch}, {ACTION_REDUCE, rule_idx}});
                    }
                }
            }
        }
        for (size_t idx2 = 0; idx2 < items.size(); ++idx2)
        {
            for (auto &ch : symbols)
            {
                if (gotos.find({items[idx1], ch}) != gotos.end() && gotos.at({items[idx1], ch}) == items[idx2])
                {
                    if (isupper(ch))
                    {
                        tbl.insert({{idx1, ch}, {ACTION_GOTO, idx2}});
                    }
                    else
                    {
                        tbl.insert({{idx1, ch}, {ACTION_SHIFT, idx2}});
                    }
                }
            }
        }
    }

    printTable(tbl, grammar, items);

    return 0;
}
