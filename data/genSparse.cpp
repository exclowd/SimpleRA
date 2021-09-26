//
// Created by exclowd on 9/26/21.
//


#include <bits/stdc++.h>

using namespace std;

int main() {
    const int n = 1000;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (j != 0) printf(",");
            if (rand() % 10 < 2) {
                printf("1");
            } else {
                printf("0");
            }
        }
        printf("\n");
    }
}
