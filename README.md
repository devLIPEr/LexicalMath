#### **How to use**

```
g++ lexical.cpp -o lexical

lexical.out input.calc
```

The analyzer can understand integers and floats (4.1, 2.), both as number, and has support to + - * / ^ operators (keywords), as well to ( ) (punctuation).

The language to be analyzed accepts spaces, tabulations and new lines between operands and operators/punctuation.

If it's unable to understand something, such as "3a + 5", where a doesn't belong to the language, it is marked as rejected.
