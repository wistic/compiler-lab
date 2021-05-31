#include <iostream>
#include <iomanip>
#include <vector>
#include <set>
#include <map>
#include <list>

#define EPSILON ' '

enum Actions
{
    ACTION_SHIFT,
    ACTION_REDUCE,
    ACTION_GOTO,
    ACTION_ACCEPT
};

typedef std::vector<char> Rule;
typedef std::set<char> Set;
typedef std::pair<int, char> Index;
typedef std::pair<Actions, int> Action;
typedef std::map<Index, Action> ParsingTable;
typedef std::set<std::pair<std::vector<char>, std::list<char>>> Item;

std::vector<Rule> grammar = {
    {'S', 'E', '$'},
    {'E', 'E', '+', 'T'},
    {'E', 'T'},
    {'T', 'T', '*', 'F'},
    {'T', 'F'},
    {'F', '(', 'E', ')'},
    {'F', 'i'}};

void print_set(std::map<char, Set> &some_set)
{
    std::cout << std::endl;
    for (auto &it : some_set)
    {
        if (std::isupper(it.first))
        {
            std::cout << "SET(" << it.first << ") : [";
            for (auto &i : it.second)
            {
                std::cout << '\'' << (i) << '\'' << ',';
            }
            std::cout << ']' << std::endl;
        }
    }
}

std::string action_helper(Action &action)
{
    std::stringstream ret;
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

void print_tbl(ParsingTable &tbl, std::vector<Rule> &rules, std::vector<Item> &items)
{
    std::list<char> symbols;
    std::set<char> _symbols;
    for (auto &rule : rules)
    {
        for (auto &ch : rule)
        {
            if (std::isupper(ch))
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

    std::cout << std::setw(WIDTH_COL_1) << "State";
    for (auto &ch : symbols)
    {
        std::cout << '|' << std::setw(WIDTH_OTHER) << ch;
    }
    std::cout << std::endl;
    for (size_t idx = 0; idx < WIDTH_COL_1 + (WIDTH_OTHER + 1) * symbols.size(); ++idx)
    {
        std::cout << '-';
    }
    std::cout << std::endl;

    for (size_t idx = 0; idx < items.size(); ++idx)
    {
        std::cout << std::setw(WIDTH_COL_1) << idx;
        for (auto &ch : symbols)
        {
            std::cout << '|' << std::setw(WIDTH_OTHER);
            if (tbl.find({idx, ch}) != tbl.end())
            {
                std::cout << action_helper(tbl.at({idx, ch}));
            }
            else
            {
                std::cout << ' ';
            }
        }
        std::cout << std::endl;
    }
}

void print_item(Item item)
{
    for (auto &it : item)
    {
        auto it1 = it.first.begin();
        std::cout << *it1 << " -> ";
        ++it1;
        for (; it1 < it.first.end(); ++it1)
        {
            std::cout << *it1;
        }
        std::cout << ".";
        for (auto &it2 : it.second)
        {
            std::cout << it2;
        }
        std::cout << std::endl;
    }
}

void print_items(std::vector<Item> &items)
{
    size_t idx = 0;
    for (auto &item : items)
    {
        std::cout << idx << ')' << std::endl;
        print_item(item);
        std::cout << std::endl;
        ++idx;
    }
}

std::map<char, Set> get_first(std::vector<Rule> &rules)
{
    std::map<char, Set> first;

    for (auto &vec : rules)
    {
        for (auto &ch : vec)
        {
            if (std::isupper(ch))
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
                        // std::cout << (non_terminal) << ch << std::endl;
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

std::map<char, Set> get_follow(std::vector<Rule> &rules, std::map<char, Set> first)
{
    std::map<char, Set> follow;
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

Item closure(Item item, std::vector<Rule> &rules)
{
    bool did_change = true;
    do
    {
        did_change = false;
        for (auto &it : item)
        {
            if (it.second.size() >= 1)
            {
                if (std::isupper(it.second.front()))
                {
                    char non_terminal = it.second.front();
                    for (auto &rule : rules)
                    {
                        if (rule.front() == non_terminal)
                        {
                            std::list<char> r_val;
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

Item Goto(Item item, char ch, std::map<std::pair<Item, char>, Item> &gotos, std::vector<Rule> &rules)
{
    Item ret_item;
    bool is_goto = false;
    for (auto &it : item)
    {
        if (it.second.size() >= 1 && ch == it.second.front())
        {
            is_goto = true;
            std::vector<char> l = it.first;
            std::list<char> r = it.second;
            l.push_back(r.front());
            r.pop_front();
            ret_item.insert({l, r});
        }
    }
    ret_item = closure(ret_item, rules);
    if (is_goto)
    {
        gotos.insert({{item, ch}, ret_item});
    }
    return ret_item;
}

std::vector<Item> get_items(std::vector<Rule> &rules, std::map<std::pair<Item, char>, Item> &gotos)
{
    std::set<Item> _items;
    std::vector<Item> items;
    items.push_back(closure({{{'S'}, {'E'}}}, rules));
    _items.insert(items.back());

    std::set<char> symbols;
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
                Item goto_res = Goto(item, symbol, gotos, rules);
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

int main(void)
{
    std::map<char, Set> first = get_first(grammar);
    // print_set(first);

    std::map<char, Set> follow = get_follow(grammar, first);
    // print_set(follow);

    std::map<std::pair<Item, char>, Item> gotos;
    std::vector<Item> items = get_items(grammar, gotos);
    std::cout << "LR(0) items are:\n"
              << std::endl;
    print_items(items);

    std::set<char> symbols;
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
                    if (std::isupper(ch))
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

    print_tbl(tbl, grammar, items);

    return 0;
}
