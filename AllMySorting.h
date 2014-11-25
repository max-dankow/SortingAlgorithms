#include <vector>
#include <algorithm>
#include <random>

template<typename Iterator, typename Comparator=std::less
         <typename std::iterator_traits<Iterator>::value_type>>
extern void SortInsertionManualCopy(Iterator Begin, Iterator End, const Comparator &comparator = Comparator())
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

static std::default_random_engine QuickSortLocalGen;
static const size_t CUT_OFF = 25;

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
        if ((size_t)(End - Begin) <= CUT_OFF)
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
                           const Comparator &comparator)
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
