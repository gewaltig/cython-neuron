

#include <iostream>
#include "iteratordatum.h"


sli::pool IteratorDatum::memory(sizeof(IteratorDatum),10240,1);



  // << and == operators



std::ostream& operator<<(std::ostream& o, const IteratorState &is)
{
 o << "[" << is.start << " " << is.stop << " " << is.di << "]";
 return o;
}




