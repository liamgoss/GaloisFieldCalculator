#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>
#include "boost/dynamic_bitset.hpp"
#include <algorithm>
#include <iterator>
#include <string>
using namespace std;

/*
Write a simple four-function calculator (+, -, *, /) in GaloisField (2^4) using C/C++. You may use table lookups
for the multiplicative inverses. You may represent elements using binary string format (that is, x^3+1 would be 1001).
*/



class fieldElement {
    private:
        int bitwidth = 4;
        boost::dynamic_bitset <uint32_t> value;// = boost::dynamic_bitset <uint32_t> (bitwidth, 0);
        boost::dynamic_bitset <uint32_t> definingPolynomial;
    public:

        int getTrailingZeros(boost::dynamic_bitset <uint32_t> bits) {
            int count = 0;
            for (int i=0; i<bits.size(); i++) {
                if (bits[i] == 0) {
                    count++;
                } else {
                    break;
                }
            }

            return count;
        }


        fieldElement fieldElementGCD(fieldElement u, fieldElement v) {
            
            if ((int)u.value.to_ulong() == 0) {
                return v;
            } else if ((int)v.value.to_ulong() == 0) {
                return u;
            }
            // Rust implementation: https://en.wikipedia.org/wiki/Binary_GCD_algorithm
            
            /*
                Important identities
                1. gcd(0,v) = v
                2. gcd(2u,2v) = 2gcd(u,v)
                3. gcd(2u,v) = gcd(u,v) if v is odd
                   gcd(u,2v) = gcd(u,v) if u is odd
                4. gcd(u,v) = gcd(|u-v|, min(u, v)) if u and v are both odd
            */ 
            
            int i = getTrailingZeros(u.value);
            int j = getTrailingZeros(v.value);

            u.value = u.value >> i;
            v.value = v.value >> j;
            int k = min(i,j);

            while ((int)u.value.to_ulong() % 2 == 1 && (int)v.value.to_ulong() % 2 == 1) {
                if (u.value > v.value) {
                    // swap u and v so that u <= v
                    fieldElement tmp = u;
                    u = v;
                    v = tmp;
                }
                v.value = v.value - u.value;
                if ((int)v.value.to_ulong() == 0) {
                    fieldElement returnVal(u.bitwidth, (int)(u.value << k).to_ulong(), (int)u.definingPolynomial.to_ulong());
                    return returnVal;
                }

                v.value = v.value >> getTrailingZeros(v.value);
            }



        }

        boost::dynamic_bitset <uint32_t> getValue () {
            return value;
        }
        void setValue (int newVal) {
            value = boost::dynamic_bitset <uint32_t> (bitwidth, newVal);
        }
        int getWidth () {
            return bitwidth;
        }
        void setWidth (int newSize) {
            bitwidth = newSize;
        }


        // The following functions are overloads of the +,-,*,/ operators such that they
        // are performed according to their respective polynomial operations over the finite field

        // Addition and subtraction in a binary extension field are identical to the bitwise XOR of the binary representations of the polynomials
        boost::dynamic_bitset <uint32_t> operator + (fieldElement const &input) {
            cout << input.value << "+" << value << "=" << (input.value ^ value) << endl;
            return input.value ^ value;
        }
        boost::dynamic_bitset <uint32_t> operator - (fieldElement const &input) {
            cout << input.value << "+" << value << "=" << (input.value ^ value) << endl;
            return input.value ^ value;
        }

        //Multiplication Operator that is currently working
        boost::dynamic_bitset <uint32_t> operator * (fieldElement const &input) {

            boost::dynamic_bitset <uint32_t> product(bitwidth, 0);
            boost::dynamic_bitset <uint32_t> temp = value;
            for (int i =0; i < bitwidth; i++) {
                if (input.value[i])
                    product = product ^ temp;
                if(temp[bitwidth-1] == 1)
                    temp = (temp << 1) ^ definingPolynomial;
                else
                    temp = (temp << 1);
            }
            return product;
        }

        //Division Operator NOT WORKING
        boost::dynamic_bitset <uint32_t> operator / (fieldElement const &input) {
            return input.value;
        }

        fieldElement(int width, int val, int poly) {
            // The preferred constructor; specifies both bitwidth and value of the element, and the defining polynomial of the field
            bitwidth = width;
            value = boost::dynamic_bitset <uint32_t> (bitwidth, val);
            definingPolynomial = boost::dynamic_bitset <uint32_t> (bitwidth, poly);
        }

        fieldElement(int width, int val) {
            // Specifies both bitwidth and value of the element
            bitwidth = width;
            value = boost::dynamic_bitset <uint32_t> (bitwidth, val);
        }
        fieldElement(int width) {
            // If only one parameter is specified, assume its the bitwidth of the element representations
            bitwidth = width;
            value = boost::dynamic_bitset <uint32_t> (bitwidth, 0); // Default the value to 0 for now
        }
        fieldElement() {
            // Default constructor; default to 4 bit values of 0
            bitwidth = 4;
            value = boost::dynamic_bitset <uint32_t> (bitwidth, 0);
        }

}; // end fieldElement class


class GaloisField {

private:
    int degree = 3; // m where GaloisField(2^m)
    int elementBitSize = degree; // number of bits needed to represent the polynomial elements
    int polynomialVal = 19; // Defaults to defining polynomial of x^4+x+1 (10011)
    vector<fieldElement> elements; // vector to hold field elements

    // Create 2^(fieldSize) many binary representations of the polynomials
    void defineFieldValues() {
        for (int i=0; i<pow(2, elementBitSize); i++) {
            fieldElement element_i(elementBitSize, i, polynomialVal);
            elements.push_back(element_i);
        }
    }



public:
    // Move to private after testing
    int polynomialStringToInt(string polynomial) {

        // Note: Can possibly copy or modify the code at the bottom of this page: https://cplusplus.com/forum/general/118352/

        /*
            1. find all "x^"
                a) also find just "x" without a "^", this is x^1s
            2. find all chars on the left side of "x^"
                *) This will be the coefficent of the term
                *) Make sure it isn't an operator or whitespace or outside the bounds
            3. find all chars on the right side of "x^"
                *) This will be the power for each term
            4. Mod 2 all coefficients (to keep it in binary extension field {0, 1})
            5. Mod p(x) polynomial to keep it in range
            6. Convert to binary representation (outside of this function or change the return type)
        */


        // Get index of every x
        vector<int> xIndices;
        vector<int> xCoefficients;
        vector<int> xPowers;
        cout << "x at ";
        for (int i=0; i<polynomial.size(); i++) {
            if (polynomial[i] == 'x') {
                xIndices.push_back(i);
                cout << i;
            }
        }
        cout << endl;
        for (auto it: xIndices) {
            vector<char> blacklist = {'+', '-', '*', '/', ' '};
            if (it == 0) {
                // No leading coefficient on first term
                xCoefficients.push_back(0);
            } else {
                if (find(blacklist.begin(), blacklist.end(), polynomial[it-1]) != blacklist.end()) {
                    // If the char before x is not a number, its not a coefficient

                    // Not sure if this is the most efficient way to do this, I'm open to scrapping this function entirely
                }
            }
        }
        return 0;
    }

    int getDegree() {
        return degree;
    }
    void setDegree(int m){
        degree = m;
    }
    int getElementBitSize() {
        return elementBitSize;
    }
    void setElementBitSize(int s) {
        elementBitSize = s;
    }
    int getPolynomialVal() {
        return polynomialVal;
    }

    vector<fieldElement> getElements() {
        return elements;
    }


    // Overload the [] operator so that you can do GaloisField[0] to get the 0th fieldElement object
    fieldElement& operator[] (int index) {
        return elements[index];
    }

    void binaryToPolynomial(boost::dynamic_bitset <uint32_t> bits) {
        string lineOut = "";
        if (bits == boost::dynamic_bitset <uint32_t>(elementBitSize, 0)) {
            cout << "0" << endl;
        } else {
            for (int i=bits.size()-1; i>=0; i--) {
                if (bits[i] == 1) {
                    if (i == 0) {
                        lineOut = lineOut + "1";
                        //cout << "1"; // Anything to the 0th power is 1
                    } else if (i == 1) {
                        lineOut = lineOut + "x";
                        //cout << "x";
                    } else {
                        lineOut = lineOut + "x^" + to_string(i);
                        //cout << "x^" << i;
                    }
                if (i > 0) {
                        lineOut = lineOut + " + ";
                        //cout << " + ";
                    }
                }
            }
            // This part right here removes the trailing plus sign at the end of some lines
            if (lineOut[lineOut.size()-2] == '+') {
                cout << lineOut.substr(0,lineOut.size()-3);
            } else {
                cout << lineOut;
            }

            cout << endl;
        }
    }

    void printFieldValues() {
        for (int i=0; i<elements.size(); i++) {
            cout << elements[i].getValue() << endl;
        }
    }

    /**
     * Galois Field Class Constructor
     *
     * Define the Galois Field of a base of 2 with a degree m (i.e. GaloisField(2^m))
     *
     * @param m Degree of the polynomial of base 2
     * @param poly Custom irreducible polynomial represented in decimal (i.e. 13 for x^3+2+1 [1101 which is 13])
     */
    GaloisField (int m, int poly) {
        // If custom polynomial is desired, this constructor will be executed
        degree = m;
        elementBitSize = m;
        polynomialVal = poly;
        defineFieldValues();
    }

    // Default constructor
    GaloisField () {
        degree = 3;
        polynomialVal = 13;
        defineFieldValues();
    }


};




int main() {
    // GaloisField(2^4) defined by p(x) = x^4 + x + 1; p(x)=10011 which is 19
    int m = 4; // input m for degree of field GaloisField(2^m)
    //int definingPolynomial = 19;
    int definingPolynomial = 19; // x^4+x+1
                                 // 10011 = 19
    GaloisField field(m, definingPolynomial);

    //field.polynomialStringToInt("x^2+2x+1");

    fieldElement element2 = field[11]; // x^2
    fieldElement element5 = field[4]; // x
    //boost::dynamic_bitset<uint32_t> product = boost::dynamic_bitset<uint32_t>(4, );

    fieldElement poly(m, definingPolynomial, definingPolynomial);
    cout << "gcd(" << element2.getValue() << "," << poly.getValue() <<") = " << element2.fieldElementGCD(element2, poly).getValue() << endl;
    //cout << element2.getValue() << "*" << element5.getValue() << "=" << element2 * element5 << endl;
    /*
    for (int i=0; i<pow(2, m); i++){
        field.binaryToPolynomial(field[i].getValue());
    }
    */

    // cout << field.getDegree() << endl;
    // cout << field.getElementBitSize() << endl;
    // field.printFieldValues();


    return 0;
}
