//
// Created by exclowd on 9/26/21.
//


#include <bits/stdc++.h>

using namespace std;

int main() {
    const int n = 1000;
    for (int i = 0; i < n; i++) {
        printf("0");
        for (int j = 1; j < n; j++) {
            if (j != 0) {
                printf(",%d", i+j+rand()%10);
            }
        }
        printf("\n");
    }
}
