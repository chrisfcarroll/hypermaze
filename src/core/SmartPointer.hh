/**
 * @file
 * @brief This file provides two smart pointer classes
 */

#ifndef SMARTPOINTER_HH_INC
#define SMARTPOINTER_HH_INC

#ifdef IOSTREAM
#include <iostream>
#endif
#include <cstring>


///A Smart pointer class
/**
 * This class provides a basic smart pointer class that reference
 * tracks it's argument and deletes when no pointers are left
 * @tparam T the type of the data stored in the smart pointer
 */
template <class T>
class SP{
  protected:
    T* p;///< The data pointer
    int* c;///< The reference count
  public:
    ///Construct a new smart pointer pointing to null
    SP<T>():c(new int(1)),p(0){};
    ///Construct a smart pointer pointing to the provided data
    /**
     * The provided pointer should be a freshly created pointer that nothing else
     * should try to delete or access unless directly unless you ensure that a smart pointer is also kept
     * @param p the pointer to the data to be managed
     */
    explicit SP<T>(T* p):c(new int(1)),p(p){};

    ///Copy Constructor
    /**
     * Creates a new copy of the smart pointer refering to the same data and reference count and
     * increments the reference count
     * @param op the smart pointer to copy
     */
    SP<T>(const SP<T>& op):p(op.p),c(op.c){++*c;}

    ///Copy Constructor
    /**
     * Creates a new copy of the smart pointer refering to the same data and reference count and
     * increments the reference count
     * @param op the smart pointer to copy
     * @tparam U the type of the pointer we are copying
     */
    template <class U>
    explicit SP<T>(const SP<U>& op):p((T*)op.p),c(op.c){++*c;}

    template <class U>
    friend class SP;

    ///Destructor for pointer
    /**
     * Destroys the pointer and updates the reference count. If this is the last pointer then delete the
     * data we point to as well
     */
    ~SP<T>(){
      if(--*c==0){
        delete p;
        delete c;
      }
    }

    ///Assignment operator
    /**
     * Updates the reference count for the current data and deletes if needed and then replaces with
     * value pointed to by other pointer
     * @param op the pointer to replace this one with
     * @return a reference to this pointer
     */
    SP<T>& operator=(const SP<T>& op){
      if(p==op.p && op.c==op.c)
        return *this;
      if(--*c==0){
        delete p;
        delete c;
      }
      p=op.p;
      c=op.c;
      ++*c;

      return *this;
    }

    ///Type Cast Assignment operator
    /**
     * Updates the reference count for the current data and deletes if needed and then replaces with
     * value pointed to by other pointer
     * @param op the pointer to replace this one with
     * @return a reference to this pointer
     */
    template<class U>
    SP<T>& operator=(const SP<U>& op){
      if(p==op.p && op.c==op.c)
        return *this;
      if(--*c==0){
        delete p;
        delete c;
      }
      p=(T*)op.p;
      c=op.c;
      ++*c;

      return *this;
    }

    ///Dereference the pointer
    /**
     * @return a reference to the element we point to
     */
    T& operator* () const{
      return *p;
    }

    ///Dereference the pointer to a member
    /**
     * @return A pointer that can be used to access the member requested
     */
    T* operator-> () const{
      return p;
    }

    ///Compare this pointer to another pointer
    /**
     * Returns true if the other pointer points to the same memory location as this pointer
     * @param op the other pointer to compare to this pointer
     * @return true if they point to the same memory location
     */
    const bool operator==(const  SP<T>& op)const{
      return p==op.p;
    }

    ///Check if this is a null pointer
    /**
     * @return true if this is a null pointer
     */
    const bool isnull() const{
      return p==0;
    }

    #ifdef IOSTREAM
    template<class U>
    friend std::ostream& operator<<(std::ostream&,const SP<U>&);
    #endif

};

///A Smart array pointer class
/**
 * This class provides a more advanced smart pointer class that reference
 * tracks a c style array acting as a pointer to a location in the array
 * and deletes the entire array when no pointers are left
 * @tparam T the type of the data stored in the smart pointer
 */
template <class T>
class SPA{
  protected:
    T* p;///< The data pointer
    T* h;///< The head of the array
    int* c;///< The reference count

  public:
    ///Construct a new smart pointer pointing to null
    SPA<T>():c(new int(1)),p(0),h(0){};
    ///Construct a new smart pointer for a new array of len elements
    /**
     * this will point to the first element in the new array
     * (explicit to stop this being used as a cast from an integer as that doesn't make sence)
     * @param len the number of elements for the new array to contain
     */
    explicit SPA<T>(const int& len):c(new int(1)),p(new T[len]()),h(p){};
    ///Copy Constructor
    /**
     * Creates a new copy of the smart pointer refering to the same data and reference count and
     * increments the reference count
     * @param op the smart pointer to copy
     */
    SPA<T>(const SPA<T>& op):p(op.p),c(op.c),h(op.h){++*c;}
    ///Copy Constructor
    /**
     * Creates a new copy of the smart pointer refering to the same data and reference count and
     * increments the reference count
     * @param op the smart pointer to copy
     * @tparam U the type of the pointer we are copying
     */
    template <class U>
    SPA<T>(const SPA<U>& op):p(op.p),c(op.c),h(op.h){++*c;}

    ///Declare friend so it can access the data pointer for the wrong type
    template <class U>
    friend class SPA;

    ///Destructor for pointer
    /**
     * Destroys the pointer and updates the reference count. If this is the last pointer then delete the
     * data we point to as well
     */
    ~SPA<T>(){
      if(--*c==0){
        delete[] h;
        delete c;
      }
    }

    ///Assignment operator
    /**
     * Updates the reference count for the current data and deletes if needed and then replaces with
     * value pointed to by other pointer
     * @param op the pointer to replace this one with
     * @return a reference to this pointer
     */
    SPA<T>& operator=(const SPA<T>& op){
      if(p==op.p && h==op.h && c==op.c)//if they are identical then do nothing
        return *this;
      ++*op.c;//claim op's data before freeing own in case they are part of the same array
      if(--*c==0){
        delete[] h;
        delete c;
      }
      p=op.p;
      c=op.c;
      h=op.h;
      return *this;
    }

    ///Dereference the pointer
    /**
     * @return a reference to the element we point to
     */
    T& operator* () const{
      return *p;
    }

    ///Dereference the pointer to a member
    /**
     * @return A pointer that can be used to access the member requested
     */
    T* operator-> () const{
      return p;
    }
    ///Derefence the array pointer to an element relative to element we point to
    /**
     * @param i the position relative to the element this points to to return
     * @return a reference to the element
     */
    T& operator[](const int i) const{
      return p[i];
    }
    ///Get a new pointer to the element i after the one this points to
    /**
     * @param i the position relative to this pointers element we want to return a pointer to
     * @return a new pointer that points i elements after this one
     */
    SPA<T> operator+(const int i) const{
      return SPA<T>(*this)+=i;
    }
    ///Move this pointer to the element i after the one it currently points to
    /**
     * Updates the pointer it's called on and returns a reference to itself
     * @param i how many positions to move this pointer forwards by
     * @return a reference to this pointer
     */
    SPA<T>& operator+=(const int i){
      p+=i;
      return *this;
    }
    ///Get a new pointer to the element i before the one this points to
    /**
     * @param i the position relative to this pointers element we want to return a pointer to
     * @return a new pointer that points i elements before this one
     */
    SPA<T> operator-(const int i) const{
      return SPA<T>(*this)-=i;
    }
    ///Move this pointer to the element i before the one it currently points to
    /**
     * Updates the pointer it's called on and returns a reference to itself
     * @param i how many positions to move this pointer backwards by
     * @return a reference to this pointer
     */
    SPA<T>& operator-=(const int i){
      p-=i;
      return *this;
    }
    ///Increment this pointer by one
    /**
     * Updates this pointer and returns a reference to itself
     * @return a reference to this pointer
     */
    SPA<T>& operator++(){
      ++p;
      return *this;
    }
    ///Decrement this pointer by one
    /**
     * Updates this pointer and returns a reference to itself
     * @return a reference to this pointer
     */
    SPA<T>& operator--(){
      --p;
      return *this;
    }
    ///Increment this pointer by one and return a copy of the old value
    /**
     * Updates this pointer and returns a reference to itself
     * @return a reference to this pointer
     */
    SPA<T> operator++(int){
      SPA<T> tmp(*this);
      ++p;
      return tmp;
    }
    ///Decrement this pointer by one and return a copy of the old value
    /**
     * Updates this pointer and returns a reference to itself
     * @return a reference to this pointer
     */
    SPA<T> operator--(int){
      SPA<T> tmp(*this);
      --p;
      return tmp;
    }
    ///Compare this pointer to another pointer
    /**
     * Returns true if the other pointer points to the same memory location as this pointer
     * @param op the other pointer to compare to this pointer
     * @return true if they point to the same memory location
     */
    const bool operator==(const  SPA<T>& op)const{
      return h==op.h && p==op.p;
    }
    ///Compare this pointer to another pointer
    /**
     * Returns false if the other pointer points to the same memory location as this pointer
     * @param op the other pointer to compare to this pointer
     * @return true if they point to different memory location
     */
    const bool operator!=(const  SPA<T>& op)const{
      return h!=op.h || p!=op.p;
    }

    ///Compare this pointer to another pointer
    /**
     * Returns true of the other pointer points to a location that compares less than the location
     * this points to
     * @param op the other pointer to compare to this pointer
     * @return true if they compare as less
     */
    const bool operator <(const SPA<T>& op)const{
      return p<op.p;
    }
    ///Compare this pointer to another pointer
    /**
     * Returns true of the other pointer points to a location that compares greater than the location
     * this points to
     * @param op the other pointer to compare to this pointer
     * @return true if they compare as greater
     */
    const bool operator >(const SPA<T>& op)const{
      return p>op.p;
    }
    ///Compare this pointer to another pointer
    /**
     * Returns true of the other pointer points to a location that compares less than or equal to the location
     * this points to
     * @param op the other pointer to compare to this pointer
     * @return true if they compare as less than or equal
     */
    const bool operator <=(const SPA<T>& op)const{
      return p<=op.p;
    }
    ///Compare this pointer to another pointer
    /**
     * Returns true of the other pointer points to a location that compares greater than or equal to the location
     * this points to
     * @param op the other pointer to compare to this pointer
     * @return true if they compare as greater than or equal
     */
    const bool operator >=(const SPA<T>& op)const{
      return p>=op.p;
    }
    ///Check if this is a null pointer
    /**
     * @return true if this is a null pointer
     */
    const bool isnull() const{
      return p==0;
    }

    /// Cast this to a const version
    /**
     * @return a const version of this SPA
     */
    operator SPA<const T>(){
      return SPA<const T>(*this);
    }

    #ifdef IOSTREAM
    ///Declare friend so it can access the data pointer
    template<class U>
    friend std::ostream& operator<<(std::ostream&,const SPA<U>&);
    #endif

    ///Declare friend so it can access the data pointer
    friend size_t strlen(const SPA<const char>);
    ///Declare friend so it can access the data pointer
    template<class U> friend void memcopy(SPA<U>,SPA<const U>,size_t);
    ///Declare friend so it can access the data pointer
    template<class U> friend void memcopy(U*,SPA<const U>,size_t);
    ///Declare friend so it can access the data pointer
    template<class U> friend void memcopy(SPA<U>,const U*,size_t);
};

/// A override of strlen for SPAs
/**
 * @param s the string to get the length of
 * @return the length of the string in s
 */
inline size_t strlen(const SPA<const char> s){
   return std::strlen(s.p);
}

/// A type aware memcpy like function
/**
 * This is the version that actually calls memcpy. All others call this either
 * directly or indirectly.
 * @param dest the destination for the data
 * @param src the source for the data
 * @param num the number of elements to copy
 */
template<class U>
inline void memcopy(U* dest,const U* src,size_t num){
  std::memcpy(dest,src,num*sizeof(U));
}
/// A type aware memcpy like function
/**
 * @param dest the destination for the data
 * @param src the source for the data
 * @param num the number of elements to copy
 */
template<class U>
inline void memcopy(SPA<U> dest,SPA<const U> src,size_t num){
  memcopy(dest.p,src.p,num);
}
/// A type aware memcpy like function
/**
 * @param dest the destination for the data
 * @param src the source for the data
 * @param num the number of elements to copy
 */
template<class U>
inline void memcopy(U* dest,SPA<const U> src,size_t num){
  memcopy(dest,src.p,num);
}
/// A type aware memcpy like function
/**
 * @param dest the destination for the data
 * @param src the source for the data
 * @param num the number of elements to copy
 */
template<class U>
inline void memcopy(SPA<U> dest,const U* src,size_t num){
  memcopy(dest.p,src,num);
}
/// A type aware memcpy like function
/**
 * @param dest the destination for the data
 * @param src the source for the data
 * @param num the number of elements to copy
 */
template<class U>
inline void memcopy(SPA<U> dest,SPA<U> src,size_t num){
  memcopy(dest,SPA<const U>(src),num);
}
/// A type aware memcpy like function
/**
 * @param dest the destination for the data
 * @param src the source for the data
 * @param num the number of elements to copy
 */
template<class U>
inline void memcopy(U* dest,SPA<U> src,size_t num){
  memcopy(dest,SPA<const U>(src),num);
}
/// A type aware memcpy like function
/**
 * @param dest the destination for the data
 * @param src the source for the data
 * @param num the number of elements to copy
 */
template<class U>
inline void memcopy(SPA<U> dest,U* src,size_t num){
  memcopy(dest,(const U*)src,num);
}

#ifdef IOSTREAM
/// write a representation of the contents of a SP to a stream
/**
 * If DEBUDPOINTER is set then output the representation of the contents along 
 * with the reference count, otherwise just print the contents
 * @param o the stream to write to
 * @param p the pointer to write
 * @return the stream o
 */
template <class U>
std::ostream& operator<<(std::ostream& o,const SP<U>& p){
#ifdef DEBUGPOINTER
  return o<<"<SP: "<<p.p<<" ("<<*p.c<<")>";
#else
  return o<<p.p;
#endif
}
/// write a representation of the contents of a SP to a stream
/**
 * If DEBUGPOINTER is set then output the representation of the contents along 
 * with the head location and reference count, otherwise just print the contents
 * @param o the stream to write to
 * @param p the pointer to write
 * @return the stream o
 */
template <class U>
std::ostream& operator<<(std::ostream& o,const SPA<U>& p){
#ifdef DEBUGPOINTER
  return o<<"<SPA: "<<p.p<<" from "<<(void*)p.h<<" ("<<*p.c<<")>";
#else
  return o<<p.p;
#endif
}
#endif
#endif
