# Snip — Scheme nano-interpreter project

> Lightweight, extensible, scientific, and powerful.

---

## ✨ Features

- **Minimalist core:**  
  Written in **~660 lines** of C++, all contained in a **single header file**.
- **Easy to integrate:**  
  Just include `snip.h` — no external libraries needed.
- **Homoiconic design:**  
  Code and data share the same structure, like Lisp and Scheme.
- **Tail-call optimization:**  
  Thanks to a carefully crafted `while`-based `eval`, recursion never grows the C++ call stack.
- **Macro system:**  
  Macros can manipulate unevaluated code, allowing elegant new syntactic forms like `let`, etc.
- **Basic scientific library built-in:**  
  Includes:
  - Basic machine learning: `kmeans`, `linear-regression`, `predict-linear`, `knn`
  - Basic signal processing: `fft`, `ifft`, `conv` (fast convolution), `dot`, `pol2car`, `car2pol`
  - Statistics: `mean`, `variance`, `stddev`, `distance`
- **CSV and WAV file I/O:**  
  Read and write multichannel `.csv` and `.wav` files easily.
- **Customizable environment:**  
  Extend the language by simply adding C++ functors.

---

## 📦 Philosophy

- **Small is beautiful:**  
  Focus on essential features only.
- **Understandable in a few hours:**  
  Core code is simple enough to read and understand easily.
- **No dependencies:**  
  No Boost, no external math libraries, no complex build systems.
- **Extensible:**  
  You can add primitives, types, and libraries incrementally.

---

## ⚡ Example: Hello Snip

```scheme
(define factorial
  (lambda (n)
    (if (eq? n 0)
        1
        (* n (factorial (- n 1))))))
        
(display (factorial 5))
;; => 120
```

---

## 🔬 Example: Basic machine learning

```scheme
(define x (list 1 2 3 4))
(define y (list 2 4 6 8))
(define model (linear-regression x y))
(predict-linear model (list 5 6))
;; => (10 12)
```

---

## 🎶 Example: Signal processing

```scheme
(define signal (range 0 16))
(define transformed (fft signal))
(define reconstructed (ifft transformed))
(display (take 10 reconstructed))
```

---

## 🛠️ Building and Using

Simply add:

```cpp
#include "snip.h"
```

in your project.  See [`snip.cpp`](snip.cpp) for a minimal example of embedding the interpreter.

Compile normally with any **C++17 or later** compiler.

---

## 🔥 Why Snip?

- Ideal for **educational use**.
- Perfect as an **embedded scripting language**.
- Great foundation for **experimental extensions** (AI, DSP, etc.).
- Tiny footprint but **serious capabilities**.

---

## 🌟 Roadmap (Possible Future Features)

- Memory pooling for even faster performance.
- Plotting capabilities (generate `.ps` or `.pdf` files).
- More scientific and machine learning primitives (neural networks, clustering, etc.).

---

# 🚀 Quick Start

Just add `snip.h` to your project, call `make_env()`, and start the `repl()`!

---

# 🕋 License

This project is licensed under the **BSD 2-Clause License**:

Copyright (c) 2025, Carmine-Emanuele Cella  
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
