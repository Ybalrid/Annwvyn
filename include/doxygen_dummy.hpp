//This file should not be #included anywere:

namespace std
{
  ///standard shared pointer from the <memory> header. Dummy redefiniton.
  template<class T> class shared_ptr{ T* dummy };
  ///standard weak pointer from the <memory> header. Dummy redefiniton.
  template<class T> class weak_ptr{ T* dummy };
  ///standard unique pointer from the <memory> header. Dummy redefiniton.
  template<class T> class unique_ptr{ T* dummy };
  
}
