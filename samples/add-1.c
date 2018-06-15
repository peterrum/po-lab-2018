int test() {
    int a = 1; a++; // a={2}
    int b = 2; b++; // b={3}
    int c;
    c += 1;         // c=T
    
    if(c == a){     // c=a={2}
        c += 1;     // c={3}
        a = c;      // a={3}
    }
    
                    // a={2,3}
    return a + b;   // ret={5,6}
}