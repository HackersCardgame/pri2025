
#include <chrono>
#include <iostream>
#include <future>
#include <string>

#include <thread>
#include <chrono>

#include <cstdlib>
#include <cstdio>
#include <stdio.h>


#include "../lib/colorize/color.hpp"

using namespace std;

int main(int argc, char **argv)
{
  cout << color::rize("red blinking on white", "Red", "White", "Blink") << endl;
  cout << color::rize("red blinking on default", "Red", "Defualt", "Blink") << endl;
  cout << color::rize("white blinking on red", "White", "Red", "Blink") << endl;
  cout << color::rize("red blinking on white, BOLD", "Red", "White", "Bold", "Blink");
  cout << color::rize("red blinking on white", "Red", "White", "Blink") << endl;
  cout << color::rize("red blinking on white", "Red", "White", "Blink") << endl;
}




