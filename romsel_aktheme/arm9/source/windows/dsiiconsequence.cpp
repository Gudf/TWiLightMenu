#include "dsiiconsequence.h"
#include <nds.h>

DSiIconSequence::DSiIconSequence()
{
    memset(_gameTid, 0, sizeof(_gameTid));
    memset(_sequence, 0, sizeof(_sequence));
    _flipH = false;
    _flipV = false;
    _currentSequenceIndex = 0;
    _updatesSinceNewIndex = 0;
    _bitmapIndex = 0;
    _paletteIndex = 0;
}

void DSiIconSequence::update()
{
    if (!_visible || (_sequence[0] == 0x0001 && _sequence[1] == 0x0100))
        return;

    if (_sequence[_currentSequenceIndex] == 0x0000)
    {
        reset();
    }

    _flipH = SEQ_FLIPH(_sequence[_currentSequenceIndex]);
    _flipV = SEQ_FLIPV(_sequence[_currentSequenceIndex]);
    _bitmapIndex = SEQ_BMP(_sequence[_currentSequenceIndex]);
    _paletteIndex = SEQ_PAL(_sequence[_currentSequenceIndex]);
    _updatesSinceNewIndex++;
    if (_updatesSinceNewIndex >= SEQ_DUR(_sequence[_currentSequenceIndex]))
    {    
        _currentSequenceIndex++;
        _updatesSinceNewIndex = 0;
    }
}

void DSiIconSequence::reset()
{
    _currentSequenceIndex = 0;
}

DSiIconSequence::~DSiIconSequence()
{
}

int IconSequenceManager::allocate_sequence(u8 *gameTid, u16 *sequence)
{

    int cached;
    if ((cached = is_cached(gameTid)) != -1)
        return cached;

    dbg_printf("need to reallocate...");
    // We need to allocate something now... How do we do that?
    // Rotate left, so the the previously first item is now last.
    std::rotate(_dsiIconSequence.begin(), _dsiIconSequence.begin() + 1, _dsiIconSequence.end());

    size_t index = _dsiIconSequence.size() - 1;

    memset(seq()._dsiIconSequence[index].gameTid(), 0, SIZE_GAMETID);
    memset(seq()._dsiIconSequence[index].sequence(), 0, SIZE_SEQUENCE);

    memcpy(seq()._dsiIconSequence[index].gameTid(), gameTid, SIZE_GAMETID);
    memcpy(seq()._dsiIconSequence[index].sequence(), sequence, SIZE_SEQUENCE);

    seq()._dsiIconSequence[index].reset();
    seq()._dsiIconSequence[index].show();

    // Allow the last item to be replaced.
    return index;
}

int IconSequenceManager::is_cached(u8 *gameTid)
{
    for (size_t i = 0; i < _dsiIconSequence.size(); i++)
    {
        if (memcmp(gameTid, _dsiIconSequence[i].gameTid(), SIZE_GAMETID) == 0)
        {
            return i;
        }
    }
    return -1;
}