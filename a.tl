min := function (a b) {
        if (a < b) { a }
        else       { b }
};

factorial := function (n) {
        if (n < 2) { 1 }
        else       { n * factorial(n - 1) }
};

print(factorial(5));

str := "Hello, World!";
str[0] = 65;
print(str);
print("Hello, World!: min(2 3) = " min(2 3));
print("ABC" is [65 66 67]);
print(11 % 10)
