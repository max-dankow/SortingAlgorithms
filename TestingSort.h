#include <iostream>
#include <vector>
#include <cstdio>
#include <algorithm>
#include <chrono>
#include <random>
#include <deque>
#include <string>

template <typename Container>
void Reserve(Container &c, size_t n){}

template <typename Value>
void Reserve(std::vector<Value> &c, size_t n)
{
    c.reserve(n);
}

template <typename Iterator, typename Comparator>
bool CheckCorrect(Iterator testBegin, Iterator testEnd, Iterator perfBegin, Iterator perfEnd, const Comparator &comparator)
{
    if ((testEnd - testBegin) != (perfEnd - perfBegin))
        return false;

    while (testBegin != testEnd)
    {
        if (!(*testBegin == *perfBegin))
            return false;
        else
        {
            ++testBegin;
            ++perfBegin;
        }
    }

    return true;
}

template <typename SortFunc, typename Container, typename Gen, typename Comparator=std::less
         <typename Container::value_type>>
bool TestSort(const SortFunc &sortFunc, size_t length, const Gen &gen,
              std::chrono::milliseconds &workTime, const Comparator &comparator = Comparator())
{
    Container data;
    Reserve (data, length);

    for (size_t i = 0; i < length; ++i)
    {
        data.push_back(gen());
    }

    Container perfect = data;
    std::stable_sort(perfect.begin(), perfect.end(), comparator);

    auto TStart = std::chrono::steady_clock::now();
    sortFunc(data);
    auto TEnd = std::chrono::steady_clock::now();

    workTime = std::chrono::milliseconds(std::chrono::duration_cast<std::chrono::milliseconds>(TEnd - TStart).count());

    return CheckCorrect(data.begin(), data.end(), perfect.begin(), perfect.end(), comparator);
}

template <typename SortFunc, typename Container, typename Gen, typename GenLength,
         typename Comparator=std::less<typename Container::value_type>>
void RunTestSortAll(const SortFunc sortFunc, size_t testNumber, Gen const &gen, GenLength const &genLength,
                    const std::string message, std::chrono::milliseconds &averageTime, const Comparator &comparator = Comparator())
{
    std::chrono::milliseconds Time;
    averageTime = std::chrono::milliseconds (0);
    bool success = true;

    for (size_t i = 0; i <= testNumber - 1; ++i)
    {
        size_t localLength = genLength();
        std::cout << message << ": ";

        if (!TestSort<SortFunc, Container, Gen> (sortFunc, localLength, gen, Time, comparator))
        {
            std::cout << "[-] Error: not correct! Time: ";
            std::cout << Time.count() << " ms. Length is " << localLength << std::endl;
            success = false;
            break;
        }
        else
        {
            std::cout << "[+] All right. Time: ";
            std::cout << Time.count() << " ms. Length is " << localLength << std::endl;
            averageTime += Time;
        }

    }

    averageTime /= testNumber;

    if (success)
    {
        std::cout << std::endl << "ALL TESTES PASSED SUCCESSFULLY. AVERAGE TIME IS "
            << averageTime.count() << std::endl << std::endl;
    }

}
