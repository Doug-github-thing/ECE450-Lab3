#include <iostream>
#include <ap_int.h>
#include <array>

void compute_c (ap_int<16> a, ap_int<16> b, ap_int<16> &c);

void test(ap_int<16> a, ap_int<16> b, bool expect_correct, ap_int<16> &c) {
    compute_c(a, b, c);

    bool correct = a*a - b*b == c*c;
    std::cout << ((expect_correct == correct) ? "Test case passed - " : "Test case failed - ");
    std::cout << (correct ? "c is valid" : "c is invalid");
    std::cout << ": a=" << a << ", b=" << b << " => c=" << c << std::endl;
}

struct ab_pair {
    ap_int<16> a;
    ap_int<16> b;
    bool pass;
};

int main() {
    ap_int<16> c;

    // Define the pairs to be tested one at a time
    ab_pair pairs[] = {
        // Test normal pythagorean triples - Expect PASS
        {0, 0, true},
        {1, 1, true},
        {5, 3, true},
        {181, 180, true},
        {29, 21, true},
        // Same cases, but negative now - Expect PASS
        {-5, -3, true},
        {-181, -180, true},
        {-29, -21, true},
        {5, -3, true},
        {181, -180, true},
        {29, -21, true},
        {-5, 3, true},
        {-181, 180, true},
        {-29, 21, true},
        // Valid Pythagorean triple where intermediate terms are greater than 16 bits
        {32500, 19500, true},
        {32500, -19500, true},
        {-32500, 19500, true},
        {-32500, -19500, true},
        // Valid Pythagorean triples too big to fit in the 16 bit signed range - Expect FAIL
        {65000, 52000, false},
        // Test non-pythagorean triples - Expect FAIL
        {15,7, false},
        {280, 16, false},
        {365, 363, false},
        {32500, 19501, false},
        // Cases where b^2 > a^2 - Expect FAIL
        {4, 5, false},
        {21, 29, false},
        {-4, -5, false},
        {-21, -29, false}
    };

    // Test each a/b pair at a time
    for (auto &p: pairs)
        test(p.a, p.b, p.pass, c);
    
    return 0;
}
