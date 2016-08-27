//This file should not be #included anywere:

namespace std
{
  template<class T> class shared_ptr{ T* dummy };
  template<class T> class weak_ptr{ T* dummy };
}
