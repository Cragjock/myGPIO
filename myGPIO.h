
#ifndef MY_GPIO_H
#define MY_GPIO_H

#include <string>

using std::string;

#define GPIO_PATH "/sys/class/gpio/"
enum GPIO_DIRECTION{INPUT,OUTPUT};
enum GPIO_VALUE{LOW=0, HIGH=1};
enum GPIO_EDGE{NONE,RISING,FALLING,BOTH};

typedef struct tag_gpio
{
    int number;
    GPIO_DIRECTION dir;


}gpio_data;


class myGPIO
{
public:
    myGPIO();
    myGPIO(int g_number);
    myGPIO(int g_number, GPIO_DIRECTION g_dir, GPIO_VALUE g_value, GPIO_EDGE g_edge);
    myGPIO(GPIO_DIRECTION d_gpio);
    ~myGPIO();
    int gpio_setdir(GPIO_DIRECTION dir);
    int gpio_setvalue(GPIO_VALUE val);
    GPIO_VALUE gpio_getvalue();
    int gpio_set_edge(GPIO_EDGE edge);
    int gpio_getname();
    string gpio_string_name();
private:
    int gpio_export();
    int gpio_unexport();
    int gpio_number;
    string s_gpio_number;
    int gpio_direction;
    int gpio_value;
    int gpio_edge;
    string gpio_path;
};

#endif
