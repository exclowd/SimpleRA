//
// Created by exclowd on 9/26/21.
//


#include <bits/stdc++.h>

using namespace std;

int main() {
    srand(time(0));
    const int n = 1000;
    cout << "A,B,C,D,E,F" << endl;
    for (int i = n - 1; i >= 0; i--) {
        for (int j = 0; j < 6; j++) {
            if (j != 0) printf(",");
            printf(" %d", i * 6 + j);
        }
        printf("\n");
    }
}
