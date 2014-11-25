#include <iostream>
#include <vector>
#include <cstdio>
#include <algorithm>
#include <chrono>
#include <random>
#include <deque>
#include <string>

#include "AllMySorting.h"
#include "TestingSort.h"

struct MyStruct
{
    std::string Key;
    double Value;
};

bool operator== (const MyStruct &a, const MyStruct &b)
{
    return (a.Key == b.Key) && (a.Value == b.Value);
}

struct CompareMyStruct
{
    bool operator()(const MyStruct &left, const MyStruct &right)const
    {
        return left.Key > right.Key;
    }
};

int main()
{
    std::default_random_engine generator;
    std::chrono::milliseconds Time;

    {//vector<int> 1..10
    const size_t TEST_NUMBER = 10;
    std::uniform_int_distribution<int> Values(1, 10);
    std::uniform_int_distribution<size_t> Lengths(0, 6000);
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
    std::uniform_int_distribution<size_t> Lengths(0, 6000);
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
    std::uniform_int_distribution<size_t> Lengths(0, 6000);
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

    auto testInsMCpy = [](std::vector<MyStruct> &v){SortInsertionManualCopy(v.begin(), v.end(), CompareMyStruct());};
    auto testInsSTLCpy = [](std::vector<MyStruct> &v){SortInsertionSTLCopy(v.begin(), v.end(), CompareMyStruct());};
    auto testSelection = [](std::vector<MyStruct> &v){SortSelection(v.begin(), v.end(), CompareMyStruct());};
    auto testHeap = [](std::vector<MyStruct> &v){SortHeap(v.begin(), v.end(), CompareMyStruct());};
    auto testMergeIt= [](std::vector<MyStruct> &v){SortMergeIteration(v.begin(), v.end(), CompareMyStruct());};
    auto testMergeRec= [](std::vector<MyStruct> &v){SortMergeRec(v.begin(), v.end(), CompareMyStruct());};
    auto testQuick = [](std::vector<MyStruct> &v){SortQuick(v.begin(), v.end(), CompareMyStruct());};

    RunTestSortAll <decltype(testInsMCpy), std::vector<MyStruct>, decltype(genTest), decltype(genLen)>
        (testInsMCpy, TEST_NUMBER, genTest, genLen, "InsertionMCpy - vector<MyStruct>", Time, CompareMyStruct());

    RunTestSortAll <decltype(testInsSTLCpy), std::vector<MyStruct>, decltype(genTest), decltype(genLen)>
        (testInsSTLCpy, TEST_NUMBER, genTest, genLen, "InsertionSTLCpy - vector<MyStruct>", Time, CompareMyStruct());

    RunTestSortAll <decltype(testSelection), std::vector<MyStruct>, decltype(genTest), decltype(genLen)>
        (testSelection, TEST_NUMBER, genTest, genLen, "Selection - vector<MyStruct>", Time, CompareMyStruct());

    RunTestSortAll <decltype(testQuick), std::vector<MyStruct>, decltype(genTest), decltype(genLen)>
        (testQuick, TEST_NUMBER, genTest, genLen, "QuickSort - vector<MyStruct>", Time, CompareMyStruct());

    RunTestSortAll <decltype(testHeap), std::vector<MyStruct>, decltype(genTest), decltype(genLen)>
        (testHeap, TEST_NUMBER, genTest, genLen, "HeapSort - vector<MyStruct>", Time, CompareMyStruct());

    RunTestSortAll <decltype(testMergeIt), std::vector<MyStruct>, decltype(genTest), decltype(genLen)>
        (testMergeIt, TEST_NUMBER, genTest, genLen, "MergeSortIt - vector<MyStruct>", Time, CompareMyStruct());

    RunTestSortAll <decltype(testMergeRec), std::vector<MyStruct>, decltype(genTest), decltype(genLen)>
        (testMergeRec, TEST_NUMBER, genTest, genLen, "MergeSortRec - vector<MyStruct>", Time, CompareMyStruct());
    }

    {//deque<int>
    const size_t TEST_NUMBER = 10;
    std::uniform_int_distribution<int> Values(1, 1000);
    std::uniform_int_distribution<size_t> Lengths(0, 6000);
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

    return 0;
}
