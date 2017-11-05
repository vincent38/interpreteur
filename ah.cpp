#include <iostream>
using namespace std;

int main() {

    //Variables utilisateur
    int a;
    int b;
    int c;

    //Programme généré
    cin >> a;

    a = a * a;
    b = a + 5;
    while (a > 4) {
        a = a - 1;
    }

    cout << "resultat :" << a << endl;

    if (a == b + 1) {
        a = 2;
    } else if (a == b + 2) {
        a = 3;
    } else {
        a = 1256;
    }

    c = 1;
    cout << c << endl;

    c++;
    cout << c << endl;

    return 0;
}
