struct CBitset
{
  unsigned int data;

  void set(const unsigned int index, const bool i_b = true)
  {
    const int iSetBit = 1 << index;
    if(i_b)
    {
      data |= iSetBit;
    }
    else
    {
      data &= ~iSetBit;
    }
  }

  bool get(const unsigned int index)
  {
    return (data & (1 << index)) != 0;
  }
};
