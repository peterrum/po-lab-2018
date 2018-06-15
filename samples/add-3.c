int test() {
    int a = 1; a++; // a={2}
    int b = 2; b++; // b={3}
    int c = 3; c++; // c={4}
    
    if(c != a){     
        c += 1;     // c={5} 
        a = c;      // a={5}
    }
    
                    // a={5}
    return a + b;   // ret={8}
}