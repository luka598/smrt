#include <iostream>
#define BESERT_BEGIN
#define BESERT_NONFATAL
#define BESERT_BORDERS
#include "besert.hpp"
#include "../smrt.hpp"

class A : public smrt::Base {
	public:
	int _x;
	virtual int x() {return _x;};
	virtual void ix() {_x += 1;};
};

class B : public A {
	public:
	int _x = 1;
	virtual int x() {return _x;};
	virtual void ix() {_x += 1;};
};

class C: public A {
	public:
	int _x = 2;
	virtual int x() {return _x;};
	virtual void ix() {_x += 1;};
};

class D {

};

int main(){
	auto a = smrt::make<A>();
	auto b = smrt::make<B>();
	auto c = smrt::make<C>();
	
	ASSERT(b->x() == 1);
	ASSERT(c->x() == 2);
	
	c->ix();
	ASSERT(c->x() == 3);

	ASSERT(b->bptr().as<B>()->x() == 1);
	ASSERT(b->bptr().as<A>()->x() == 1);
	// TODO: Actually write tests properly
}
