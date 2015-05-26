#include <string>
#include <iostream>

namespace glor {


namespace example {

using namespace std;

class c_entity { };
class cEntity { };
class Entity { };

class c_human : public c_entity { } ;
class cHuman : public cEntity { } ;
class Human : public Entity { } ;

int global_now_time;

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

c_man::c_man(const string& name) : m_beer(0), m_name(name) { }

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

	if (age > 25) { m_money *= 2; }
	if (m_beer > 100) m_money -= 100;
}

void test() {
	c_man bob("Bob"), dave("Dave");
	c_man the_other_guy("James");
}

} // namespace 
} // namespace


