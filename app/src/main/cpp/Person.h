//
// Created by JerryZhu on 2018/5/31.
//

#ifndef HELLO_GL2_PERSON_H
#define HELLO_GL2_PERSON_H


#include <string>
class Person {

private:
    std::string name;
    int age;

public:
    virtual ~Person();

    Person(const std::string &name, int age);

    std::string getName();
    int getAge();
    void setName(std::string name);
    void setAge(int age);

};


#endif //HELLO_GL2_PERSON_H
