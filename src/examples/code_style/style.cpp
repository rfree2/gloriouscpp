#include <string>
#include <iostream>

#include <glor/system/utils.cpp>

using namespace glor::system;

namespace glor {

namespace example {

using namespace std;

class c_entity { };

class c_human : public c_entity { } ;

int global_now_time; // but better to not use global

class c_man : public c_human {
	private:
		void foo();
		void bar(int x, int y);

		int m_beer;
		int m_money;
		string m_name;

	public:
		c_man(const string& name);
		c_man(const string& name, int age, int money, int luck);

		void drive_to(int max_speed);
		void print(string str);
};


// use shorter (one-line) or longer syntax (with newline, ident, and ending brace in newline)
// depending on size/complexity of the code

c_man::c_man(const string& name) : m_beer(0), m_name(name) { } // trivial = oneliner

c_man::c_man(const string& name, int age, int money, int luck) : 
	m_beer( age + money*luck ), 
	m_money(money), 
	m_name(name) 
{ 
	cout << "Created human (advanced)" << endl;

	if (age > 18) {
		foo();
		bar(age,luck);
	}

	if (age > 25) { m_money *= 2; } // even for simple one-line one-instruction if-then lines, use braces { }
	if (m_beer > 100) { m_money -= 100; }
}


void c_man::foo() { }

void c_man::bar(int x, int y) { 
	int x_new = x;
	int c = x_new + y;
	_info("c = " << c);
}

void test() {
	_mark("Running the test.");
	c_man bob("Bob"), dave("Dave");
	c_man the_other_guy("James");
}

} // namespace 
} // namespace


