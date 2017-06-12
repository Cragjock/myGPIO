#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include "myGPIO.h"

using namespace std;

myGPIO::myGPIO(int g_number, GPIO_DIRECTION g_dir, GPIO_VALUE g_value, GPIO_EDGE g_edge)
{
  	this->gpio_number = g_number;
  	this->gpio_direction = g_dir;
  	this->gpio_value = g_value;
  	this->gpio_edge = g_edge;
	this->gpio_export();
    this->gpio_setdir(g_dir);
    this->gpio_setvalue(g_value);

    stringstream sss;
    sss<<g_number;
    this->s_gpio_number = sss.str();
}

myGPIO::myGPIO()
{
	this->gpio_number = 26;
	this->gpio_export();
}

myGPIO::myGPIO(int number)
{
	this->gpio_number = number;
	this->gpio_export();
	this->s_gpio_number = to_string(number); /// c++ 11 way

}

myGPIO::myGPIO(GPIO_DIRECTION d_gpio)
{
	this->gpio_direction = d_gpio;
}


myGPIO::~myGPIO()
{
	this->gpio_unexport();
	cout<<"in destructor "<<endl;
}


int myGPIO::gpio_export()
{
	string export_str = "/sys/class/gpio/export";
	ofstream exportgpio(export_str.c_str()); // Open "export" file. Convert C++ string to C string. Required for all Linux pathnames

	if (exportgpio < 0)
        {
		cout << " OPERATION FAILED: Unable to export GPIO"<< this->gpio_number <<" ."<< endl;
		return -1;
        }

	exportgpio << this->gpio_number;
    exportgpio.close();
    return 0;
}

int myGPIO::gpio_unexport()
{
    stringstream ss;
    ss << (this->gpio_number);

	string unexport_str = "/sys/class/gpio/unexport";
	ofstream unexportgpio(unexport_str.c_str()); //Open unexport file
	if (unexportgpio < 0){
		cout << " OPERATION FAILED: Unable to unexport GPIO"<< this->gpio_number <<" ."<< endl;
		return -1;
	}

	//unexportgpio << ss.str(); //write GPIO number to unexport
	unexportgpio << this->gpio_number ; //write GPIO number to unexport
	cout<<"unexport: "<<unexport_str.c_str()<<endl;
    unexportgpio.close(); //close unexport file
    return 0;
}

int myGPIO::gpio_setdir(GPIO_DIRECTION direction)
{
    string which_way = "out";       //default for me;
    if (direction == OUTPUT)
        which_way = "out";
    else
        which_way = "in";

    this->gpio_direction = direction;
	string setdir_str ="/sys/class/gpio/gpio" + to_string(this->gpio_number) + "/direction"; /// needs c++ 11 to_string(number);
	ofstream setdirgpio(setdir_str.c_str()); // open direction file for gpio
		if (setdirgpio < 0){
			cout << " OPERATION FAILED: Unable to set direction of GPIO"<< this->gpio_number <<" ."<< endl;
			return -1;
		}
    setdirgpio << which_way; //write direction to direction file
    cout<<"set gpio dir: "<<setdir_str.c_str()<<endl;
    setdirgpio.close(); // close direction file
    return 0;
}


int myGPIO::gpio_setvalue(GPIO_VALUE value)
{
    string which_value = "0";       //default for me;
    if (value == HIGH)
        which_value = "1";
    else
        which_value = "0";

	this->gpio_value = value;
	string setval_str = "/sys/class/gpio/gpio" + to_string(this->gpio_number) + "/value";
	ofstream setvalgpio(setval_str.c_str()); // open value file for gpio
		if (setvalgpio < 0){
			cout << " OPERATION FAILED: Unable to set the value of GPIO"<< this->gpio_number <<" ."<< endl;
			return -1;
		}
    setvalgpio << which_value;//write value to value file
    setvalgpio.close();// close value file
    return 0;
}

int myGPIO::gpio_set_edge(GPIO_EDGE edge)
{
    string s_edge ="falling";
    switch(edge)
    {
        case NONE:
            this->gpio_edge = edge;
            s_edge = "none";
            //return this->write(this->path, "edge", "none");
        break;
        case RISING:
            this->gpio_edge = edge;
            s_edge = "rising";
            //return this->write(this->path, "edge", "rising");
        break;
        case FALLING:
            this->gpio_edge = edge;
            s_edge = "falling";
            //return this->write(this->path, "edge", "falling");
        break;
        case BOTH:
            this->gpio_edge = edge;
            s_edge = "both";
            //return this->write(this->path, "edge", "both");
        break;
   }

	this->gpio_edge = edge;
    stringstream ss;
    ss << this->gpio_number;
    //ss.str();

	string setval_str = "/sys/class/gpio/gpio" + ss.str() + "/edge";
	//string setval_str = "/sys/class/gpio/gpio" + this->gpio_number + "/edge";
	ofstream setedge(setval_str.c_str()); // open value file for gpio
		if (setedge < 0){
			cout << " OPERATION FAILED: Unable to set the value of GPIO"<< this->gpio_number <<" ."<< endl;
			return -1;
		}
		setedge << s_edge ;//write value to value file
		setedge.close();// close value file

    return 0;
}

GPIO_VALUE myGPIO::gpio_getvalue()
{
    string value;
    int i_value;
    stringstream ss;
    ss << this->gpio_number;

	string getval_str = "/sys/class/gpio/gpio" + ss.str() + "/value";
	//string getval_str = "/sys/class/gpio/gpio" + this->gpio_number + "/value";
	ifstream getvalgpio(getval_str.c_str());// open value file for gpio
	if (getvalgpio < 0)
        {
		cout << " OPERATION FAILED: Unable to get value of GPIO"<< this->gpio_number <<" ."<< endl;
		return LOW; // -1;
        }

    cout<<"at read value"<<endl;

	//getvalgpio >> i_value ;  //read gpio value
	//cout<<"after read value"<<i_value<<endl;

	getvalgpio >> value ;  //read gpio value
	cout<<"after read value: "<<value<<" from "<<this->gpio_number<<endl;


	getvalgpio.close(); //close the value file

	if(value != "0")
    {
        value = "1";
        return HIGH;
    }

	else
    {
        value = "0";
        return LOW;
    }
    return LOW; //stoi( value );
}


int myGPIO::gpio_getname()
{
    return this->gpio_number;
}



