#include <iostream>
#include <fstream>
#include <sstream>

#include <algorithm>
#include <memory>

#include <unordered_set>
#include <vector>
#include <string>

using namespace std;

struct Range
{
    uint64_t start = 0;
    uint64_t stop = 0;

    bool contains(uint64_t id) const
    {
        return id >= start && id <= stop;
    }

    bool contains(const Range &other) const
    {
        return contains(other.start) && contains(other.stop);
    }

    uint64_t size() const
    {
        return (stop - start) + 1;
    }
};

ostream &operator<<(ostream &out, const Range &p)
{
    return out << "Range{" << to_string(p.start) << ":" << to_string(p.stop) << "," << to_string(p.size()) << "}";
}

class Ingredients
{
public:
    vector<Range> &getRanges()
    {
        return ranges;
    }

    vector<uint64_t> &getIds()
    {
        return ids;
    }

private:
    vector<Range> ranges;
    vector<uint64_t> ids;
};

void first(shared_ptr<Ingredients> input)
{
    uint64_t sum = 0;

    for (const auto id : input->getIds())
    {
        for (const auto &range : input->getRanges())
        {
            if (range.contains(id))
            {
                sum++;
                break;
            }
        }
    }

    cout << "Day one: " << sum << endl;
}

void second(shared_ptr<Ingredients> input)
{
    uint64_t sum = 0;
    vector<Range> continuousRanges;

    for (const auto &range : input->getRanges())
    {
        Range *updatedExistingRange = nullptr;
        bool currentIsDone = false;

        for (auto &existing : continuousRanges)
        {
            // new range already contained entirely inside one of the new cont. ranges
            if (existing.contains(range))
            {
                currentIsDone = true;
                break;
            }

            if (existing.contains(range.start))
            {
                // Update the end because it will now contain the range
                existing.stop = range.stop;
                updatedExistingRange = &existing;
                break;
            }
            else if (existing.contains(range.stop))
            {
                // Update the start because it will now contain the range
                existing.start = range.start;
                updatedExistingRange = &existing;
                break;
            }
        }

        if (!currentIsDone)
        {
            // Add it to the ranges when nothing could be done
            if (updatedExistingRange == nullptr)
            {
                auto &it = continuousRanges.emplace_back(range);
                updatedExistingRange = &it;
            }
            // We might need to do some cleanup now: new range might overlap with other ones
            for (auto it = continuousRanges.end() - 1; it >= continuousRanges.begin(); --it)
            {
                Range &existing = *it;
                if (&existing == updatedExistingRange)
                    continue;

                if (updatedExistingRange->contains(existing))
                {
                    continuousRanges.erase(it);
                }
                else if (updatedExistingRange->contains(existing.start))
                {
                    updatedExistingRange->stop = existing.stop;
                    continuousRanges.erase(it);
                    break;
                }
                else if (updatedExistingRange->contains(existing.stop))
                {
                    updatedExistingRange->start = existing.start;
                    continuousRanges.erase(it);
                    break;
                }
            }
        }
    }

    for (auto &range : continuousRanges)
    {
        cout << " (" << range.start << "," << range.stop << ")" << " - " << range.size() << endl;
        sum += range.size();
    }

    // 413090025954769 is too high
    // 353856671283188 is too low
    // 360341832208407

    cout << "Day two: " << sum << endl;
}

shared_ptr<Ingredients> read_input(string filename)
{
    ifstream infile(filename);

    if (infile.good())
    {
        auto ingredients = make_shared<Ingredients>();

        string line;
        while (getline(infile, line))
        {
            if (line.empty())
                continue;
            if (line.contains("-"))
            {
                auto split = stringstream(line);
                Range range;
                string values;
                getline(split, values, '-');
                range.start = stoul(values);
                getline(split, values);
                range.stop = stoul(values);

                ingredients->getRanges().push_back(range);
            }
            else
            {
                ingredients->getIds().push_back(stoul(line));
            }
        }

        return ingredients;
    }
    else
    {
        cout << "Bad input file " << filename << endl;
        return nullptr;
    }
}

int main(int argc, char *archv[])
{
    std::string inputFile = "test.txt";
    if (argc > 1)
    {
        inputFile = std::string(archv[1]);
    }

    std::cout << "Let's get started on " << inputFile << std::endl;

    auto input = read_input(inputFile);
    first(input);
    second(input);

    return 0;
}
