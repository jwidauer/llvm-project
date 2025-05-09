// RUN: %clang_analyze_cc1 -analyzer-checker=webkit.RefCntblBaseVirtualDtor -verify %s

namespace WTF {

class NoVirtualDestructorBase { };

};

using WTF::NoVirtualDestructorBase;

struct RefCntblBase {
  void ref() {}
  void deref() {}
};

struct Derived : RefCntblBase { };
// expected-warning@-1{{Struct 'RefCntblBase' is used as a base of struct 'Derived' but doesn't have virtual destructor}}

struct DerivedWithVirtualDtor : RefCntblBase {
// expected-warning@-1{{Struct 'RefCntblBase' is used as a base of struct 'DerivedWithVirtualDtor' but doesn't have virtual destructor}}
  virtual ~DerivedWithVirtualDtor() {}
};

// Confirm that the checker respects [[clang::suppress]]
struct [[clang::suppress]] SuppressedDerived : RefCntblBase { };
struct [[clang::suppress]] SuppressedDerivedWithVirtualDtor : RefCntblBase {
  virtual ~SuppressedDerivedWithVirtualDtor() {}
};

class ClassWithoutVirtualDestructor : public NoVirtualDestructorBase {
public:
  void ref() const;
  void deref() const;
};

class DerivedClassWithoutVirtualDestructor : public ClassWithoutVirtualDestructor {
};

// FIXME: Support attributes on base specifiers? Currently clang
// doesn't support such attributes at all, even though it knows
// how to parse them.
//
// struct SuppressedBaseSpecDerived : [[clang::suppress]] RefCntblBase { };

template<class T>
struct DerivedClassTmpl : T { };
typedef DerivedClassTmpl<RefCntblBase> Foo;



struct RandomBase {};
struct RandomDerivedClass : RandomBase { };



struct FakeRefCntblBase1 {
  private:
  void ref() {}
  void deref() {}
};
struct Quiet1 : FakeRefCntblBase1 {};

struct FakeRefCntblBase2 {
  protected:
  void ref() {}
  void deref() {}
};
struct Quiet2 : FakeRefCntblBase2 {};

class FakeRefCntblBase3 {
  void ref() {}
  void deref() {}
};
struct Quiet3 : FakeRefCntblBase3 {};
struct Quiet4 : private RefCntblBase {};
class Quiet5 : RefCntblBase {};

void foo () {
  Derived d;
}
