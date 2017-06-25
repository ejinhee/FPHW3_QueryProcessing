#pragma once
#include <iostream>
using namespace std;
class Professor {
public:
	char prof_name[20];
	int prof_id;
	int prof_salary;
	Professor() {

	}
	void setName(char src[20]) {
		copy(src, src + 20, prof_name);
	}
	void setProfID(int profid) {
		this->prof_id = profid;
	}
	void setSalary(int salary) {
		this->prof_salary = salary;
	}
	void print() {
		cout << this->prof_name << " " << this->prof_id << " " << this->prof_salary << endl;
	}
};

class Student {
public:
	char stu_name[20];
	int stu_id;
	float stu_score;
	int stu_advisorid;

	Student();
	void setName(char src[20]);
	void setStuID(unsigned stuid);
	void setScore(float score);
	void setAdvisorID(unsigned advid);

};
Student::Student() {

}
void Student::setName(char src[20]) {
	copy(src, src + 20, stu_name);
}
void Student::setStuID(unsigned stuid) {
	this->stu_id = stuid;
}
void Student::setScore(float score) {
	this->stu_score = score;
}
void Student::setAdvisorID(unsigned advid) {
	this->stu_advisorid = advid;
}