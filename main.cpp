#include <iostream>
#include <vector>
#include <cstdio>
#include <algorithm>
#include <chrono>
#include <random>
#include <deque>
#include <string>

const size_t TEST_NUMBER = 10;

struct MyStruct
{
    std::string Key;
    double Value;
};

bool operator< (const MyStruct &a, const MyStruct &b)
{
    return a.Key < b.Key;
}

template<typename Iterator, typename Comparator=std::less
         <typename std::iterator_traits<Iterator>::value_type>>
void SortInsertionManualCopy(Iterator Begin, Iterator End, const Comparator &comparator = Comparator())
{
    if (Begin >= End)
    {
        return;
    }
    else
    {

        for (Iterator it = Begin; it != End; ++it)
        {

            for (Iterator moveIt = it; (moveIt != Begin) && ( comparator (*moveIt, *(moveIt-1))); --moveIt)
            {
                std::swap(*moveIt, *(moveIt-1));
            }

        }

    }
}

template<typename Iterator, typename Comparator=std::less
         <typename std::iterator_traits<Iterator>::value_type>>
void SortInsertionSTLCopy(Iterator Begin, Iterator End, const Comparator &comparator = Comparator())
{
    if (Begin >= End)
    {
        return;
    }
    else
    {

        for (Iterator it = Begin; it != End; ++it)
        {
            typename std::iterator_traits<Iterator>::value_type rememberValue = *it;
            Iterator moveIt = it;

            while ((moveIt != Begin) && (comparator(*it, *(moveIt - 1))))
                --moveIt;

            std::copy_backward(moveIt, it, it + 1);
            *moveIt = rememberValue;
        }

    }
}

template<typename Iterator, typename Comparator=std::less
         <typename std::iterator_traits<Iterator>::value_type>>
void SortSelection(Iterator Begin, Iterator End, const Comparator &comparator = Comparator())
{
    if (Begin >= End)
    {
        return;
    }
    else
    {
        for (Iterator it = Begin; it != End; ++it)
        {
            Iterator MinIt = it;

            for (Iterator Current = it + 1; Current != End; ++Current)
            {
                if (comparator(*Current, *MinIt))
                    MinIt = Current;
            }

            std::swap(*MinIt, *it);
        }
    }
}

template<typename Iterator>
void SortHeap(Iterator Begin, Iterator End)
{
    if (Begin >= End)
    {
        return;
    }
    else
    {
        std::make_heap(Begin, End);

        while (End != Begin)
            std::pop_heap(Begin, End--);

    }
}

std::default_random_engine QuickSortLocalGen;
const size_t CUT_OFF = 25;

template<typename Iterator, typename Comparator=std::less
         <typename std::iterator_traits<Iterator>::value_type>>
void SortQuick(Iterator Begin, Iterator End, const Comparator &comparator = Comparator())
{
    if (Begin >= End)
    {
        return;
    }
    else
    {
        if (End - Begin <= CUT_OFF)
        {
            SortInsertionManualCopy(Begin, End, comparator);
            return;
        }

        Iterator Left = Begin, Right = End - 1;
        std::uniform_int_distribution<size_t> MiddleRnd(0, End - Begin - 1);
        typename std::iterator_traits<Iterator>::value_type Middle = *(Begin + MiddleRnd(QuickSortLocalGen));

        do
        {
            while (comparator(*Left, Middle))
                ++Left;

            while (comparator(Middle, *Right))
                --Right;

            if (Left <= Right)
            {
                std::swap(*Left, *Right);
                ++Left;
                --Right;
            }

        }while(Left < Right);

        if (Left < End)
            SortQuick(Left, End, comparator);

        if (Begin < Right)
            SortQuick(Begin, Right + 1, comparator);

    }
}

template <typename SrcIterator, typename DstIterator, typename Comparator>
DstIterator MergeSubArrays(SrcIterator FirstBegin, SrcIterator FirstEnd,
                           SrcIterator SecondBegin, SrcIterator SecondEnd, DstIterator target,
                           Comparator comparator)
{
    DstIterator Result = target + (FirstEnd - FirstBegin) + (SecondEnd - SecondBegin);
    while ((FirstBegin != FirstEnd)&&(SecondBegin != SecondEnd))
    {
        if (comparator(*SecondBegin, *FirstBegin))
            *(target++) = *(SecondBegin++);
        else *(target++) = *(FirstBegin++);
    }

    std::copy(SecondBegin, SecondEnd, target);
    std::copy(FirstBegin, FirstEnd, target);

    return Result;
}

template<typename Iterator, typename Comparator=std::less
         <typename std::iterator_traits<Iterator>::value_type>>
void SortMergeRec(Iterator Begin, Iterator End, const Comparator &comparator = Comparator())
{
    if (End - Begin < 2)
    {
        return;
    }
    else
    {
        Iterator Middle = Begin + (End - Begin - 1) / 2;
        std::vector<typename std::iterator_traits<Iterator>::value_type> buffer (End-Begin);

        SortMergeRec(Begin, Middle + 1, comparator);
        SortMergeRec(Middle + 1, End, comparator);

        MergeSubArrays(Begin, Middle + 1, Middle + 1, End, buffer.begin(), comparator);
        std::copy(buffer.begin(), buffer.end(), Begin);
    }
}

template<typename Iterator, typename Comparator=std::less
         <typename std::iterator_traits<Iterator>::value_type>>
void SortMergeIteration(Iterator Begin, Iterator End, const Comparator &comparator = Comparator())
{
	std::vector<typename std::iterator_traits<Iterator>::value_type> buffer (End-Begin);

	for (std::ptrdiff_t chunk_size = 1; chunk_size < End - Begin; chunk_size *= 2)
	{
		auto target = buffer.begin();

		for (Iterator first_begin = Begin; first_begin < End; first_begin += std::min(2 * chunk_size, End - first_begin))
		{
			Iterator second_begin = first_begin + std::min(chunk_size, End - first_begin);
			Iterator second_end = second_begin + std::min(chunk_size, End - second_begin);
			target = MergeSubArrays(first_begin, second_begin, second_begin, second_end, target, comparator);
		}

        std::copy(buffer.begin(), buffer.end(), Begin);
	}
}

template <typename Container>
void Reserve(Container &c, size_t n){}

template <typename Value>
void Reserve(std::vector<Value> &c, size_t n)
{
    c.reserve(n);
}

template <typename Iterator>
bool CheckCorrect(Iterator testBegin, Iterator testEnd, Iterator perfBegin, Iterator perfEnd)
{
    if ((testEnd - testBegin) != (perfEnd - perfBegin)) //solve this problem
        return false;

    while (testBegin != testEnd)
    {
        if (((*testBegin < *perfBegin) || (*perfBegin < *testBegin)))
            return false;
        else
        {
            ++testBegin;
            ++perfBegin;
        }
    }

    return true;
}

template <typename SortFunc, typename Container, typename Gen>
bool TestSort(const SortFunc &sortFunc, size_t length, const Gen &gen, std::chrono::duration<double> &workTime)
{
    Container data;
    Reserve (data, length);

    for (size_t i = 0; i < length; ++i)
    {
        data.push_back(gen());
    }

    Container perfect = data;
    std::stable_sort(perfect.begin(), perfect.end());

    auto TStart = std::chrono::steady_clock::now();
    sortFunc(data);
    auto TEnd = std::chrono::steady_clock::now();

    workTime = TEnd - TStart;

    return CheckCorrect(data.begin(), data.end(), perfect.begin(), perfect.end());
}

template <typename SortFunc, typename Container, typename Gen, typename GenLength>
void RunTestSortAll(const SortFunc sortFunc, size_t testNumber, Gen const &gen, GenLength const &genLength)
{
    std::chrono::duration<double> Time;
    bool success = true;

    for (size_t i = 0; i <= testNumber; ++i)
    {
        size_t localLength = genLength();

        if (!TestSort<SortFunc, Container, Gen> (sortFunc, localLength, gen, Time))
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
        }

    }

    if (success)
        std::cout << std::endl << "ALL TESTES PASSED SUCCESSFULLY." << std::endl;

}

/*template <typename SortFunc>
void TEST(const SortFunc &sortFunc)
{
    std::vector<int> data(10,0);
    std::default_random_engine gen;
    std::uniform_int_distribution<int> Val(0, 10000);

    for (auto it = data.begin(); it != data.end(); ++it)
        *it = Val(gen);

    for (auto it = data.begin(); it != data.end(); ++it)
        std::cout << *it << ' ';

    std::cout << std::endl;
    sortFunc(data);

    for (auto it = data.begin(); it != data.end(); ++it)
        std::cout << *it << ' ';
}*/

int main()
{
    std::default_random_engine generator;
    std::uniform_int_distribution<size_t> Lengths(0, 10000);
    std::uniform_int_distribution<int> Values(1, 1000000);

    //auto genAutoLen=[&](){return Lengths(generator);};

    //vector<int>

    /*std::cout << "TEST : vector<int> 1..10" << std::endl << std::endl;

    std::uniform_int_distribution<int> Values1_10(1, 10);
    auto genTest1 = [&](){return Values1_10(generator);};*/

    auto testHeap = [](std::vector<int> &v){SortHeap(v.begin(), v.end());};
    auto testMerge= [](std::vector<int> &v){SortMergeRec(v.begin(), v.end());};

    std::chrono::duration<double> Time;
    std::cout << "TEST : vector<int> 1..10" << std::endl << std::endl;

    std::uniform_int_distribution<int> Values1_10(1, 10);
    auto genAutoLen=[&](){return Lengths(generator);};
    auto genTest1=[&](){return Values1_10(generator);};

    RunTestSortAll <decltype(testHeap), std::vector<int>, decltype(genTest1), decltype(genAutoLen)>
        (testHeap, TEST_NUMBER, genTest1, genAutoLen);

    RunTestSortAll <decltype(testMerge), std::vector<int>, decltype(genTest1), decltype(genAutoLen)>
        (testMerge, TEST_NUMBER, genTest1, genAutoLen);
}
