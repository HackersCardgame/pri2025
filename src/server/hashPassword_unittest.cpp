#include <iostream>

#include "hashPassword.cpp"

int main(int argc, char* argv[]) {

    cout << argc << endl;
    
    if ( argc == 2 )
      cout << "sha256("<< argv[1] << ") = " << sha256(argv[1]) << endl;
    else
    cout << "sha256("<<"123456" << ") = " << sha256("123456") << endl;

    return 0;
}
