int gcd() {
    unsigned a = 7387;
    unsigned b = 8633;
    unsigned r;
    while (b > 0) {
        r = a % b;
        a = b;
        b = r;
    }

    // expected solution 89
    return a;
}
