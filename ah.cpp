    int main() {
int a;
int b;
     cin >> a;
       a = a * a;
       b = a + 5;
 while (a > 4) {
           a = a - 1;
       }
    cout << "resultat :" << a;
    if (a == b + 1) {
           a = 2;
       } else if (a == b + 2) {
           a = 3;
       } else {
           a = 1256;
       }
        return 0;
   }
