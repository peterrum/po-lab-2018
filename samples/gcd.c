int gcd() {
    int a = 7387;
    int b = 8633;
    int r;
    while (b > 0) {
        r = a % b;
        a = b;
        b = r;
    }

    // expected solution 89
    return a;
}