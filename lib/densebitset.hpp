// NOTE: This source is taken from GraphChi 1.0
// NOTE, copied from GraphLab v 0.5

#ifndef __DENSE_BITSET_HPP__
#define __DENSE_BITSET_HPP__
#include <cstdio>
#include <cstdlib>
#include <stdint.h>

#include "utils.h"

class dense_bitset {
  public:
    dense_bitset() :
      array(NULL), len(0), arrlen(0) {
      }

    dense_bitset(IdType size) :
      array(NULL), len(size) {
        resize(size);
        clear();
      }

    virtual ~dense_bitset() {
      free(array);
    }

    void resize(IdType n) {
      len = n;
      //need len bits
      arrlen = n / (8 * sizeof(IdType)) + 1;
      array = (IdType*) realloc(array, sizeof(IdType) * arrlen);
    }

    void clear() {
      memset(array, 0, arrlen * sizeof(IdType));
    }

    void setall() {
      memset(array, 0xff, arrlen * sizeof(IdType));
    }

    // endBit is exclusive
    inline bool compareRangeSubset(dense_bitset* otherBitSet, IdType startBit, IdType endBit) {
      IdType arrpos, bitpos, endarrpos, endbitpos;

      bit_to_pos(startBit, arrpos, bitpos);
      bit_to_pos(endBit, endarrpos, endbitpos);

      IdType currBit = startBit;

      while (bitpos < 8 * sizeof(IdType)) {
        bool b1 = array[arrpos] & (IdType(1) << IdType(bitpos));
        if (b1) {
          bool b2 = (otherBitSet->getArray()[arrpos]) & (IdType(1) << IdType(bitpos));
          if (!b2)
            return false;
        }

        ++bitpos;
        ++currBit;
        if (currBit == endBit)
          return true;
      }

      bit_to_pos(currBit, arrpos, bitpos);

      while (arrpos < endarrpos) {
        IdType andAnswer = (array[arrpos]) & (otherBitSet->getArray()[arrpos]);
        if (array[arrpos] != andAnswer)
          return false;

        ++arrpos;
        currBit += (8 *sizeof(IdType));
      }

      bit_to_pos(currBit, arrpos, bitpos);
      assert(arrpos == endarrpos);

      while (currBit < endBit) {
        bool b1 = array[arrpos] & (IdType(1) << IdType(bitpos));
        if (b1) {
          bool b2 = (otherBitSet->getArray()[arrpos]) & (IdType(1) << IdType(bitpos));
          if (!b2)
            return false;
        }

        ++bitpos;
        ++currBit;
      }

      return true;
    }

    inline bool get(IdType b) const {
      IdType arrpos, bitpos;
      bit_to_pos(b, arrpos, bitpos);
      return array[arrpos] & (IdType(1) << IdType(bitpos));
    }

    //! Set the bit returning the old value
    inline bool set_bit(IdType b) {
      // use CAS to set the bit
      IdType arrpos, bitpos;
      bit_to_pos(b, arrpos, bitpos);
      const IdType mask(IdType(1) << IdType(bitpos));
      return __sync_fetch_and_or(array + arrpos, mask) & mask;
    }

    //! Set the state of the bit returning the old value
    inline bool set(IdType b, bool value) {
      if (value)
        return set_bit(b);
      else
        return clear_bit(b);
    }

    //! Clear the bit returning the old value
    inline bool clear_bit(IdType b) {
      // use CAS to set the bit
      IdType arrpos, bitpos;
      bit_to_pos(b, arrpos, bitpos);
      const IdType test_mask(IdType(1) << IdType(bitpos));
      const IdType clear_mask(~test_mask);
      return __sync_fetch_and_and(array + arrpos, clear_mask) & test_mask;
    }

    inline void clear_bits(IdType fromb, IdType tob) { // tob is inclusive
      // Careful with alignment
      const IdType bitsperword = sizeof(IdType) * 8;
      while ((fromb % bitsperword != 0)) {
        clear_bit(fromb);
        if (fromb >= tob)
          return;
        fromb++;
      }

      while ((tob % bitsperword != 0)) {
        clear_bit(tob);
        if (tob <= fromb)
          return;
        tob--;
      }
      clear_bit(tob);

      IdType from_arrpos = fromb / (8 * (int) sizeof(IdType));
      IdType to_arrpos = tob / (8 * (int) sizeof(IdType));
      memset(&array[from_arrpos], 0, (to_arrpos - from_arrpos) * (int) sizeof(IdType));
    }

    inline IdType size() const {
      return len;
    }

    inline const IdType* getArray() const {
      return array;
    }

    inline IdType countSetBits() const {
      IdType ret = 0;

      for(IdType i=0; i<arrlen; ++i) {
        IdType val = 0x0000000000000001;
        for(unsigned int j=0; j<sizeof(IdType) * 8; ++j) {
          if(array[i] & val)
            ++ret;

          val = val << 1;
        }
      }

      return std::min(len, ret);
    }

  private:

    inline static void bit_to_pos(IdType b, IdType &arrpos, IdType &bitpos) {
      // the compiler better optimize this...
      arrpos = b / (8 * (int) sizeof(IdType));
      bitpos = b & (8 * (int) sizeof(IdType) - 1);
    }

    IdType* array;
    IdType len;
    IdType arrlen;
};

#endif // __DENSE_BITSET_HPP__
