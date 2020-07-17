/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Ihor Szlachtycz $
   $Notice: (C) Copyright 2014 by Dream.Inc, Inc. All Rights Reserved. $
   ======================================================================== */

inline void SortRadix(linear_arena* TempArena, sort_key* SortKeys, u32 NumSortKeys)
{
    temp_mem TempMem = BeginTempMem(TempArena);
    
    sort_key* TempKeys = PushArray(TempArena, sort_key, NumSortKeys);

    sort_key* CurrKeys = SortKeys;
    sort_key* NextKeys = TempKeys;
    for (u32 ByteId = 0; ByteId < 32; ByteId += 8)
    {
        u32 SortKeyCounts[256] = {};
        
        // NOTE: First we find out how many of each key we have
        for (u32 Id = 0; Id < NumSortKeys; ++Id)
        {
            sort_key* Key = CurrKeys + Id;
            u32 BucketId = (Key->Key >> ByteId) & 0xFF;
            ++SortKeyCounts[BucketId];
        }

        // NOTE: Figure out offsets for our elements
        u32 SortKeyOffsets[256] = {};
        u32 CurrOffset = 0;
        for (u32 OffsetId = 0; OffsetId < ArrayCount(SortKeyOffsets); ++OffsetId)
        {
            SortKeyOffsets[OffsetId] = CurrOffset;
            CurrOffset += SortKeyCounts[OffsetId];
        }

        // NOTE: Place elements into their buckets
        for (u32 Id = 0; Id < NumSortKeys; ++Id)
        {
            sort_key* Key = CurrKeys + Id;
            u32 BucketId = (Key->Key >> ByteId) & 0xFF;

            NextKeys[SortKeyOffsets[BucketId]++] = *Key;
        }

        // NOTE: Swap the arrays we are working with
        {
            sort_key* Temp = CurrKeys;
            CurrKeys = NextKeys;
            NextKeys = Temp;
        }
    }

    EndTempMem(TempMem);
}
