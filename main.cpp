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
    public:

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

        boost::dynamic_bitset <uint32_t> operator * (fieldElement const &input) {     
            // TODO: Implement multiplication
            return input.value;
        }

        boost::dynamic_bitset <uint32_t> operator / (fieldElement const &input) {
            // TODO: Implement division
            return input.value;
        }

        fieldElement(int width, int val) {
            // The preferred constructor; specifies both bitwidth and value of the element
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
    int degree = 4; // m where GaloisField(2^m)
    int elementBitSize = degree; // number of bits needed to represent the polynomial elements
    int polynomialVal = 19; // Defaults to defining polynomial of x^4+x+1 (10011)
    vector<fieldElement> elements; // vector to hold field elements
    
    // Create 2^(fieldSize) many binary representations of the polynomials
    void defineFieldValues() {
        for (int i=0; i<pow(2, elementBitSize); i++) {
            fieldElement element_i(elementBitSize, i);
            elements.push_back(element_i);
        }
    }

public:

    int getDegree() {
        return degree;
    }
    int getElementBitSize() {
        return elementBitSize;
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
    int definingPolynomial = 19;
    GaloisField field(m, definingPolynomial); 

    for (int i=0; i<pow(2, m); i++){
        field.binaryToPolynomial(field[i].getValue());
    }

    // cout << field.getDegree() << endl;
    // cout << field.getElementBitSize() << endl;
    // field.printFieldValues();


    return 0;
}