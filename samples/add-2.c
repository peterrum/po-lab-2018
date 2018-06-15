int test() {
    int a = 1; a++; // a={2}
    int b = 2; b++; // b={3}
    int c = 3; c++; // c={4}
    
    if(c == a){     // bottom
        c += 1;     // 
        a = c;      // 
    }
    
                    // a={2}
    return a + b;   // ret={5}
}