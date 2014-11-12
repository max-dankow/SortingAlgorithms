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
    if ((testEnd - testBegin) != (perfEnd - perfBegin))
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
bool TestSort(const SortFunc &sortFunc, size_t length, const Gen &gen, std::chrono::milliseconds &workTime)
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

    workTime = std::chrono::milliseconds(std::chrono::duration_cast<std::chrono::milliseconds>(TEnd - TStart).count());

    return CheckCorrect(data.begin(), data.end(), perfect.begin(), perfect.end());
}

template <typename SortFunc, typename Container, typename Gen, typename GenLength>
void RunTestSortAll(const SortFunc sortFunc, size_t testNumber, Gen const &gen, GenLength const &genLength,
                    const std::string message, std::chrono::milliseconds &averageTime)
{
    std::chrono::milliseconds Time;
    averageTime = std::chrono::milliseconds (0);
    bool success = true;

    for (size_t i = 0; i <= testNumber - 1; ++i)
    {
        size_t localLength = genLength();
        std::cout << message << ": ";

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

int main()
{
    std::default_random_engine generator;
    std::chrono::milliseconds Time;

    {//vector<int> 1..10
    const size_t TEST_NUMBER = 10;
    std::uniform_int_distribution<int> Values(1, 10);
    std::uniform_int_distribution<size_t> Lengths(0, 1000);
    auto genLen=[&](){return Lengths(generator);};
    auto genTest=[&](){return Values(generator);};

    auto testInsMCpy = [](std::vector<int> &v){SortInsertionManualCopy(v.begin(), v.end());};
    auto testInsSTLCpy = [](std::vector<int> &v){SortInsertionSTLCopy(v.begin(), v.end());};
    auto testSelection = [](std::vector<int> &v){SortSelection(v.begin(), v.end());};
    auto testHeap = [](std::vector<int> &v){SortHeap(v.begin(), v.end());};
    auto testMergeIt= [](std::vector<int> &v){SortMergeIteration(v.begin(), v.end());};
    auto testMergeRec= [](std::vector<int> &v){SortMergeRec(v.begin(), v.end());};
    auto testQuick = [](std::vector<int> &v){SortQuick(v.begin(), v.end());};

    RunTestSortAll <decltype(testInsMCpy), std::vector<int>, decltype(genTest), decltype(genLen)>
        (testInsMCpy, TEST_NUMBER, genTest, genLen, "InsertionMCpy - vector<int>", Time);

    RunTestSortAll <decltype(testInsSTLCpy), std::vector<int>, decltype(genTest), decltype(genLen)>
        (testInsSTLCpy, TEST_NUMBER, genTest, genLen, "InsertionSTLCpy - vector<int>", Time);

    RunTestSortAll <decltype(testSelection), std::vector<int>, decltype(genTest), decltype(genLen)>
        (testSelection, TEST_NUMBER, genTest, genLen, "Selection - vector<int>", Time);

    RunTestSortAll <decltype(testQuick), std::vector<int>, decltype(genTest), decltype(genLen)>
        (testQuick, TEST_NUMBER, genTest, genLen, "QuickSort - vector<int>", Time);

    RunTestSortAll <decltype(testHeap), std::vector<int>, decltype(genTest), decltype(genLen)>
        (testHeap, TEST_NUMBER, genTest, genLen, "HeapSort - vector<int>", Time);

    RunTestSortAll <decltype(testMergeIt), std::vector<int>, decltype(genTest), decltype(genLen)>
        (testMergeIt, TEST_NUMBER, genTest, genLen, "MergeSortIt - vector<int>", Time);

    RunTestSortAll <decltype(testMergeRec), std::vector<int>, decltype(genTest), decltype(genLen)>
        (testMergeRec, TEST_NUMBER, genTest, genLen, "MergeSortRec - vector<int>", Time);
    }

    {//vector<double>
    const size_t TEST_NUMBER = 10;
    std::uniform_real_distribution<double> Values(-1000, 1000);
    std::uniform_int_distribution<size_t> Lengths(0, 10000);
    auto genLen=[&](){return Lengths(generator);};
    auto genTest=[&](){return Values(generator);};

    auto testInsMCpy = [](std::vector<double> &v){SortInsertionManualCopy(v.begin(), v.end());};
    auto testInsSTLCpy = [](std::vector<double> &v){SortInsertionSTLCopy(v.begin(), v.end());};
    auto testSelection = [](std::vector<double> &v){SortSelection(v.begin(), v.end());};
    auto testHeap = [](std::vector<double> &v){SortHeap(v.begin(), v.end());};
    auto testMergeIt= [](std::vector<double> &v){SortMergeIteration(v.begin(), v.end());};
    auto testMergeRec= [](std::vector<double> &v){SortMergeRec(v.begin(), v.end());};
    auto testQuick = [](std::vector<double> &v){SortQuick(v.begin(), v.end());};

    RunTestSortAll <decltype(testInsMCpy), std::vector<double>, decltype(genTest), decltype(genLen)>
        (testInsMCpy, TEST_NUMBER, genTest, genLen, "InsertionMCpy - vector<double>", Time);

    RunTestSortAll <decltype(testInsSTLCpy), std::vector<double>, decltype(genTest), decltype(genLen)>
        (testInsSTLCpy, TEST_NUMBER, genTest, genLen, "InsertionSTLCpy - vector<double>", Time);

    RunTestSortAll <decltype(testSelection), std::vector<double>, decltype(genTest), decltype(genLen)>
        (testSelection, TEST_NUMBER, genTest, genLen, "Selection - vector<double>", Time);

    RunTestSortAll <decltype(testQuick), std::vector<double>, decltype(genTest), decltype(genLen)>
        (testQuick, TEST_NUMBER, genTest, genLen, "QuickSort - vector<double>", Time);

    RunTestSortAll <decltype(testHeap), std::vector<double>, decltype(genTest), decltype(genLen)>
        (testHeap, TEST_NUMBER, genTest, genLen, "HeapSort - vector<double>", Time);

    RunTestSortAll <decltype(testMergeIt), std::vector<double>, decltype(genTest), decltype(genLen)>
        (testMergeIt, TEST_NUMBER, genTest, genLen, "MergeSortIt - vector<double>", Time);

    RunTestSortAll <decltype(testMergeRec), std::vector<double>, decltype(genTest), decltype(genLen)>
        (testMergeRec, TEST_NUMBER, genTest, genLen, "MergeSortRec - vector<double>", Time);
    }

    {//vector<MyStruct>
    const size_t TEST_NUMBER = 10;
    std::uniform_int_distribution<size_t> Values(1, 10);
    std::uniform_int_distribution<char> ValuesChar('a', 'z');
    std::uniform_int_distribution<size_t> ValuesStrLength(0, 20);
    std::uniform_int_distribution<size_t> Lengths(0, 1000);
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

    auto testInsMCpy = [](std::vector<MyStruct> &v){SortInsertionManualCopy(v.begin(), v.end());};
    auto testInsSTLCpy = [](std::vector<MyStruct> &v){SortInsertionSTLCopy(v.begin(), v.end());};
    auto testSelection = [](std::vector<MyStruct> &v){SortSelection(v.begin(), v.end());};
    auto testHeap = [](std::vector<MyStruct> &v){SortHeap(v.begin(), v.end());};
    auto testMergeIt= [](std::vector<MyStruct> &v){SortMergeIteration(v.begin(), v.end());};
    auto testMergeRec= [](std::vector<MyStruct> &v){SortMergeRec(v.begin(), v.end());};
    auto testQuick = [](std::vector<MyStruct> &v){SortQuick(v.begin(), v.end());};

    RunTestSortAll <decltype(testInsMCpy), std::vector<MyStruct>, decltype(genTest), decltype(genLen)>
        (testInsMCpy, TEST_NUMBER, genTest, genLen, "InsertionMCpy - vector<MyStruct>", Time);

    RunTestSortAll <decltype(testInsSTLCpy), std::vector<MyStruct>, decltype(genTest), decltype(genLen)>
        (testInsSTLCpy, TEST_NUMBER, genTest, genLen, "InsertionSTLCpy - vector<MyStruct>", Time);

    RunTestSortAll <decltype(testSelection), std::vector<MyStruct>, decltype(genTest), decltype(genLen)>
        (testSelection, TEST_NUMBER, genTest, genLen, "Selection - vector<MyStruct>", Time);

    RunTestSortAll <decltype(testQuick), std::vector<MyStruct>, decltype(genTest), decltype(genLen)>
        (testQuick, TEST_NUMBER, genTest, genLen, "QuickSort - vector<MyStruct>", Time);

    RunTestSortAll <decltype(testHeap), std::vector<MyStruct>, decltype(genTest), decltype(genLen)>
        (testHeap, TEST_NUMBER, genTest, genLen, "HeapSort - vector<MyStruct>", Time);

    RunTestSortAll <decltype(testMergeIt), std::vector<MyStruct>, decltype(genTest), decltype(genLen)>
        (testMergeIt, TEST_NUMBER, genTest, genLen, "MergeSortIt - vector<MyStruct>", Time);

    RunTestSortAll <decltype(testMergeRec), std::vector<MyStruct>, decltype(genTest), decltype(genLen)>
        (testMergeRec, TEST_NUMBER, genTest, genLen, "MergeSortRec - vector<MyStruct>", Time);
    }

    {//deque<int>
    const size_t TEST_NUMBER = 10;
    std::uniform_int_distribution<int> Values(1, 1000);
    std::uniform_int_distribution<size_t> Lengths(0, 1000);
    auto genLen=[&](){return Lengths(generator);};
    auto genTest=[&](){return Values(generator);};

    auto testInsMCpy = [](std::deque<int>  &v){SortInsertionManualCopy(v.begin(), v.end());};
    auto testInsSTLCpy = [](std::deque<int>  &v){SortInsertionSTLCopy(v.begin(), v.end());};
    auto testSelection = [](std::deque<int>  &v){SortSelection(v.begin(), v.end());};
    auto testHeap = [](std::deque<int>  &v){SortHeap(v.begin(), v.end());};
    auto testMergeIt= [](std::deque<int>  &v){SortMergeIteration(v.begin(), v.end());};
    auto testMergeRec= [](std::deque<int>  &v){SortMergeRec(v.begin(), v.end());};
    auto testQuick = [](std::deque<int>  &v){SortQuick(v.begin(), v.end());};

    RunTestSortAll <decltype(testInsMCpy), std::deque<int> , decltype(genTest), decltype(genLen)>
        (testInsMCpy, TEST_NUMBER, genTest, genLen, "InsertionMCpy - deque<int> ", Time);

    RunTestSortAll <decltype(testInsSTLCpy), std::deque<int> , decltype(genTest), decltype(genLen)>
        (testInsSTLCpy, TEST_NUMBER, genTest, genLen, "InsertionSTLCpy - deque<int> ", Time);

    RunTestSortAll <decltype(testSelection), std::deque<int> , decltype(genTest), decltype(genLen)>
        (testSelection, TEST_NUMBER, genTest, genLen, "Selection - deque<int> ", Time);

    RunTestSortAll <decltype(testQuick), std::deque<int> , decltype(genTest), decltype(genLen)>
        (testQuick, TEST_NUMBER, genTest, genLen, "QuickSort - deque<int> ", Time);

    RunTestSortAll <decltype(testHeap), std::deque<int> , decltype(genTest), decltype(genLen)>
        (testHeap, TEST_NUMBER, genTest, genLen, "HeapSort - deque<int> ", Time);

    RunTestSortAll <decltype(testMergeIt), std::deque<int> , decltype(genTest), decltype(genLen)>
        (testMergeIt, TEST_NUMBER, genTest, genLen, "MergeSortIt - deque<int> ", Time);

    RunTestSortAll <decltype(testMergeRec), std::deque<int> , decltype(genTest), decltype(genLen)>
        (testMergeRec, TEST_NUMBER, genTest, genLen, "MergeSortRec - deque<int> ", Time);
    }

}
