min := function (a b) {
        if (a < b) { a }
        else       { b }
};

factorial := function (n) {
        if (n < 2) { 1 }
        else       { n * factorial(n - 1) }
};

wfactorial := function (n) {
        acc := 1;
        while (n > 1)
        {
                acc = acc * n;
                n = n - 1;
        }
        
        acc
};

print(factorial(7));
print(wfactorial(7));

str := "Hello, World!";
str[0] = 65;
print(str);
print("Hello, World!: min(2 3) = " min(2 3));
print(if ("ABC" is [65 66 67]) { "true" } else { "false" });
print(11 % 10)
