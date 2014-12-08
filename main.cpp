#include <iostream>
#include <vector>
#include <cstdio>
#include <algorithm>
#include <chrono>
#include <random>
#include <deque>
#include <string>

struct MyStruct
{
    std::string Key;
    double Value;
};

struct EqualMyStruct
{
    bool operator() (const MyStruct &a, const MyStruct &b)const
    {
        return (a.Key == b.Key) && (a.Value == b.Value);
    }
};

struct CompareMyStruct
{
    bool operator()(const MyStruct &left, const MyStruct &right)const
    {
        return left.Key > right.Key;
    }
};

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

            for (Iterator moveIt = it; moveIt != Begin; --moveIt)
            {
                if (!comparator (*moveIt, *(moveIt-1)))
                    break;
                else
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

template<typename Iterator, typename Comparator=std::less
         <typename std::iterator_traits<Iterator>::value_type>>
void SortHeap(Iterator Begin, Iterator End, const Comparator &comparator = Comparator())
{
    if (Begin >= End)
    {
        return;
    }
    else
    {
        std::make_heap(Begin, End, comparator);

        while (End != Begin)
        {
            std::pop_heap(Begin, End, comparator);
            --End;
        }

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
        if ((size_t ) (End - Begin) <= CUT_OFF)
        {
            SortInsertionSTLCopy(Begin, End, comparator);
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
                           const Comparator &comparator = Comparator())
{
    DstIterator Result = target + (FirstEnd - FirstBegin) + (SecondEnd - SecondBegin);
    while ((FirstBegin != FirstEnd)&&(SecondBegin != SecondEnd))
    {
        if (comparator(*SecondBegin, *FirstBegin))
            *(target++) = *(SecondBegin++);
        else
            *(target++) = *(FirstBegin++);
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

template <typename Iterator, typename EqualComparator>
Iterator FindElement(Iterator Begin, Iterator End, Iterator KeyIt, const std::vector<bool> &available, const EqualComparator &equal_comparator)
{
    size_t i = 0;

    while (Begin != End)
    {
        if (equal_comparator(*Begin, *KeyIt) && (!available[i]))
            return Begin;
        ++Begin;
        ++i;
    }

    return End;
}


template <typename Iterator, typename EqualComparator>
bool CheckValues(Iterator testBegin, Iterator testEnd, Iterator perfBegin, Iterator perfEnd, const EqualComparator &equal_comparator)
{
    if ((testEnd - testBegin) != (perfEnd - perfBegin))
        return false;

    std::vector<bool> flag (testEnd - testBegin, false);

    while (testBegin != testEnd)
    {
        Iterator searchResult = FindElement(perfBegin, perfEnd, testBegin, flag, equal_comparator);

        if (searchResult == perfEnd)
            return false;
        else
        {
            size_t index = searchResult - perfBegin;

            if (flag[index])
                return false;
            else
                flag[index] = true;
        }
        ++testBegin;
    }

    return true;
}

template <typename Iterator, typename Comparator>
bool CheckOrder(Iterator testBegin, Iterator testEnd, const Comparator &comparator)
{
    while (testBegin + 1 < testEnd)
    {
        if (comparator(*(testBegin + 1), *testBegin))
            return false;
        else
            ++testBegin;
    }

    return true;
}

template <typename SortFunc, typename Container, typename Gen, typename Comparator=std::less<typename Container::value_type>,
          typename EqualComparator = std::equal_to<typename Container::value_type>>
bool TestSort(const SortFunc &sortFunc, size_t length, const Gen &gen, std::chrono::milliseconds &workTime,
              const Comparator &comparator = Comparator(), const EqualComparator &equal_comparator = Comparator())
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

    bool Order = CheckOrder(data.begin(), data.end(), comparator);
    bool Values = CheckValues(data.begin(), data.end(), perfect.begin(), perfect.end(), equal_comparator);

    return Order && Values;
}

template <typename SortFunc, typename Container, typename Gen, typename GenLength,
          typename Comparator = std::less<typename Container::value_type>,
          typename EqualComparator = std::equal_to<typename Container::value_type>>
void RunTestSortAll(const SortFunc sortFunc, size_t testNumber, Gen const &gen, GenLength const &genLength, const std::string message,
                    std::chrono::milliseconds &averageTime, const Comparator &comparator = Comparator(),
                    const EqualComparator &equal_comparator = EqualComparator())
{
    std::chrono::milliseconds Time;
    averageTime = std::chrono::milliseconds (0);
    bool success = true;

    for (size_t i = 0; i <= testNumber - 1; ++i)
    {
        size_t localLength = genLength();
        std::cout << message << ": ";

        if (!TestSort<SortFunc, Container, Gen> (sortFunc, localLength, gen, Time, comparator, equal_comparator))
        {
            std::cout << "[-] Error: not correct! Time: ";
            std::cout << Time.count() << " ms. Length is " << localLength << std::endl;
            success = false;
            exit(1);
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

template <typename Container, typename Gen, typename GenLength,
          typename Comparator = std::less<typename Container::value_type>,
          typename EqualComparator = std::equal_to<typename Container::value_type>>
void container_test_full(const size_t test_number, Gen const &gen_test, GenLength const &gen_length, const std::string container_name,
                         const Comparator &comparator = Comparator(),const EqualComparator &equal_comparator = EqualComparator())
{
    auto testInsMCpy = [&comparator](Container &v){SortInsertionManualCopy(v.begin(), v.end(), comparator);};
    auto testInsSTLCpy = [&comparator](Container &v){SortInsertionSTLCopy(v.begin(), v.end(), comparator);};
    auto testSelection = [&comparator](Container &v){SortSelection(v.begin(), v.end(), comparator);};
    auto testHeap = [&comparator](Container &v){SortHeap(v.begin(), v.end(), comparator);};
    auto testMergeIt= [&comparator](Container &v){SortMergeIteration(v.begin(), v.end(), comparator);};
    auto testMergeRec= [&comparator](Container &v){SortMergeRec(v.begin(), v.end(), comparator);};
    auto testQuick = [&comparator](Container &v){SortQuick(v.begin(), v.end(), comparator);};

    std::chrono::milliseconds Time;

    RunTestSortAll <decltype(testInsMCpy), Container, decltype(gen_test), decltype(gen_length)>
        (testInsMCpy, test_number, gen_test, gen_length, "InsertionMCpy - " + container_name, Time, comparator, equal_comparator);

    RunTestSortAll <decltype(testInsSTLCpy), Container, decltype(gen_test), decltype(gen_length)>
        (testInsSTLCpy, test_number, gen_test, gen_length, "InsertionSTLCpy - " + container_name, Time, comparator, equal_comparator);

    RunTestSortAll <decltype(testSelection), Container, decltype(gen_test), decltype(gen_length)>
        (testSelection, test_number, gen_test, gen_length, "Selection - " + container_name, Time, comparator, equal_comparator);

    RunTestSortAll <decltype(testQuick), Container, decltype(gen_test), decltype(gen_length)>
        (testQuick, test_number, gen_test, gen_length, "QuickSort - " + container_name, Time, comparator, equal_comparator);

    RunTestSortAll <decltype(testHeap), Container, decltype(gen_test), decltype(gen_length)>
        (testHeap, test_number, gen_test, gen_length, "HeapSort - " + container_name, Time, comparator, equal_comparator);

    RunTestSortAll <decltype(testMergeIt), Container, decltype(gen_test), decltype(gen_length)>
        (testMergeIt, test_number, gen_test, gen_length, "MergeSortIt - " + container_name, Time, comparator, equal_comparator);

    RunTestSortAll <decltype(testMergeRec), Container, decltype(gen_test), decltype(gen_length)>
        (testMergeRec, test_number, gen_test, gen_length, "MergeSortRec - " + container_name, Time, comparator, equal_comparator);
}

int main()
{
    std::default_random_engine generator;
    std::chrono::milliseconds Time;

    {//vector<int>
    std::uniform_int_distribution<int> Values(1, 10);
    std::uniform_int_distribution<size_t> Lengths(0, 10000);
    auto genLen=[&](){return Lengths(generator);};
    auto genTest=[&](){return Values(generator);};

    container_test_full<std::vector<int>, decltype(genTest), decltype(genLen)>(10, genTest, genLen, "vector<int>");
    }

    {//vector<double>
    std::uniform_real_distribution<double> Values(-1000, 1000);
    std::uniform_int_distribution<size_t> Lengths(0, 10000);
    auto genLen=[&](){return Lengths(generator);};
    auto genTest=[&](){return Values(generator);};

    container_test_full<std::vector<double>, decltype(genTest), decltype(genLen)>(10, genTest, genLen, "vector<double>");
    }

    {//vector<MyStruct>
    std::uniform_int_distribution<size_t> Values(1, 10);
    std::uniform_int_distribution<char> ValuesChar('a', 'z');
    std::uniform_int_distribution<size_t> ValuesStrLength(0, 20);
    std::uniform_int_distribution<size_t> Lengths(0, 10000);
    auto genLen=[&](){return Lengths(generator);};
    auto genTest=[&](){
                        int lengthStr = ValuesStrLength(generator);

                        MyStruct element;
                        element.Key = "";
                        element.Value = Values(generator);
                        element.Key.reserve(lengthStr);

                        for (int i = 0; i <= lengthStr; i++)
                            element.Key += char(ValuesChar(generator));

                        return element;
                        };

    container_test_full<std::vector<MyStruct>, decltype(genTest), decltype(genLen)>(10, genTest, genLen, "vector<MyStruct>",
                                                                                    CompareMyStruct(), EqualMyStruct());
    }

    {//deque<int>
    std::uniform_int_distribution<int> Values(1, 1000);
    std::uniform_int_distribution<size_t> Lengths(0, 10000);
    auto genLen=[&](){return Lengths(generator);};
    auto genTest=[&](){return Values(generator);};

    container_test_full<std::deque<int>, decltype(genTest), decltype(genLen)>(10, genTest, genLen, "deque<int>");
    }

    return 0;
}
