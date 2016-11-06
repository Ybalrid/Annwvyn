//This file should not be #included anywere:

namespace std
{
	///standard shared pointer from the <em>memory</em> header. Dummy redefiniton.
	template<class T> class shared_ptr { T* someType };
	///standard weak pointer from the <em>memory</em> header. Dummy redefiniton.
	template<class T> class weak_ptr { T* someType };
	///standard unique pointer from the <em>memory</em>  header. Dummy redefiniton.
	template<class T> class unique_ptr { T* someType };

	///C++ standard string
	class string {};
}
